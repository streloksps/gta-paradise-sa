#include "config.hpp"
#include "server_configuration.hpp"
#include "core/module_c.hpp"
#include "core/players.hpp"
#include "core/serialization/configuradable.hpp"
#include "core/buffer/buffer.hpp"
#include "core/buffer/buffer_reader.hpp"
#include "core/buffer/buffer_writer.hpp"
#include "core/utility/directory.hpp"
#include "core/logger/logger.hpp"
#include "core/samp/samp_api.hpp"
#include "core/utility/integer_expression_calc.hpp"
#include "core/messages/messages_storage.hpp"
#include "server_paths.hpp"

#include <ctime>
#include <functional>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

REGISTER_IN_APPLICATION(server_configuration);

server_configuration::ptr server_configuration::instance() {
    return application::instance()->get_item<server_configuration>();
}

server_configuration::server_configuration()
:is_first_configurated(false)
{
    server_vers.push_back(std::make_pair(std::tr1::regex("^a1.*$"), "anderius"));
    server_vers.push_back(std::make_pair(std::tr1::regex("^cr.*$"), "cr"));
    server_vers.push_back(std::make_pair(std::tr1::regex("^g6.*$"), "gostown"));
    server_vers.push_back(std::make_pair(std::tr1::regex("^u1.*$"), "united"));
    server_vers.push_back(std::make_pair(std::tr1::regex("^.*$"), "sa"));
}

server_configuration::~server_configuration() {

}

void server_configuration::on_timer60000() {
    if (!is_first_configurated) {
        // Небыло первой полной конфигурации, кеш конфига пуст
        return;
    }
    try_autoreconfig();
}

void server_configuration::reconfigure() {
    buffer_ptr_c_t      config_buff_root = get_preprocessor_params();
    buffer::ptr         config_buff(config_buff_root->children_create_connected());
    buffer::ptr const&  textrc_buff = messages_storage::instance()->get_root_buff();
    textrc_buff->clear();

    load_from_config(config_buff, textrc_buff);
    configure_use_buffer(config_buff, textrc_buff);
    
    is_first_configurated = true;
}

bool server_configuration::reconfigure_from_cache() {
    if (!is_first_configurated) {
        return false;
    }

    buffer_ptr_c_t      config_buff_root = get_preprocessor_params();
    buffer::ptr         config_buff(config_buff_root->children_create_connected());
    buffer::ptr const&  textrc_buff = messages_storage::instance()->get_root_buff();
    textrc_buff->clear();

    if (load_from_cache(config_buff, textrc_buff)) {
        configure_use_buffer(config_buff, textrc_buff);
        return true;
    }
    return false;
}

namespace {
    void load_from_config_impl(
         buffer_ptr_t const& buff
        ,buffer_reader::expressions_t& preprocessed_expressions
        ,buffer_ptr_c_t const& preprocessor_params
        ,boost::filesystem::path const& dir
        ,std::string const& regex
        ,boost::filesystem::path const& cache_filename
   ) {
       // Путь к файлу кеша
       boost::filesystem::create_directories(cache_filename.parent_path());
       boost::filesystem::ofstream preprocessed_file(cache_filename);

       buffer_reader reader(buff, preprocessor_params);

       if (preprocessed_file) {
           // Файл должен оставаться открытым до завершения чтения конфигов
           reader.attach_preprocessed_stream(preprocessed_file);
       }

       reader.attach_expressions_buff(preprocessed_expressions);

       iterate_directory(dir, regex, bind(&buffer_reader::load_file, std::tr1::ref(reader), std::tr1::placeholders::_1));
    }

    bool load_from_cache_impl(
        buffer_ptr_t const& buff
        ,buffer_ptr_c_t const& preprocessor_params
        ,boost::filesystem::path const& cache_filename
    ) {
        boost::filesystem::ifstream cache_file(cache_filename);
        if (cache_file) {
            buffer_reader reader(buff, preprocessor_params);
            reader.load_stream_only_condition(cache_file);
            return true;
        }
        return false;
    }
} // namespace {


void server_configuration::configure_use_buffer_debug_save(buffer_ptr_t const& buff, boost::filesystem::path const& filename) {
    buffer_writer writer(buff);
    boost::filesystem::create_directories(filename.parent_path());
    writer.save_file(filename, true);
}

void server_configuration::load_from_config(buffer_ptr_t const& config_buff, buffer_ptr_t const& textrc_buff) {
    buffer_ptr_c_t preprocessor_params = get_preprocessor_params();

    buffer_reader::expressions_t preprocessed_expressions;
    load_from_config_impl(config_buff, preprocessed_expressions, preprocessor_params, PATH_CONFIG_DIR, PATH_CONFIG_REGEX, PATH_CONFIG_CACHE_FILENAME);
    load_from_config_impl(textrc_buff, preprocessed_expressions, preprocessor_params, PATH_TEXTRC_DIR, PATH_TEXTRC_REGEX, PATH_TEXTRC_CACHE_FILENAME);

    { // Обрабатываем выражения
        configure_expressions.clear();
        BOOST_FOREACH(std::string const& str, preprocessed_expressions) {
            configure_expressions.insert(get_trimed_expression(str));
        }
        calc_vals(configure_expressions, configure_expressions_vals_last);
    }
}

bool server_configuration::load_from_cache(buffer_ptr_t const& config_buff, buffer_ptr_t const& textrc_buff) {
    buffer_ptr_c_t preprocessor_params = get_preprocessor_params();

    return load_from_cache_impl(config_buff, preprocessor_params, PATH_CONFIG_CACHE_FILENAME)
        && load_from_cache_impl(textrc_buff, preprocessor_params, PATH_TEXTRC_CACHE_FILENAME);
}

void server_configuration::configure_use_buffer(buffer_ptr_t const& config_buff, buffer_ptr_t const& textrc_buff) {
    { // Отладка конфигурации и текстовых ресурсов
        configure_use_buffer_debug_save(config_buff, PATH_CONFIG_PROCESSED_FILENAME);
        configure_use_buffer_debug_save(textrc_buff, PATH_TEXTRC_PROCESSED_FILENAME);
    }

    { // Текстовые ресурсы уже установлены (они должны быть до конфигурирования)
    }

    { // Конфигурируем
        std::vector<configuradable::ptr> configuradables = get_root()->get_items<configuradable>();
        for_each(configuradables.begin(), configuradables.end(), bind(&configuradable::configure_pre, std::tr1::placeholders::_1));
        BOOST_FOREACH(configuradable::ptr ptr, configuradables) {
            serialize(config_buff, serialization::make_nvp(ptr), serialization::make_def(&configuradable::configure, serialization::def_reading));
        }
        for_each(configuradables.begin(), configuradables.end(), bind(&configuradable::configure_post, std::tr1::placeholders::_1));
    }
}

std::string server_configuration::get_trimed_expression(std::string const& expr) {
    return boost::erase_all_copy(expr, " ");
}

void server_configuration::calc_vals(expressions_t const& expressions, expressions_vals_t& expressions_vals) const {
    expressions_vals.clear();
    expressions_vals.reserve(expressions.size());
    integer_expression_calc calc;
    buffer_ptr_c_t vars = get_preprocessor_params();
    BOOST_FOREACH(std::string const& expr, expressions) {
        int rezult;
        std::string curr_expr = vars->process_all_vars(expr);
        expressions_vals.push_back(calc.evaluate(curr_expr, rezult) && 0 != rezult);
    }
}

void server_configuration::reconfig_on_changed_vals() {
    messages_item msg_item;
    msg_item.get_sender()
        ("$(server_autoreconfigure_begin_log)", msg_log);
    if (reconfigure_from_cache()) {
        msg_item.get_sender()
            ("$(server_autoreconfigure_done_log)", msg_log)
            ("$(server_autoreconfigure_done_admins)", msg_players_all_admins);
    }
    else {
        msg_item.get_sender()
            ("$(server_autoreconfigure_error_log)", msg_log)
            ("$(server_autoreconfigure_error_admins)", msg_players_all_admins);
    }
}

namespace { // Описание переменных препроцессора
    // Константы
    std::string const preprocessor_game_name        = "game";

    std::string const preprocessor_ver_name         = "ver";

    std::string const preprocessor_debug_name       = "debug";
    std::string const preprocessor_debug_on         = "true";
    std::string const preprocessor_debug_off        = "false";

    std::string const preprocessor_develop_name     = "dev";
    std::string const preprocessor_develop_on       = "true";
    std::string const preprocessor_develop_off      = "false";

    std::string const preprocessor_bind_ip_name     = "bind_ip";

    // Переменные
    std::string const preprocessor_time_name        = "time";
    std::string const preprocessor_players_name     = "players";
    std::string const preprocessor_players_max_name = "players_max";
} // namespace {

buffer_ptr_c_t server_configuration::get_preprocessor_params() const {
    std::string preprocessor_game;
    std::string preprocessor_ver;
    std::string preprocessor_debug;
    std::string preprocessor_develop;
    std::string preprocessor_bind_ip;

    {
        std::string query_version = samp::api::instance()->get_server_var_as_string("version");

        BOOST_FOREACH(server_vers_t::value_type const& pair, server_vers) {
            if (std::tr1::regex_match(query_version, pair.first)) {
                preprocessor_game = pair.second;
                break;
            }
        }
        {
#       ifndef NDEBUG
            // В режиме отладки
            preprocessor_debug = preprocessor_debug_on;
#       else // #ifndef NDEBUG
            preprocessor_debug = preprocessor_debug_off;
#       endif // #ifndef NDEBUG
        }

#       ifdef DEVELOP
            preprocessor_develop = preprocessor_develop_on;
#       else // ifdef DEVELOP
            preprocessor_develop = preprocessor_develop_off;
#       endif // ifdef DEVELOP


        preprocessor_bind_ip = samp::api::instance()->get_server_var_as_string("bind");
    }

    buffer::ptr buff(new buffer);
    buff->value_replace(preprocessor_game_name, preprocessor_game);
    buff->value_replace(preprocessor_ver_name, application::instance()->get_server_version());
    buff->value_replace(preprocessor_debug_name, preprocessor_debug);
    buff->value_replace(preprocessor_develop_name, preprocessor_develop);
    buff->value_replace(preprocessor_time_name, boost::lexical_cast<std::string>(std::time(0)));
    buff->value_replace(preprocessor_players_name, boost::lexical_cast<std::string>(players::instance()->get_players_count()));
    buff->value_replace(preprocessor_players_max_name, boost::lexical_cast<std::string>(players::instance()->get_players_max()));
    buff->value_replace(preprocessor_bind_ip_name, preprocessor_bind_ip);

    return buff;
}

void server_configuration::try_autoreconfig() {
    expressions_vals_t  expressions_vals;
    calc_vals(configure_expressions, expressions_vals);
    if (expressions_vals == configure_expressions_vals_last) {
        // Изменений не произошло - ничего не делаем
        return;
    }
    reconfig_on_changed_vals();
    configure_expressions_vals_last = expressions_vals;
}
