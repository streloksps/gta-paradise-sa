#include "config.hpp"
#include "integer_expression_calc.hpp"
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <iostream>
#include <string>
#include <vector>

using boost::phoenix::function;
using boost::phoenix::ref;
using boost::phoenix::size;

using boost::spirit::qi::unused_type;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

# pragma warning(disable: 4800) // forcing value to bool 'true' or 'false' (performance warning)

///////////////////////////////////////////////////////////////////////////////
//  The Virtual Machine
///////////////////////////////////////////////////////////////////////////////
enum byte_code {
    op_neg,         //  negate the top stack entry
    op_add,         //  add top two stack entries
    op_sub,         //  subtract top two stack entries
    op_mul,         //  multiply top two stack entries
    op_div,         //  divide top two stack entries
    op_rem,         //  remainde top two stack entries (остаток от деления)

    op_not,         //  boolean negate the top stack entry
    op_eq,          //  compare the top two stack entries for ==
    op_neq,         //  compare the top two stack entries for !=
    op_lt,          //  compare the top two stack entries for <
    op_lte,         //  compare the top two stack entries for <=
    op_gt,          //  compare the top two stack entries for >
    op_gte,         //  compare the top two stack entries for >=

    op_and,         //  logical and top two stack entries
    op_or,          //  logical or top two stack entries

    op_load,        //  load a variable
    op_store,       //  store a variable

    op_int,         //  push constant integer into the stack
    op_true,        //  push constant 0 into the stack
    op_false,       //  push constant 1 into the stack

    op_jump_if,     //  jump to an absolute position in the code if top stack
    //  evaluates to false
    op_jump         //  jump to an absolute position in the code
};

class vmachine {
public:
    vmachine(unsigned stackSize = 4096)
        :stack(stackSize)
        ,stack_ptr(stack.begin())
    {
    }

    std::vector<int> const& get_stack() const {return stack;}
    int top() const {return stack_ptr[-1]; }
    void execute(std::vector<int> const& code, int nvars = 0) {
        std::vector<int>::const_iterator pc = code.begin();
        std::vector<int>::iterator locals = stack.begin();
        stack_ptr = stack.begin() + nvars;

        while (pc != code.end())
        {
            switch (*pc++)
            {
            case op_neg:
                stack_ptr[-1] = -stack_ptr[-1];
                break;

            case op_not:
                stack_ptr[-1] = !bool(stack_ptr[-1]);
                break;

            case op_add:
                --stack_ptr;
                stack_ptr[-1] += stack_ptr[0];
                break;

            case op_sub:
                --stack_ptr;
                stack_ptr[-1] -= stack_ptr[0];
                break;

            case op_mul:
                --stack_ptr;
                stack_ptr[-1] *= stack_ptr[0];
                break;

            case op_div:
                --stack_ptr;
                stack_ptr[-1] /= stack_ptr[0];
                break;

            case op_rem:
                --stack_ptr;
                stack_ptr[-1] %= stack_ptr[0];
                break;

            case op_eq:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1] == stack_ptr[0]);
                break;

            case op_neq:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1] != stack_ptr[0]);
                break;

            case op_lt:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1] < stack_ptr[0]);
                break;

            case op_lte:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1] <= stack_ptr[0]);
                break;

            case op_gt:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1] > stack_ptr[0]);
                break;

            case op_gte:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1] >= stack_ptr[0]);
                break;

            case op_and:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1]) && bool(stack_ptr[0]);
                break;

            case op_or:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1]) || bool(stack_ptr[0]);
                break;

            case op_load:
                *stack_ptr++ = locals[*pc++];
                break;

            case op_store:
                --stack_ptr;
                locals[*pc++] = stack_ptr[0];
                break;

            case op_int:
                *stack_ptr++ = *pc++;
                break;

            case op_true:
                *stack_ptr++ = true;
                break;

            case op_false:
                *stack_ptr++ = false;
                break;

            case op_jump:
                pc = code.begin() + *pc;
                break;

            case op_jump_if:
                if (!bool(stack_ptr[-1]))
                    pc = code.begin() + *pc;
                else
                    ++pc;
                --stack_ptr;
                break;
            }
        }
    }
private:
    std::vector<int> stack;
    std::vector<int>::iterator stack_ptr;
};

///////////////////////////////////////////////////////////////////////////////
//  A generic compiler that compiles 1 to 3 codes
///////////////////////////////////////////////////////////////////////////////
struct compile_op {
    template <typename A, typename B = unused_type, typename C = unused_type>
    struct result { typedef void type; };

    compile_op(std::vector<int>& code):code(code) {
    }

    void operator()(int a) const {
        code.push_back(a);
    }

    void operator()(int a, int b) const {
        code.push_back(a);
        code.push_back(b);
    }

    void operator()(int a, int b, int c) const {
        code.push_back(a);
        code.push_back(b);
        code.push_back(c);
    }

    std::vector<int>& code;
};

///////////////////////////////////////////////////////////////////////////////
//  Our error handler
///////////////////////////////////////////////////////////////////////////////
struct error_handler_
{
    template <typename, typename, typename>
    struct result { typedef void type; };

    template <typename Iterator>
    void operator()(
        qi::info const& what
        , Iterator err_pos, Iterator last) const
    {
        std::cout
            << "Error! Expecting "
            << what                         // what failed?
            << " here: \""
            << std::string(err_pos, last)   // iterators to error-pos, end
            << "\""
            << std::endl
            ;
    }
};

function<error_handler_> const error_handler = error_handler_();

///////////////////////////////////////////////////////////////////////////////
//  Our expression grammar and compiler
///////////////////////////////////////////////////////////////////////////////
template <typename Iterator>
struct expression : qi::grammar<Iterator, ascii::space_type>
{
    expression(std::vector<int>& code, qi::symbols<char, int>& vars)  : expression::base_type(expr)
        , code(code)
        , vars(vars)
        , op(code)
    {
        using qi::lexeme;
        using qi::lit;
        using qi::uint_;
        using namespace qi::labels;
        using qi::on_error;
        using qi::fail;
        using ascii::alnum;

        expr =
            equality_expr.alias()
            ;

        equality_expr =
            relational_expr
            >> *(   ("==" > relational_expr     [op(op_eq)])
                |   ("!=" > relational_expr     [op(op_neq)])
                )
            ;

        relational_expr =
            logical_expr
            >> *(   ("<=" > logical_expr        [op(op_lte)])
                |   ('<' > logical_expr         [op(op_lt)])
                |   (">=" > logical_expr        [op(op_gte)])
                |   ('>' > logical_expr         [op(op_gt)])
                )
            ;

        logical_expr =
            additive_expr
            >> *(   ("&&" > additive_expr       [op(op_and)])
                |   ("||" > additive_expr       [op(op_or)])
                )
            ;

        additive_expr =
            multiplicative_expr
            >> *(   ('+' > multiplicative_expr  [op(op_add)])
                |   ('-' > multiplicative_expr  [op(op_sub)])
                )
            ;

        multiplicative_expr =
            unary_expr
            >> *(   ('*' > unary_expr           [op(op_mul)])
                |   ('/' > unary_expr           [op(op_div)])
                |   ('%' > unary_expr           [op(op_rem)])
                )
            ;

        unary_expr =
            primary_expr
            |   ('!' > primary_expr             [op(op_not)])
            |   ('-' > primary_expr             [op(op_neg)])
            |   ('+' > primary_expr)
            ;

        primary_expr =
            uint_                               [op(op_int, boost::spirit::_1)]
            |   variable
            |   lit("true")                     [op(op_true)]
            |   lit("false")                    [op(op_false)]
            |   '(' > expr > ')'
            ;

        variable =
            (
            lexeme[
                vars
                    >> !(alnum | '_')           // make sure we have whole words
            ]
        )                                   [op(op_load, boost::spirit::_1)]
        ;

        expr.name("expression");
        equality_expr.name("equality-expression");
        relational_expr.name("relational-expression");
        logical_expr.name("logical-expression");
        additive_expr.name("additive-expression");
        multiplicative_expr.name("multiplicative-expression");
        unary_expr.name("unary-expression");
        primary_expr.name("primary-expression");
        variable.name("variable");

        on_error<fail>(expr, error_handler(boost::spirit::_4, boost::spirit::_3, boost::spirit::_2));
    }


    qi::rule<Iterator, ascii::space_type>
        expr, equality_expr, relational_expr
        , logical_expr, additive_expr, multiplicative_expr
        , unary_expr, primary_expr, variable
        ;

    std::vector<int>& code;
    qi::symbols<char, int>& vars;
    function<compile_op> op;
};


integer_expression_calc::integer_expression_calc(unsigned int vm_stack_size)
:vm_stack_size(vm_stack_size)
{
}

integer_expression_calc::~integer_expression_calc() {
}

bool integer_expression_calc::evaluate(std::string const& expr, int& rezult) {
    typedef expression<std::string::const_iterator> expression_t;
    vmachine mach(vm_stack_size);               //  Our virtual machine
    std::vector<int> code;                      //  Our VM code
    boost::spirit::qi::symbols<char, int> vars; //
    expression_t calc(code, vars);              //  Our grammar

    std::string::const_iterator iter = expr.begin(), end = expr.end();
    
    bool r = qi::phrase_parse(iter, end, calc, ascii::space);
    if (r && iter == end) {
        // Парсинг прошел успешно, производим подсчет и возращаем результат
        mach.execute(code, 0);
        rezult = mach.top();
        return true;
    }
    return false;
}
