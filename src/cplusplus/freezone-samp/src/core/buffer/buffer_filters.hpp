#ifndef BUFFER_FILTERS_HPP
#define BUFFER_FILTERS_HPP
#include "core/utility/stream_filters.hpp"
#include "buffer.hpp"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Возращает истину, если строка является оператором препроцессора
inline bool get_preprocessor_statement(std::string const& source, std::string& statement_name, std::string& statement_params) {
    // Оптимизация :)
    for (std::string::const_iterator it = source.begin(), end = source.end(); it != end; ++it) {
        char c = *it;
        if (' ' == c || '\t' == c) {
            continue;
        }
        if ('!' == c) {
            break;
        }
        return false;
    }

    std::string work_buff = boost::trim_copy(source);
    std::string const prefix = "!";
    if (boost::starts_with(work_buff, prefix) && work_buff.length() > prefix.length()) {
        std::size_t pos = work_buff.find(' ', prefix.length());
        if (std::string::npos == pos) {
            statement_name = work_buff.substr(prefix.length());
            statement_params = "";
        }
        else {
            statement_name = work_buff.substr(prefix.length(), pos - prefix.length());
            statement_params = work_buff.substr(pos + 1);
        }
        return true;
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename filter_next_t>
struct filter_preprocessor_condition_t {
    typedef std::vector<std::string> expressions_t;

    filter_next_t& filter_next;
    buffer_ptr_c_t const& vars;
    expressions_t* const expressions_ptr;
    integer_expression_calc calc;
    filter_preprocessor_condition_t(filter_next_t& filter_next, buffer_ptr_c_t const& vars, expressions_t* expressions_ptr = 0): filter_next(filter_next), vars(vars), expressions_ptr(expressions_ptr) {}
    ~filter_preprocessor_condition_t() {
        assert(state.empty() && "Вектор состояний должен быть пуст к концу файла - несбалансировано открытие и закрытие");
    }
    void filter_addline(std::string const& line) {
        std::string statement_name;
        std::string statement_params;
        if (get_preprocessor_statement(line, statement_name, statement_params)) {
            // Начинаем обрабатывать инструкцию
            if (boost::iequals(statement_name, "if", locale_ru)) {
                process_if_cond_set(get_val_statement_if(statement_params));
                return;
            }
            else if (boost::iequals(statement_name, "ifexp", locale_ru)) {
                process_if_cond_set(get_val_statement_ifexp(statement_params));
                return;
            }
            else if (boost::iequals(statement_name, "else", locale_ru)) {
                process_statement_else(statement_params);
                return;
            }
            else if (boost::iequals(statement_name, "endif", locale_ru)) {
                process_statement_endif(statement_params);
                return;
            }
        }
        // У нас обычная строчка - можем ее пускать или не пускать
        if (is_can_pass_through()) {
            filter_next.filter_addline(line);
        }
    }

    struct state_item_t {
        bool active_state;
        //bool is_changed;
        bool is_been_true_state;
        state_item_t(bool active_state): active_state(active_state)/*, is_changed(false)*/, is_been_true_state(active_state) {}
        bool operator< (state_item_t const& right) const {
            return active_state < right.active_state;
        }
    };
    typedef std::vector<state_item_t> state_t;
    state_t state;

    // Возращает истину, если мы можем вывести строчку, которая не является оператором
    bool is_can_pass_through() const {
        if (state.empty()) {
            return true;
        }
        // Возращаем ложь, если хотя бы 1 условие ложное
        return std::min_element(state.begin(), state.end())->active_state;
    }

    bool get_val_statement_if(std::string const& params) {
        if (params.empty()) {
            assert(false && "пустые параметры");
            return false;
        }
        std::string work_params = params;
        if (vars) {
            work_params = vars->process_all_vars(params);
        }
        else {
            work_params = params;
        }

        std::string cond_operand1;
        std::string cond_operand2;
        if (get_if_condition(work_params, "==", cond_operand1, cond_operand2)) {
            return boost::iequals(cond_operand1, cond_operand2, locale_ru);
        }
        else if (get_if_condition(work_params, "!=", cond_operand1, cond_operand2)) {
            return !boost::iequals(cond_operand1, cond_operand2, locale_ru);
        }
        else {
            assert(false && "ошибка выражения условного оператора");
        }
        return false;
    }

    bool get_val_statement_ifexp(std::string const& params) {
        if (params.empty()) {
            assert(false && "пустые параметры");
            return false;
        }

        // Если указан буффер для записи выражений, пишем сырое выражение в него
        if (expressions_ptr) {
            expressions_ptr->push_back(params);
        }

        std::string expression = params;
        if (vars) {
            expression = vars->process_all_vars(params);
        }

        // Считаем выражение
        int rezult;
        if (calc.evaluate(expression, rezult)) {
            return 0 != rezult;
        }
        else {
            assert(false && "Ошибка подсчета выражения");
        }
        return false;
    }

    bool get_val_statement_else(std::string const& params) {
        std::size_t pos = params.find(' ');
        if (std::string::npos == pos) {
            assert(false && "не найден оператор");
            return false;
        }
        std::string else_operator = boost::trim_copy(params.substr(0, pos));
        std::string else_params = boost::trim_copy(params.substr(pos + 1));
        if (else_operator.empty() || else_params.empty()) {
            assert(false && "не найден оператор 2");
            return false;
        }

        if (boost::iequals(else_operator, "if", locale_ru)) {
            return get_val_statement_if(else_params);
        }
        else if (boost::iequals(else_operator, "ifexp", locale_ru)) {
            return get_val_statement_ifexp(else_params);
        }

        assert(false && "неизвесный оператор");
        return false;
    }

    void process_statement_else(std::string const& params) {
        if (state.empty()) {
            assert(false && "Небыло условия");
            return;
        }
        bool val = true;
        if (!params.empty()) { // У нас else с условием
            val = get_val_statement_else(params);
        }

        state_item_t& item = state.back();
        if (item.active_state) { // С истины на лож переключаем на любом условии
            item.active_state = false;            
        }
        else if (val && !item.is_been_true_state) {
            item.active_state = true;
            item.is_been_true_state = true;
        }
    }

    void process_statement_endif(std::string const& params) {
        if (!params.empty()) {
            assert(false && "не пустые параметры");
            return;
        }
        assert(!state.empty() && "Небыло условия");
        if (!state.empty()) {
            state.erase(state.end() - 1);
        }
    }

    static bool get_if_condition(std::string const& condition, std::string const& cond_type, std::string& cond_operand1, std::string& cond_operand2) {
        std::size_t pos = condition.find(cond_type);
        if (std::string::npos == pos) {
            return false;
        }
        cond_operand1 = boost::trim_copy(condition.substr(0, pos));
        cond_operand2 = boost::trim_copy(condition.substr(pos + cond_type.length()));
        return !cond_operand1.empty() && !cond_operand2.empty();
    }
    /*
    void process_if_cond_equality(std::string const& cond_operand1, std::string const& cond_operand2) {
    process_if_cond_set(boost::iequals(cond_operand1, cond_operand2, locale_ru));
    }
    void process_if_cond_inequality(std::string const& cond_operand1, std::string const& cond_operand2) {
    process_if_cond_set(!boost::iequals(cond_operand1, cond_operand2, locale_ru));
    }
    */
    void process_if_cond_set(bool value) {
        state.push_back(state_item_t(value));
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename filter_next_t>
struct filter_preprocessor_include_t {
    filter_next_t& filter_next;
    boost::filesystem::path& root_directory;
    filter_preprocessor_include_t(filter_next_t& filter_next, boost::filesystem::path& root_directory): filter_next(filter_next), root_directory(root_directory) {}
    void filter_addline(std::string const& line) {
        std::string statement_name;
        std::string statement_params;
        if (get_preprocessor_statement(line, statement_name, statement_params)) {
            // Начинаем обрабатывать инструкцию
            if (boost::iequals(statement_name, "tryinclude", locale_ru)) {
                process_statement_tryinclude(statement_params);
                return;
            }
        }
        // У нас обычная строчка - можем ее пускать или не пускать
        filter_next.filter_addline(line);
    }

    void process_statement_tryinclude(std::string const& params) {
        if (params.empty()) {
            assert(false && "пустые параметры");
            return;
        }
        boost::filesystem::path include_filename = root_directory / params;
        if (!boost::filesystem::exists(include_filename)) {
            return;
        }

        boost::filesystem::ifstream file(include_filename, std::ios_base::binary);
        if (file) {
            typedef filter_next_t filter05_t;

            typedef filter_preprocessor_include_t   <filter05_t>    filter04_t;
            typedef filter_multiline_t              <filter04_t>    filter03_t;
            typedef filter_trim_t                   <filter03_t>    filter02_t;
            typedef filter_dropcomments_t           <filter02_t>    filter01_t;
            typedef filter_streamreader_t           <filter01_t>    filter00_t;

            filter04_t filter04(filter_next, root_directory);
            filter03_t filter03(filter04);
            filter02_t filter02(filter03);
            filter01_t filter01(filter02);
            filter00_t filter00(filter01, file);

            filter00.filter_process();
        }
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BUFFER_FILTERS_HPP
