#ifndef GEO_INFO_GETTER_IP_CHECK_HPP
#define GEO_INFO_GETTER_IP_CHECK_HPP
#include "geo_info_getter.hpp"

namespace geo {
    class info_getter_ip_check: public info_getter {
    public:
        info_getter_ip_check();
        virtual ~info_getter_ip_check();
    public: // info_getter
        virtual void process_ip_info(ip_info& info);
        virtual bool get_db_info(std::string& db_info_string) const;

    public:
        static unsigned int get_ip(std::string const& ip_string); // Переводит строку с ип аресом в число. В случае неудачи - 0
    };
} // namespace geo {
#endif // GEO_INFO_GETTER_IP_CHECK_HPP
