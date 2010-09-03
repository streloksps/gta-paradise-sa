#ifndef INTEGER_EXPRESSION_CALC_HPP
#define INTEGER_EXPRESSION_CALC_HPP
#include <string>
class integer_expression_calc {
public:
    integer_expression_calc(unsigned int vm_stack_size = 128);
    ~integer_expression_calc();

    // Возращает истину, если удалось просчитать выражение и в rezult результат
    bool evaluate(std::string const& expr, int& rezult);
private:
    unsigned int vm_stack_size;
};
#endif // INTEGER_EXPRESSION_CALC_HPP

