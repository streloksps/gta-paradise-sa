#include "config.hpp"
#include "locale_ru.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

namespace {
    //! Класс фасета, для работы с русскими буквами в кодеровке cp1251
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
            case 'А': rezult += 'A'; break;
            case 'а': rezult += 'a'; break;
            case 'Б': rezult += 'B'; break;
            case 'б': rezult += 'b'; break;
            case 'В': rezult += 'V'; break;
            case 'в': rezult += 'v'; break;
            case 'Г': rezult += 'G'; break;
            case 'г': rezult += 'g'; break;
            case 'Д': rezult += 'D'; break;
            case 'д': rezult += 'd'; break;
            case 'Е': rezult += 'E'; break;
            case 'е': rezult += 'e'; break;
            case 'Ё': rezult += 'E'; break;
            case 'ё': rezult += 'e'; break;
            case 'Ж': rezult += "ZH"; break;
            case 'ж': rezult += "zh"; break;
            case 'З': rezult += 'Z'; break;
            case 'з': rezult += 'z'; break;
            case 'И': rezult += 'I'; break;
            case 'и': rezult += 'i'; break;
            case 'Й': rezult += 'J'; break;
            case 'й': rezult += 'j'; break;
            case 'К': rezult += 'K'; break;
            case 'к': rezult += 'k'; break;
            case 'Л': rezult += 'L'; break;
            case 'л': rezult += 'l'; break;
            case 'М': rezult += 'M'; break;
            case 'м': rezult += 'm'; break;
            case 'Н': rezult += 'N'; break;
            case 'н': rezult += 'n'; break;
            case 'О': rezult += 'O'; break;
            case 'о': rezult += 'o'; break;
            case 'П': rezult += 'P'; break;
            case 'п': rezult += 'p'; break;
            case 'Р': rezult += 'R'; break;
            case 'р': rezult += 'r'; break;
            case 'С': rezult += 'S'; break;
            case 'с': rezult += 's'; break;
            case 'Т': rezult += 'T'; break;
            case 'т': rezult += 't'; break;
            case 'У': rezult += 'U'; break;
            case 'у': rezult += 'u'; break;
            case 'Ф': rezult += 'F'; break;
            case 'ф': rezult += 'f'; break;
            case 'Х': rezult += 'H'; break;
            case 'х': rezult += 'h'; break;
            case 'Ц': rezult += "TS"; break;
            case 'ц': rezult += "ts"; break;
            case 'Ч': rezult += "CH"; break;
            case 'ч': rezult += "ch"; break;
            case 'Ш': rezult += "SH"; break;
            case 'ш': rezult += "sh"; break;
            case 'Щ': rezult += "SHCH"; break;
            case 'щ': rezult += "shch"; break;
            case 'Ъ': rezult += '"'; break;
            case 'ъ': rezult += '"'; break;
            case 'Ы': rezult += 'Y'; break;
            case 'ы': rezult += 'y'; break;
            case 'Ь': rezult += '\''; break;
            case 'ь': rezult += '\''; break;
            case 'Э': rezult += "EI"; break;
            case 'э': rezult += "ei"; break;
            case 'Ю': rezult += "YU"; break;
            case 'ю': rezult += "yu"; break;
            case 'Я': rezult += "YA"; break;
            case 'я': rezult += "ya"; break;
            default: rezult += c;
        }
    }
    return rezult;
}
