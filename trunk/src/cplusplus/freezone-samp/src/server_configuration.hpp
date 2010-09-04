#ifndef SERVER_CONFIGURATION_HPP
#define SERVER_CONFIGURATION_HPP
#include "core/module_h.hpp"
#include "core/buffer/buffer_fwd.hpp"
#include <set>
#include <string>
#include <regex>
#include <boost/filesystem.hpp>

class server_configuration
    :public application_item
    ,public on_timer60000_i
{
public:
    typedef std::tr1::shared_ptr<server_configuration> ptr;
    static ptr instance();

public:
    server_configuration();
    virtual ~server_configuration();

public:
    virtual void on_timer60000();

public: // Публичные методы
    void reconfigure(); // Обычная конфигурация. Создает кеш
    bool reconfigure_from_cache(); // Конфигурация из кеша. Возращает истину, если все прошло успешно
    buffer_ptr_c_t get_preprocessor_params() const; // Возращает параметры приложения
    static void configure_use_buffer_debug_save(buffer_ptr_t const& buff, boost::filesystem::path const& filename);

private:
    bool is_first_configurated;

private:
    typedef std::vector<std::pair<std::tr1::regex, std::string> > server_vers_t;
    typedef std::set<std::string>   expressions_t;
    typedef std::vector<bool>       expressions_vals_t;

    expressions_t       configure_expressions;
    expressions_vals_t  configure_expressions_vals_last;
    server_vers_t       server_vers;

    void try_autoreconfig();

    static std::string get_trimed_expression(std::string const& expr);
    void calc_vals(expressions_t const& expressions, expressions_vals_t& expressions_vals) const;
    void reconfig_on_changed_vals();
    
    void load_from_config(buffer_ptr_t const& config_buff, buffer_ptr_t const& textrc_buff);
    bool load_from_cache(buffer_ptr_t const& config_buff, buffer_ptr_t const& textrc_buff);
    void configure_use_buffer(buffer_ptr_t const& config_buff, buffer_ptr_t const& textrc_buff);
};
#endif // SERVER_CONFIGURATION_HPP
