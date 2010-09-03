#ifndef GEO_INFO_GETTER_HPP
#define GEO_INFO_GETTER_HPP
#include <memory>
#include <string>

namespace geo {
    class ip_info;

    class info_getter {
    public:
        typedef std::tr1::shared_ptr<info_getter> ptr;

    public:
        virtual ~info_getter() {}
        virtual void process_ip_info(ip_info& info) = 0;
        virtual bool get_db_info(std::string& db_info_string) const = 0;
    };
} // namespace geo {
#endif // GEO_INFO_GETTER_HPP
