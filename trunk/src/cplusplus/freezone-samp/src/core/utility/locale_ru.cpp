#include "config.hpp"
#include "locale_ru.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

namespace {
    //! ����� ������, ��� ������ � �������� ������� � ��������� cp1251
    class ctype_char_cp1251: public std::ctype<char> {
    protected:
        virtual ~ctype_char_cp1251() {}

        virtual char do_tolower(char ch) const {
            unsigned char rezult = std::ctype<char>::do_tolower(ch);
            if (0xC0 <= rezult && 0xDF >= rezult) {
                rezult += 0x20;
            }
            else if (0xA8 == rezult) {
                rezult = 0xB8;
            }
            return rezult;
        }
        virtual char do_toupper(char ch) const {
            unsigned char rezult = std::ctype<char>::do_toupper(ch);
            if (0xE0 <= rezult/* && 0xFF >= rezult*/) {
                rezult -= 0x20;
            }
            else if (0xB8 == rezult) {
                rezult = 0xA8;
            }
            return rezult;
        }
    };
} // namespace {

std::locale locale_ru(std::locale(), new ctype_char_cp1251());

bool ilexicographical_less::operator()(std::string const& left, std::string const& right) const {
    return boost::ilexicographical_compare(left, right, locale_ru);
}

std::string cp1251_to_transliteration(std::string const& str) {
    std::string rezult;
    BOOST_FOREACH(char c, str) {
        switch (c) {
            case '�': rezult += 'A'; break;
            case '�': rezult += 'a'; break;
            case '�': rezult += 'B'; break;
            case '�': rezult += 'b'; break;
            case '�': rezult += 'V'; break;
            case '�': rezult += 'v'; break;
            case '�': rezult += 'G'; break;
            case '�': rezult += 'g'; break;
            case '�': rezult += 'D'; break;
            case '�': rezult += 'd'; break;
            case '�': rezult += 'E'; break;
            case '�': rezult += 'e'; break;
            case '�': rezult += 'E'; break;
            case '�': rezult += 'e'; break;
            case '�': rezult += "ZH"; break;
            case '�': rezult += "zh"; break;
            case '�': rezult += 'Z'; break;
            case '�': rezult += 'z'; break;
            case '�': rezult += 'I'; break;
            case '�': rezult += 'i'; break;
            case '�': rezult += 'J'; break;
            case '�': rezult += 'j'; break;
            case '�': rezult += 'K'; break;
            case '�': rezult += 'k'; break;
            case '�': rezult += 'L'; break;
            case '�': rezult += 'l'; break;
            case '�': rezult += 'M'; break;
            case '�': rezult += 'm'; break;
            case '�': rezult += 'N'; break;
            case '�': rezult += 'n'; break;
            case '�': rezult += 'O'; break;
            case '�': rezult += 'o'; break;
            case '�': rezult += 'P'; break;
            case '�': rezult += 'p'; break;
            case '�': rezult += 'R'; break;
            case '�': rezult += 'r'; break;
            case '�': rezult += 'S'; break;
            case '�': rezult += 's'; break;
            case '�': rezult += 'T'; break;
            case '�': rezult += 't'; break;
            case '�': rezult += 'U'; break;
            case '�': rezult += 'u'; break;
            case '�': rezult += 'F'; break;
            case '�': rezult += 'f'; break;
            case '�': rezult += 'H'; break;
            case '�': rezult += 'h'; break;
            case '�': rezult += "TS"; break;
            case '�': rezult += "ts"; break;
            case '�': rezult += "CH"; break;
            case '�': rezult += "ch"; break;
            case '�': rezult += "SH"; break;
            case '�': rezult += "sh"; break;
            case '�': rezult += "SHCH"; break;
            case '�': rezult += "shch"; break;
            case '�': rezult += '"'; break;
            case '�': rezult += '"'; break;
            case '�': rezult += 'Y'; break;
            case '�': rezult += 'y'; break;
            case '�': rezult += '\''; break;
            case '�': rezult += '\''; break;
            case '�': rezult += "EI"; break;
            case '�': rezult += "ei"; break;
            case '�': rezult += "YU"; break;
            case '�': rezult += "yu"; break;
            case '�': rezult += "YA"; break;
            case '�': rezult += "ya"; break;
            default: rezult += c;
        }
    }
    return rezult;
}
