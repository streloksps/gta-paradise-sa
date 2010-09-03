#ifndef LIMIT_HPP
#define LIMIT_HPP
struct limit_def_t {
    int limit_player_count; // ������� ����� ��������������� ���� �����. ���� = 0 - �� ��� ������
    int limit_player_time;  // ����� �������� �����������, �������
    int limit_global_count; // ���������� ����������. ���� = 0 - �� ��� ������
    int limit_global_time;  // ���������� ����� �������� �����������, �������
    limit_def_t(): limit_player_count(0), limit_player_time(0), limit_global_count(0), limit_global_time(0) {}
};
#endif // LIMIT_HPP
