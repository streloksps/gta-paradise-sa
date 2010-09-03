#ifndef IS_STREAMEBLE_HPP
#define IS_STREAMEBLE_HPP
#include <string>
#include <type_traits>

namespace serialization {
    // ������, ���� ����� ������ �� ����� ���
    template <typename T> struct is_streameble_read: std::tr1::false_type {};
    // ������, ���� ����� ������ � ����� ���
    template <typename T> struct is_streameble_write: std::tr1::false_type {};

    template <> struct is_streameble_read<char>:         std::tr1::true_type {};
    template <> struct is_streameble_read<int>:          std::tr1::true_type {};
    template <> struct is_streameble_read<unsigned int>: std::tr1::true_type {};
    template <> struct is_streameble_read<float>:        std::tr1::true_type {};
    template <> struct is_streameble_read<bool>:         std::tr1::true_type {};
    template <> struct is_streameble_read<std::string>:  std::tr1::true_type {};

    template <> struct is_streameble_write<char>:         std::tr1::true_type {};
    template <> struct is_streameble_write<int>:          std::tr1::true_type {};
    template <> struct is_streameble_write<unsigned int>: std::tr1::true_type {};
    template <> struct is_streameble_write<float>:        std::tr1::true_type {};
    template <> struct is_streameble_write<bool>:         std::tr1::true_type {};
    template <> struct is_streameble_write<std::string>:  std::tr1::true_type {};

    /*
    // ��������� ����� ���� ������������� � 1 ���� ����� ������ �����/������
    template <typename T> struct is_streameble: std::tr1::false_type {};

    template <> struct is_streameble<char>:         std::tr1::true_type {};
    template <> struct is_streameble<int>:          std::tr1::true_type {};
    template <> struct is_streameble<unsigned int>: std::tr1::true_type {};
    template <> struct is_streameble<float>:        std::tr1::true_type {};
    template <> struct is_streameble<bool>:         std::tr1::true_type {};
    template <> struct is_streameble<std::string>:  std::tr1::true_type {};
    */
} // namespace serialization {

#endif // IS_STREAMEBLE_HPP
