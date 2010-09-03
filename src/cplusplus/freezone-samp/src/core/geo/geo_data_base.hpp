#ifndef GEO_DATA_BASE_HPP
#define GEO_DATA_BASE_HPP
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include "core/container/application_item.hpp"
#include "core/createble_i.hpp"
#include "core/serialization/configuradable.hpp"
#include "geo_ip_info.hpp"
#include "detail/geo_info_getter.hpp"

namespace geo {
    class data_base
        :public application_item
        ,public createble_i
        ,public configuradable
    {
    public:
        typedef std::tr1::shared_ptr<data_base> ptr;
        static ptr instance();

    public: // createble_i
        virtual void create();

    public: // configuradable
        virtual void configure_pre();
        virtual void configure(buffer::ptr const& buff, def_t const& def);
        virtual void configure_post();
    public: // auto_name
        SERIALIZE_MAKE_CLASS_NAME(data_base);


    public:
        data_base();
        virtual ~data_base();

        ip_info get_info(std::string const& ip_string);

        void load();
        void unload();

        std::string get_dbs_info() const;
    private:
        std::vector<info_getter::ptr> info_getters;
        bool is_loaded;
        void try_load_db(boost::filesystem::path const& file);
    };
} // namespace geo {
#endif // GEO_DATA_BASE_HPP
