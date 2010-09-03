#ifndef LOCALE_RU_HPP
#define LOCALE_RU_HPP
#include <string>
#include <functional>
#include <locale>

extern std::locale locale_ru;

struct ilexicographical_less: public std::binary_function<std::string, std::string, bool> {
    bool operator()(std::string const& left, std::string const& right) const;
};

// Конвертирует из кодировки DOS(консоль) в Windows
inline char cp866_to_cp1251(char c) {
    unsigned char rezult = static_cast<unsigned char>(c);

    if (128 <= rezult && 175 >= rezult) {
        rezult += 64;
    }
    else if (224 <= rezult && 239 >= rezult) {
        rezult += 16;
    }
    else if (168 == rezult) {
        rezult = 240;
    }
    else if (184 == rezult) {
        rezult = 241;
    }

    return static_cast<char>(rezult);
}

// Конвертирует из кодировки Windows в DOS(консоль)
inline char cp1251_to_cp866(char c) {
    unsigned char rezult = static_cast<unsigned char>(c);

    if (192 <= rezult && 239 >= rezult) {
        rezult -= 64;
    }
    else if (240 <= rezult) {
        rezult -= 16;
    }
    else if (240 == rezult) {
        rezult = 168;
    }
    else if (241 == rezult) {
        rezult = 184;
    }

    return static_cast<char>(rezult);
}

std::string cp1251_to_transliteration(std::string const& str);

#endif // LOCALE_RU_HPP
