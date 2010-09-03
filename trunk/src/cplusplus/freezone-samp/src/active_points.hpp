#ifndef ACTIVE_POINTS_HPP
#define ACTIVE_POINTS_HPP
#include <functional>
#include <string>
#include <boost/optional.hpp>
#include "core/player/player_fwd.hpp"
#include "basic_types.hpp"
#include "limit.hpp"

typedef int active_point_handle_t;
typedef std::tr1::function<void (player_ptr_t const& player_ptr, active_point_handle_t handle)> active_point_event_t;
typedef std::tr1::function<std::string (active_point_handle_t handle, player_ptr_t const& player_ptr)> active_point_description_getter_t;

// ��������� ������, ���� ����� ������������ �������� ����� (���� �� ����� - �� ������������ ����� ������)
typedef std::tr1::function<bool (player_ptr_t const& player_ptr, active_point_handle_t handle)> is_can_use_checker_t;

struct active_point_def_t {
    // �������� ��������� �������� �����
    pos3                    pos;                        // ������� �������� �����
    int                     pickup_id;                  // �� ������ ������ ��� ��������� ����� ������������� ��� �����
    float                   visibility_radius;          // ������, ��� ����� � ������� ����� �������� ������ �������� �����
    float                   active_radius;              // ������, ��� ��������� � ������� ���������� �������������� ������ � �������� ������

    // �������� ������ �� �����, ������������ �������� �����
    int                     mapicon_marker_type;        // �������� ������
    float                   mapicon_visibility_radius;  // ������, ��� ��������� � ������� ����� ����� ������. ���� = 0.0f, �� ������ �� ���������


    // ������� �����/������ � ���� active_radius
    active_point_event_t    on_enter;
    active_point_event_t    on_leave;

    // ���������� ������ ������� ����� ��� ����������� ��� ������ ������. �������� ������������� ��� ��������� ������ � visibility_radius
    active_point_description_getter_t description_getter;

    // �������� �������
    limit_def_t             limit_def;

};

// �� �������� ������, ������� ���������� ������������ ������ ������ ��������
enum {checkpoint_pickup_id = -1};

class active_points
{
public:
    active_points();
    virtual ~active_points();

public:
    active_point_handle_t add(pos3 const& pos, int pickup_id, float visibility_radius, float active_radius);
};
#endif // ACTIVE_POINTS_HPP
