#ifndef GEO_IP_INFO_HPP
#define GEO_IP_INFO_HPP
#include <string>
#include <boost/optional.hpp>

namespace geo {
    class ip_info {
    public:
        std::string                     ip_string;
        bool                            is_ip_string_valid_ip;
        boost::optional<std::string>    country_code2;
        boost::optional<std::string>    country_code3;
        boost::optional<std::string>    country_name_en;
        boost::optional<std::string>    country_name_ru;
        boost::optional<std::string>    city_en;
        boost::optional<int>            as_num;
        boost::optional<std::string>    as_description;
        boost::optional<std::string>    organization;
        boost::optional<float>          latitude;
        boost::optional<float>          longitude;
    public:
        ip_info();
        ~ip_info();

        int get_as_num() const;
        std::string get_as_num_str() const;
        std::string get_long_desc() const;
    };
    
    // Переводит ип адрес в строку - всегда успешна
    std::string get_ip_string(unsigned int ip);

    // Переводит строку с ип аресом в число. В случае неудачи - 0
    unsigned int get_ip(std::string const& ip_string);

} // namespace geo {
#endif // GEO_IP_INFO_HPP
