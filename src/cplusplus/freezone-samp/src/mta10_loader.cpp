#include "config.hpp"
#include "mta10_loader.hpp"
#include <algorithm>
#include <functional>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include "core/utility/directory.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace {
    template <typename iterator>
    struct object_parser_t: boost::spirit::qi::grammar<iterator, boost::spirit::ascii::space_type> {
        object_parser_t(object_dynamic_t& object): object_parser_t::base_type(start), object(object) {
            namespace qi = boost::spirit::qi;
            namespace ascii = boost::spirit::ascii;

            start = qi::lit("<object ") [boost::phoenix::ref(object.interior) = 0] >> 
                *(  (qi::lit("model=\"")      >> qi::int_     [boost::phoenix::ref(object.model_id)  = qi::_1] >> '"') 
                |   (qi::lit("interior=\"")   >> qi::int_     [boost::phoenix::ref(object.interior)  = qi::_1] >> '"')
                |   (qi::lit("posX=\"")       >> qi::float_   [boost::phoenix::ref(object.x)         = qi::_1] >> '"')
                |   (qi::lit("posY=\"")       >> qi::float_   [boost::phoenix::ref(object.y)         = qi::_1] >> '"')
                |   (qi::lit("posZ=\"")       >> qi::float_   [boost::phoenix::ref(object.z)         = qi::_1] >> '"')
                |   (qi::lit("rotX=\"")       >> qi::float_   [boost::phoenix::ref(object.rx)        = qi::_1] >> '"')
                |   (qi::lit("rotY=\"")       >> qi::float_   [boost::phoenix::ref(object.ry)        = qi::_1] >> '"')
                |   (qi::lit("rotZ=\"")       >> qi::float_   [boost::phoenix::ref(object.rz)        = qi::_1] >> '"')
                |   (qi::lexeme[+(ascii::char_ - '=')] >> qi::lit("=\"") >> qi::lexeme[+(ascii::char_ - '"')] >> '"')
                ) >> "/>";
        }

        boost::spirit::qi::rule<iterator, boost::spirit::ascii::space_type> start;
        object_dynamic_t& object;
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct mta10_parser_objects_t {
        objects_dynamic_t& objects;
        mta10_loader_item_t const* item_ptr;
        mta10_parser_objects_t(objects_dynamic_t& objects): objects(objects), item_ptr(0), object_dynamic_t_parser(object) {}
        inline void parse_begin(mta10_loader_item_t const& item) {item_ptr = &item; object.world = item_ptr->world;}
        inline void parse_next_line(std::string const& line);
        inline void parse_end() {}
        inline bool parse_next_line_impl(std::string const& line, object_dynamic_t& object);
        
        object_dynamic_t object;
        object_parser_t<std::string::const_iterator> object_dynamic_t_parser;
    };

    struct mta10_parser_spawn_points_t {
        spawn_points_t& spawn_points;
        mta10_loader_item_t const* item_ptr;
        mta10_parser_spawn_points_t(spawn_points_t& spawn_points): spawn_points(spawn_points), item_ptr(0) {}
        inline void parse_begin(mta10_loader_item_t const& item) {item_ptr = &item;}
        inline void parse_next_line(std::string const& line);
        inline void parse_end() {}
        inline bool parse_next_line_impl(std::string const& line, pos4& pos);
    };

    struct mta10_parser_vehicles_t {
        vehicles_dynamic_t& vehicles;
        mta10_loader_vehicle_t const* item_ptr;
        mta10_parser_vehicles_t(vehicles_dynamic_t& vehicles): vehicles(vehicles), item_ptr(0) {}
        inline void parse_begin(mta10_loader_vehicle_t const& item) {item_ptr = &item;}
        inline void parse_next_line(std::string const& line);
        inline void parse_end() {}
        inline bool parse_next_line_impl(std::string const& line, pos_vehicle& vehicle);
    };

    template <std::size_t left_n, std::size_t right_n>
    inline bool get_text_between(std::string const& src, std::string::const_iterator src_begin, char const(&left)[left_n], char const(&right)[right_n], std::string::const_iterator& rezult_begin, std::string::const_iterator& rezult_end) {
        std::size_t pos1 = src.find(left);
        if (std::string::npos == pos1) {
            return false;
        }
        std::size_t pos2 = src.find(right, pos1 + left_n - 1);
        if (std::string::npos == pos2) {
            return false;
        }
        rezult_begin = src_begin + pos1 + left_n - 1;
        rezult_end = src_begin + pos2;
        return true;
    }
    template <std::size_t left_n>
    inline bool get_text_between(std::string const& src, std::string::const_iterator src_begin, char const(&left)[left_n], char right, std::string::const_iterator& rezult_begin, std::string::const_iterator& rezult_end) {
        std::size_t pos1 = src.find(left);
        if (std::string::npos == pos1) {
            return false;
        }
        std::size_t pos2 = src.find(right, pos1 + left_n - 1);
        if (std::string::npos == pos2) {
            return false;
        }
        rezult_begin = src_begin + pos1 + left_n - 1;
        rezult_end = src_begin + pos2;
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    inline void mta10_parser_objects_t::parse_next_line(std::string const& line) {
        /*
        // Разбор спиритом
        std::string::const_iterator begin = line.begin(), end = line.end();
        if (boost::spirit::qi::phrase_parse(begin, end, object_dynamic_t_parser, boost::spirit::ascii::space)) {
            assert(end == begin && "Ошибка парсинга");
            objects.insert(object);
        }
        */
        

        
        // Ручной разбор
        if (parse_next_line_impl(line, object)) {
            objects.insert(object);
        }
        
        // В коде оставлены оба варианта разбора, так как еще не решил, какой из них эффективней
    }

    inline bool mta10_parser_objects_t::parse_next_line_impl(std::string const& line, object_dynamic_t& object) {
        if (std::string::npos == line.find("<object ")) {
            return false;
        }
        std::string::const_iterator line_begin = line.begin(), begin, end;

        if (!get_text_between(line, line_begin, "model=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::int_, object.model_id)) {assert(false); return false;}
        assert(end == begin);


        if (get_text_between(line, line_begin, "interior=\"", '"', begin, end)) {
            if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::int_, object.interior)) {assert(false); return false;}
        }
        else {
            object.interior = 0;
        }

        if (!get_text_between(line, line_begin, "posX=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::float_, object.x)) {assert(false); return false;}
        assert(end == begin);

        if (!get_text_between(line, line_begin, "posY=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::float_, object.y)) {assert(false); return false;}
        assert(end == begin);

        if (!get_text_between(line, line_begin, "posZ=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::float_, object.z)) {assert(false); return false;}
        assert(end == begin);

        if (!get_text_between(line, line_begin, "rotX=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::float_, object.rx)) {assert(false); return false;}
        assert(end == begin);

        if (!get_text_between(line, line_begin, "rotY=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::float_, object.ry)) {assert(false); return false;}
        assert(end == begin);

        if (!get_text_between(line, line_begin, "rotZ=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::float_, object.rz)) {assert(false); return false;}
        assert(end == begin);

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    inline void mta10_parser_spawn_points_t::parse_next_line(std::string const& line) {
        pos4 pos(.0f, .0f, .0f, 0, item_ptr->world, .0f);
        if (parse_next_line_impl(line, pos)) {
            spawn_points.push_back(pos);
        }
    }

    inline bool mta10_parser_spawn_points_t::parse_next_line_impl(std::string const& line, pos4& pos) {
        if (std::string::npos == line.find("<spawnpoint ")) {
            return false;
        }

        std::string::const_iterator line_begin = line.begin(), begin, end;

        if (get_text_between(line, line_begin, "interior=\"", '"', begin, end)) {
            if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::int_, pos.interior)) {assert(false); return false;}
        }
        else {
            pos.interior = 0;
        }

        if (!get_text_between(line, line_begin, "posX=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::float_, pos.x)) {assert(false); return false;}
        assert(end == begin);

        if (!get_text_between(line, line_begin, "posY=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::float_, pos.y)) {assert(false); return false;}
        assert(end == begin);

        if (!get_text_between(line, line_begin, "posZ=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::float_, pos.z)) {assert(false); return false;}
        assert(end == begin);

        if (!get_text_between(line, line_begin, "rotZ=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::float_, pos.rz)) {assert(false); return false;}
        assert(end == begin);

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    inline void mta10_parser_vehicles_t::parse_next_line(std::string const& line) {
        pos_vehicle vehicle(0, .0f, .0f, .0f, 0, item_ptr->world, .0f, -1, -1, item_ptr->respawn_delay);
        if (parse_next_line_impl(line, vehicle)) {
            vehicles.insert(vehicle);
        }
    }

    inline bool mta10_parser_vehicles_t::parse_next_line_impl(std::string const& line, pos_vehicle& vehicle) {
        if (std::string::npos == line.find("<vehicle ")) {
            return false;
        }

        std::string::const_iterator line_begin = line.begin(), begin, end;

        if (!get_text_between(line, line_begin, "model=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::int_, vehicle.model_id)) {assert(false); return false;}
        assert(end == begin);


        if (get_text_between(line, line_begin, "interior=\"", '"', begin, end)) {
            if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::int_, vehicle.interior)) {assert(false); return false;}
        }
        else {
            vehicle.interior = 0;
        }

        if (!get_text_between(line, line_begin, "posX=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::float_, vehicle.x)) {assert(false); return false;}
        assert(end == begin);

        if (!get_text_between(line, line_begin, "posY=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::float_, vehicle.y)) {assert(false); return false;}
        assert(end == begin);

        if (!get_text_between(line, line_begin, "posZ=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::float_, vehicle.z)) {assert(false); return false;}
        assert(end == begin);

        if (!get_text_between(line, line_begin, "rotZ=\"", '"', begin, end)) {assert(false); return false;}
        if (!boost::spirit::qi::parse(begin, end, boost::spirit::qi::float_, vehicle.rz)) {assert(false); return false;}
        assert(end == begin);

        return true;
    }

    /* медленный вариант
    template <typename item_t, typename parser_t>
    inline void load_item_impl(item_t const& item, boost::filesystem::path const& file_name, parser_t& parser) {
        boost::filesystem::ifstream file(file_name, std::ios_base::binary);
        if (file) {
            parser.parse_begin(item);
            for (;;) {
                std::string buffer;
                std::getline(file, buffer);
                parser.parse_next_line(buffer);
                if (file.eof()) {
                    break;
                }
            }
            parser.parse_end();
        }
        else {
            assert(false && "не удалось открыть файл");
        }
    }
    */
    template <typename item_t, typename parser_t>
    inline void load_item_impl(item_t const& item, boost::filesystem::path const& file_name, parser_t& parser) {
        boost::filesystem::ifstream file(file_name, std::ios_base::binary);
        if (file) {
            parser.parse_begin(item);
            char file_buff[1024];
            for (;!file.eof();) {
                file.getline(file_buff, sizeof(file_buff) / sizeof(file_buff[0]));
                parser.parse_next_line(std::string(file_buff));
            }
            parser.parse_end();
        }
        else {
            assert(false && "не удалось открыть файл");
        }
    }

    template <typename item_t, typename parser_t>
    inline void load_items_impl(std::vector<item_t> const& items, boost::filesystem::path const& root_path, parser_t& parser) {
        BOOST_FOREACH(item_t const& item, items) {
            iterate_directory(root_path / item.file_name, ".*\\.map", 
                std::tr1::bind(
                static_cast<void (*)(item_t const& item, boost::filesystem::path const& file_name, parser_t& parser)>(&load_item_impl)
                ,std::tr1::cref(item)
                ,std::tr1::placeholders::_1
                ,std::tr1::ref(parser)
                )
                );
        }
    }
}; // namespace {

mta10_loader::mta10_loader(boost::filesystem::path const& root_path): root_path(root_path) {
}

mta10_loader::~mta10_loader() {
}

void mta10_loader::load_items(mta10_loader_items_t const& items, objects_dynamic_t& objects) {
    mta10_parser_objects_t parser(objects);
    load_items_impl(items, root_path, parser);
}

void mta10_loader::load_items(mta10_loader_vehicles_t const& items, vehicles_dynamic_t& vehicles) {
    mta10_parser_vehicles_t parser(vehicles);
    load_items_impl(items, root_path, parser);
}

void mta10_loader::load_items(mta10_loader_items_t const& items, spawn_points_t& spawn_points) {
    mta10_parser_spawn_points_t parser(spawn_points);
    load_items_impl(items, root_path, parser);
}
