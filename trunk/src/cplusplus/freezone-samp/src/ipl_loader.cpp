#include "config.hpp"
#include "ipl_loader.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/noncopyable.hpp>
#include <boost/foreach.hpp>
#include "core/utility/stream_filters.hpp"
#include "core/utility/math.hpp"
#include "core/logger/logger.hpp"

// Описание формата http://www.gtamodding.ru/wiki/IPL

namespace {
    struct ipl_inst_t {
        int object_id;
        int interior;
        float x;
        float y;
        float z;
        float rx;
        float ry;
        float rz;
        float rw;
    };
    template <typename iterator>
    struct ipl_parser_t: boost::spirit::qi::grammar<iterator, boost::spirit::ascii::space_type> {
        ipl_parser_t(ipl_inst_t& ipl_inst): ipl_parser_t::base_type(start), ipl_inst(ipl_inst) {
            namespace qi = boost::spirit::qi;
            namespace ascii = boost::spirit::ascii;

            start
                =  qi::int_     [boost::phoenix::ref(ipl_inst.object_id)  = qi::_1] >> ','
                >> qi::lexeme[+(ascii::char_ - ',')] >> ','
                >> qi::int_     [boost::phoenix::ref(ipl_inst.interior)   = qi::_1] >> ','
                >> qi::float_   [boost::phoenix::ref(ipl_inst.x)          = qi::_1] >> ','
                >> qi::float_   [boost::phoenix::ref(ipl_inst.y)          = qi::_1] >> ','
                >> qi::float_   [boost::phoenix::ref(ipl_inst.z)          = qi::_1] >> ','
                >> qi::float_   [boost::phoenix::ref(ipl_inst.rx)         = qi::_1] >> ','
                >> qi::float_   [boost::phoenix::ref(ipl_inst.ry)         = qi::_1] >> ','
                >> qi::float_   [boost::phoenix::ref(ipl_inst.rz)         = qi::_1] >> ','
                >> qi::float_   [boost::phoenix::ref(ipl_inst.rw)         = qi::_1] >> ','
                ;/* от дабовления этой строчки перестает компилиться
                >> qi::int_[boost::phoenix::ref(object.is_ok) = true];
                */
        }

        boost::spirit::qi::rule<iterator, boost::spirit::ascii::space_type> start;
        ipl_inst_t& ipl_inst;
    };


    template <typename object_t>
    struct line_parser: private boost::noncopyable {
        line_parser(boost::filesystem::path const& file_name, int world, std::set<object_t>& objects, bool is_dump)
            :file_name(file_name)
            ,world(world)
            ,objects(objects)
            ,is_dump(is_dump)
            ,ipl_parser(ipl_inst)
        {  }
        void init() {
            objects_dump.clear();
        }
        void fini() {
            if (is_dump) {
                logger::instance()->debug("ipl_loader", (boost::format("From file '%1%' loaded %2% object(s):") % file_name.file_string() % objects_dump.size()).str());
                BOOST_FOREACH(object_t const& object, objects_dump) {
                    //logger::instance()->debug("ipl_loader/obj", (boost::format("id=%1% x=%2% y=%3% z=%4% int=%5% world=%6% rx=%7% ry=%8% rz=%9%") 
                    logger::instance()->debug("ipl_loader/obj", (boost::format("%1%") % object).str());
                }
            }
            objects_dump.clear();
        }
        void filter_addline(std::string const& line) {
            // Разбор спиритом
            std::string::const_iterator begin = line.begin(), end = line.end();
            if (boost::spirit::qi::phrase_parse(begin, end, ipl_parser, boost::spirit::ascii::space)) {
                // У нас до донца строчка не парситься
                //assert(end == begin && "Ошибка парсинга");

                object_t work_object;
                get_object_from_ipl(work_object, ipl_inst, world);
                objects.insert(work_object);
                if (is_dump) {
                    objects_dump.push_back(work_object);
                }
            }
        }

        static void get_object_from_ipl(object_dynamic_t& object_out, ipl_inst_t ipl_inst, int world) { // Конвертируем из формата ipl
            quaternion q(ipl_inst.rw, ipl_inst.rx, ipl_inst.ry, ipl_inst.rz);
            vector3f euler = q.euler_angles();
            object_out = object_dynamic_t(ipl_inst.object_id
                ,ipl_inst.x, ipl_inst.y, ipl_inst.z
                ,ipl_inst.interior, world
                ,euler.z * rad2deg, -euler.y * rad2deg, -euler.x * rad2deg
                );
        }
        static void get_object_from_ipl(object_static_t& object_out, ipl_inst_t ipl_inst, int world) {
            object_dynamic_t object_dynamic;
            get_object_from_ipl(object_dynamic, ipl_inst, world);
            object_out = object_static_t(object_dynamic.model_id, object_dynamic.x, object_dynamic.y, object_dynamic.z, object_dynamic.rx, object_dynamic.ry, object_dynamic.rz);
        }

    private: // Параметры инициализации
        boost::filesystem::path file_name;
        int world;
        std::set<object_t>& objects;
        bool is_dump;

    private: // Объекты для работы
        typedef std::vector<object_t> objects_dump_t;
        objects_dump_t objects_dump;

        ipl_inst_t ipl_inst;
        ipl_parser_t<std::string::const_iterator> ipl_parser;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename filter_next_t>
    struct filter_ipl_section_selector_t {
        filter_next_t& filter_next;
        std::string section_name;
        bool is_in_section;
        filter_ipl_section_selector_t(filter_next_t& filter_next, std::string const& section_name): filter_next(filter_next), section_name(section_name), is_in_section(false) {}
        void filter_addline(std::string const& line) {
            if (is_in_section) {
                if (boost::iequals(line, "end")) {
                    // Нашли конец секции
                    is_in_section = false;
                    return;
                }
                if (!line.empty()) {
                    filter_next.filter_addline(line);
                }
            }
            else {
                if (boost::iequals(line, section_name)) {
                    is_in_section = true;
                }
            }
        }
    };
} // namespace {

ipl_loader::ipl_loader(boost::filesystem::path const& root_path, bool is_dump): root_path(root_path), is_dump(is_dump) {
}

ipl_loader::~ipl_loader() {
}

void ipl_loader::load_items(ipl_loader_items_t const& items, std::set<object_dynamic_t>& objects) {
    load_items_impl(items, objects);
}

void ipl_loader::load_items(ipl_loader_items_t const& items, std::set<object_static_t>& objects) {
    load_items_impl(items, objects);
}
template <typename object_t>
inline void ipl_loader::load_items_impl(ipl_loader_items_t const& items, std::set<object_t>& objects) {
    BOOST_FOREACH(ipl_loader_item_t const& item, items) {
        load_item(root_path / item.file_name, item.world, objects);
    }
}

template <typename object_t>
void ipl_loader::load_item(boost::filesystem::path const& file_name, int world, std::set<object_t>& objects) {
    boost::filesystem::ifstream file(file_name, std::ios_base::binary);
    if (file) {
        typedef line_parser<object_t> filter04_t;

        typedef filter_ipl_section_selector_t   <filter04_t>    filter03_t;
        typedef filter_trim_t                   <filter03_t>    filter02_t;
        typedef filter_dropcomments_t           <filter02_t>    filter01_t;
        typedef filter_streamreader_t           <filter01_t>    filter00_t;

        filter04_t filter04(file_name, world, objects, is_dump);
        filter03_t filter03(filter04, "inst");
        filter02_t filter02(filter03);
        filter01_t filter01(filter02);
        filter00_t filter00(filter01, file);

        filter04.init();
        filter00.filter_process();
        filter04.fini();
    }
}
