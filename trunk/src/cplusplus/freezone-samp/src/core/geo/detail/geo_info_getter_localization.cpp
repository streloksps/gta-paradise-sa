#include "config.hpp"
#include "geo_info_getter_localization.hpp"
#include "../geo_ip_info.hpp"

namespace geo {
    info_getter_localization::info_getter_localization() {
        countries_ru.insert(std::make_pair("AU", "���������"));
        countries_ru.insert(std::make_pair("AT", "�������"));
        countries_ru.insert(std::make_pair("AZ", "�����������"));
        countries_ru.insert(std::make_pair("AX", "��������� �������"));
        countries_ru.insert(std::make_pair("AL", "�������"));
        countries_ru.insert(std::make_pair("DZ", "�����"));
        countries_ru.insert(std::make_pair("UM", "������� ����� ������� (���)"));
        countries_ru.insert(std::make_pair("VI", "������������ ���������� �������"));
        countries_ru.insert(std::make_pair("AS", "������������ �����"));
        countries_ru.insert(std::make_pair("AI", "�������"));
        countries_ru.insert(std::make_pair("AO", "������"));
        countries_ru.insert(std::make_pair("AD", "�������"));
        countries_ru.insert(std::make_pair("AQ", "����������"));
        countries_ru.insert(std::make_pair("AG", "������� � �������"));
        countries_ru.insert(std::make_pair("AR", "���������"));
        countries_ru.insert(std::make_pair("AM", "�������"));
        countries_ru.insert(std::make_pair("AW", "�����"));
        countries_ru.insert(std::make_pair("AF", "����������"));
        countries_ru.insert(std::make_pair("BS", "��������� �������"));
        countries_ru.insert(std::make_pair("BD", "���������"));
        countries_ru.insert(std::make_pair("BB", "��������"));
        countries_ru.insert(std::make_pair("BH", "�������"));
        countries_ru.insert(std::make_pair("BZ", "�����"));
        countries_ru.insert(std::make_pair("BY", "����������"));
        countries_ru.insert(std::make_pair("BE", "�������"));
        countries_ru.insert(std::make_pair("BJ", "�����"));
        countries_ru.insert(std::make_pair("BM", "�������"));
        countries_ru.insert(std::make_pair("BG", "��������"));
        countries_ru.insert(std::make_pair("BO", "�������"));
        countries_ru.insert(std::make_pair("BA", "������ � �����������"));
        countries_ru.insert(std::make_pair("BW", "��������"));
        countries_ru.insert(std::make_pair("BR", "��������"));
        countries_ru.insert(std::make_pair("IO", "���������� ���������� � ��������� ������"));
        countries_ru.insert(std::make_pair("VG", "���������� ���������� �������"));
        countries_ru.insert(std::make_pair("BN", "������"));
        countries_ru.insert(std::make_pair("BF", "������� ����"));
        countries_ru.insert(std::make_pair("BI", "�������"));
        countries_ru.insert(std::make_pair("BT", "�����"));
        countries_ru.insert(std::make_pair("VU", "�������"));
        countries_ru.insert(std::make_pair("VA", "�������"));
        countries_ru.insert(std::make_pair("GB", "��������������"));
        countries_ru.insert(std::make_pair("HU", "�������"));
        countries_ru.insert(std::make_pair("VE", "���������"));
        countries_ru.insert(std::make_pair("TL", "��������� �����"));
        countries_ru.insert(std::make_pair("VN", "�������"));
        countries_ru.insert(std::make_pair("GA", "�����"));
        countries_ru.insert(std::make_pair("HT", "�����"));
        countries_ru.insert(std::make_pair("GY", "������"));
        countries_ru.insert(std::make_pair("GM", "������"));
        countries_ru.insert(std::make_pair("GH", "����"));
        countries_ru.insert(std::make_pair("GP", "���������"));
        countries_ru.insert(std::make_pair("GT", "���������"));
        countries_ru.insert(std::make_pair("GN", "������"));
        countries_ru.insert(std::make_pair("GW", "������-�����"));
        countries_ru.insert(std::make_pair("DE", "��������"));
        countries_ru.insert(std::make_pair("GI", "���������"));
        countries_ru.insert(std::make_pair("HN", "��������"));
        countries_ru.insert(std::make_pair("HK", "�������"));
        countries_ru.insert(std::make_pair("GD", "�������"));
        countries_ru.insert(std::make_pair("GL", "����������"));
        countries_ru.insert(std::make_pair("GR", "������"));
        countries_ru.insert(std::make_pair("GE", "������"));
        countries_ru.insert(std::make_pair("GU", "����"));
        countries_ru.insert(std::make_pair("DK", "�����"));
        countries_ru.insert(std::make_pair("CD", "�� �����"));
        countries_ru.insert(std::make_pair("DJ", "�������"));
        countries_ru.insert(std::make_pair("DM", "��������"));
        countries_ru.insert(std::make_pair("DO", "������������� ����������"));
        countries_ru.insert(std::make_pair("EU", "����������� ����"));
        countries_ru.insert(std::make_pair("EG", "������"));
        countries_ru.insert(std::make_pair("ZM", "������"));
        countries_ru.insert(std::make_pair("EH", "�������� ������"));
        countries_ru.insert(std::make_pair("ZW", "��������"));
        countries_ru.insert(std::make_pair("IL", "�������"));
        countries_ru.insert(std::make_pair("IN", "�����"));
        countries_ru.insert(std::make_pair("ID", "���������"));
        countries_ru.insert(std::make_pair("JO", "��������"));
        countries_ru.insert(std::make_pair("IQ", "����"));
        countries_ru.insert(std::make_pair("IR", "����"));
        countries_ru.insert(std::make_pair("IE", "��������"));
        countries_ru.insert(std::make_pair("IS", "��������"));
        countries_ru.insert(std::make_pair("ES", "�������"));
        countries_ru.insert(std::make_pair("IT", "������"));
        countries_ru.insert(std::make_pair("YE", "�����"));
        countries_ru.insert(std::make_pair("KP", "����"));
        countries_ru.insert(std::make_pair("CV", "����-�����"));
        countries_ru.insert(std::make_pair("KZ", "���������"));
        countries_ru.insert(std::make_pair("KY", "��������� �������"));
        countries_ru.insert(std::make_pair("KH", "��������"));
        countries_ru.insert(std::make_pair("CM", "�������"));
        countries_ru.insert(std::make_pair("CA", "������"));
        countries_ru.insert(std::make_pair("QA", "�����"));
        countries_ru.insert(std::make_pair("KE", "�����"));
        countries_ru.insert(std::make_pair("CY", "����"));
        countries_ru.insert(std::make_pair("KG", "��������"));
        countries_ru.insert(std::make_pair("KI", "��������"));
        countries_ru.insert(std::make_pair("CN", "�����"));
        countries_ru.insert(std::make_pair("CC", "��������� �������"));
        countries_ru.insert(std::make_pair("CO", "��������"));
        countries_ru.insert(std::make_pair("KM", "��������� �������"));
        countries_ru.insert(std::make_pair("CR", "�����-����"));
        countries_ru.insert(std::make_pair("CI", "���-������"));
        countries_ru.insert(std::make_pair("CU", "����"));
        countries_ru.insert(std::make_pair("KW", "������"));
        countries_ru.insert(std::make_pair("LA", "����"));
        countries_ru.insert(std::make_pair("LV", "������"));
        countries_ru.insert(std::make_pair("LS", "������"));
        countries_ru.insert(std::make_pair("LR", "�������"));
        countries_ru.insert(std::make_pair("LB", "�����"));
        countries_ru.insert(std::make_pair("LY", "�����"));
        countries_ru.insert(std::make_pair("LT", "�����"));
        countries_ru.insert(std::make_pair("LI", "�����������"));
        countries_ru.insert(std::make_pair("LU", "����������"));
        countries_ru.insert(std::make_pair("MU", "��������"));
        countries_ru.insert(std::make_pair("MR", "����������"));
        countries_ru.insert(std::make_pair("MG", "����������"));
        countries_ru.insert(std::make_pair("YT", "�������"));
        countries_ru.insert(std::make_pair("MO", "�����"));
        countries_ru.insert(std::make_pair("MK", "���������"));
        countries_ru.insert(std::make_pair("MW", "������"));
        countries_ru.insert(std::make_pair("MY", "��������"));
        countries_ru.insert(std::make_pair("ML", "����"));
        countries_ru.insert(std::make_pair("MV", "��������"));
        countries_ru.insert(std::make_pair("MT", "������"));
        countries_ru.insert(std::make_pair("MA", "�������"));
        countries_ru.insert(std::make_pair("MQ", "���������"));
        countries_ru.insert(std::make_pair("MH", "���������� �������"));
        countries_ru.insert(std::make_pair("MX", "�������"));
        countries_ru.insert(std::make_pair("MZ", "��������"));
        countries_ru.insert(std::make_pair("MD", "��������"));
        countries_ru.insert(std::make_pair("MC", "������"));
        countries_ru.insert(std::make_pair("MN", "��������"));
        countries_ru.insert(std::make_pair("MS", "����������"));
        countries_ru.insert(std::make_pair("MM", "������"));
        countries_ru.insert(std::make_pair("NA", "�������"));
        countries_ru.insert(std::make_pair("NR", "�����"));
        countries_ru.insert(std::make_pair("NP", "�����"));
        countries_ru.insert(std::make_pair("NE", "�����"));
        countries_ru.insert(std::make_pair("NG", "�������"));
        countries_ru.insert(std::make_pair("AN", "������������� ���������� �������"));
        countries_ru.insert(std::make_pair("NL", "����������"));
        countries_ru.insert(std::make_pair("NI", "���������"));
        countries_ru.insert(std::make_pair("NU", "����"));
        countries_ru.insert(std::make_pair("NC", "����� ���������"));
        countries_ru.insert(std::make_pair("NZ", "����� ��������"));
        countries_ru.insert(std::make_pair("NO", "��������"));
        countries_ru.insert(std::make_pair("AE", "���"));
        countries_ru.insert(std::make_pair("OM", "����"));
        countries_ru.insert(std::make_pair("CX", "������ ���������"));
        countries_ru.insert(std::make_pair("CK", "������� ����"));
        countries_ru.insert(std::make_pair("HM", "���� � ����������"));
        countries_ru.insert(std::make_pair("PK", "��������"));
        countries_ru.insert(std::make_pair("PW", "�����"));
        countries_ru.insert(std::make_pair("PS", "���������"));
        countries_ru.insert(std::make_pair("PA", "������"));
        countries_ru.insert(std::make_pair("PG", "����� � ����� ������"));
        countries_ru.insert(std::make_pair("PY", "��������"));
        countries_ru.insert(std::make_pair("PE", "����"));
        countries_ru.insert(std::make_pair("PN", "�������"));
        countries_ru.insert(std::make_pair("PL", "������"));
        countries_ru.insert(std::make_pair("PT", "����������"));
        countries_ru.insert(std::make_pair("PR", "������-����"));
        countries_ru.insert(std::make_pair("CG", "���������� �����"));
        countries_ru.insert(std::make_pair("RE", "�������"));
        countries_ru.insert(std::make_pair("RU", "������"));
        countries_ru.insert(std::make_pair("RW", "������"));
        countries_ru.insert(std::make_pair("RO", "�������"));
        countries_ru.insert(std::make_pair("US", "���"));
        countries_ru.insert(std::make_pair("SV", "���������"));
        countries_ru.insert(std::make_pair("WS", "�����"));
        countries_ru.insert(std::make_pair("SM", "���-������"));
        countries_ru.insert(std::make_pair("ST", "���-���� � ��������"));
        countries_ru.insert(std::make_pair("SA", "���������� ������"));
        countries_ru.insert(std::make_pair("SZ", "���������"));
        countries_ru.insert(std::make_pair("SJ", "���������� � ��-�����"));
        countries_ru.insert(std::make_pair("MP", "�������� ���������� �������"));
        countries_ru.insert(std::make_pair("SC", "����������� �������"));
        countries_ru.insert(std::make_pair("SN", "�������"));
        countries_ru.insert(std::make_pair("VC", "����-������� � ���������"));
        countries_ru.insert(std::make_pair("KN", "����-���� � �����"));
        countries_ru.insert(std::make_pair("LC", "����-�����"));
        countries_ru.insert(std::make_pair("PM", "���-���� � �������"));
        countries_ru.insert(std::make_pair("RS", "������"));
        countries_ru.insert(std::make_pair("CS", "������ � ����������"));
        countries_ru.insert(std::make_pair("SG", "��������"));
        countries_ru.insert(std::make_pair("SY", "�����"));
        countries_ru.insert(std::make_pair("SK", "��������"));
        countries_ru.insert(std::make_pair("SI", "��������"));
        countries_ru.insert(std::make_pair("SB", "���������� �������"));
        countries_ru.insert(std::make_pair("SO", "������"));
        countries_ru.insert(std::make_pair("SD", "�����"));
        countries_ru.insert(std::make_pair("SR", "�������"));
        countries_ru.insert(std::make_pair("SL", "������-�����"));
        countries_ru.insert(std::make_pair("SU", "����"));
        countries_ru.insert(std::make_pair("TJ", "�����������"));
        countries_ru.insert(std::make_pair("TH", "�������"));
        countries_ru.insert(std::make_pair("TW", "�������"));
        countries_ru.insert(std::make_pair("TZ", "��������"));
        countries_ru.insert(std::make_pair("TG", "����"));
        countries_ru.insert(std::make_pair("TK", "�������"));
        countries_ru.insert(std::make_pair("TO", "�����"));
        countries_ru.insert(std::make_pair("TT", "�������� � ������"));
        countries_ru.insert(std::make_pair("TV", "������"));
        countries_ru.insert(std::make_pair("TN", "�����"));
        countries_ru.insert(std::make_pair("TM", "���������"));
        countries_ru.insert(std::make_pair("TR", "������"));
        countries_ru.insert(std::make_pair("UG", "������"));
        countries_ru.insert(std::make_pair("UZ", "����������"));
        countries_ru.insert(std::make_pair("UA", "�������"));
        countries_ru.insert(std::make_pair("UY", "�������"));
        countries_ru.insert(std::make_pair("FO", "��������� �������"));
        countries_ru.insert(std::make_pair("FM", "������������ ����� ����������"));
        countries_ru.insert(std::make_pair("FJ", "�����"));
        countries_ru.insert(std::make_pair("PH", "���������"));
        countries_ru.insert(std::make_pair("FI", "���������"));
        countries_ru.insert(std::make_pair("FK", "������������ �������"));
        countries_ru.insert(std::make_pair("FR", "�������"));
        countries_ru.insert(std::make_pair("GF", "����������� ������"));
        countries_ru.insert(std::make_pair("PF", "����������� ���������"));
        countries_ru.insert(std::make_pair("TF", "����������� ����� � �������������� ����������"));
        countries_ru.insert(std::make_pair("HR", "��������"));
        countries_ru.insert(std::make_pair("CF", "���"));
        countries_ru.insert(std::make_pair("TD", "���"));
        countries_ru.insert(std::make_pair("ME", "����������"));
        countries_ru.insert(std::make_pair("CZ", "�����"));
        countries_ru.insert(std::make_pair("CL", "����"));
        countries_ru.insert(std::make_pair("CH", "���������"));
        countries_ru.insert(std::make_pair("SE", "������"));
        countries_ru.insert(std::make_pair("LK", "���-�����"));
        countries_ru.insert(std::make_pair("EC", "�������"));
        countries_ru.insert(std::make_pair("GQ", "�������������� ������"));
        countries_ru.insert(std::make_pair("ER", "�������"));
        countries_ru.insert(std::make_pair("EE", "�������"));
        countries_ru.insert(std::make_pair("ET", "�������"));
        countries_ru.insert(std::make_pair("ZA", "���"));
        countries_ru.insert(std::make_pair("KR", "����. �����"));
        countries_ru.insert(std::make_pair("GS", "����� ������� � ����� ���������� �������"));
        countries_ru.insert(std::make_pair("JM", "������"));
        countries_ru.insert(std::make_pair("JP", "������"));
        countries_ru.insert(std::make_pair("BV", "������ ����"));
        countries_ru.insert(std::make_pair("NF", "�������"));
        countries_ru.insert(std::make_pair("SH", "�.������ �����"));
        countries_ru.insert(std::make_pair("TC", "Ҹ��� � ������"));
        countries_ru.insert(std::make_pair("WF", "������ � ������"));
    }

    info_getter_localization::~info_getter_localization() {
    }

    void info_getter_localization::process_ip_info(ip_info& info) {
        if (info.country_code2) {
            if (countries_ru.find(info.country_code2.get()) != countries_ru.end()) {
                info.country_name_ru.reset(countries_ru[info.country_code2.get()]);
            }
        }
    }

    bool info_getter_localization::get_db_info(std::string& db_info_string) const {
        return false;
    }
} // namespace geo {