#ifndef STREAMER_COORD_WORKS_HPP
#define STREAMER_COORD_WORKS_HPP

struct pos3;
namespace streamer {
    // �����, �������������� ���������� ��������� � ������
    // ���� ������ ������ �������� �� ������ �����, ������ �� �� ���������������� ������ ��������� ��� ������ �����
    template <typename item_t>
    struct coord_works {
        
        // ������, ����������� ��� �����.
        inline static bool      get_boundary_box(item_t const& item, float& x_min, float& y_min, float& z_min, float& x_max, float& y_max, float& z_max); // ���� ����� ������ ���, �� ��������� ��� ����� ���� ����� ��������� (��������� �������������� �� ������)
        inline static int       get_interior(item_t const& item); // ����� ������� interior_any
        inline static int       get_world(item_t const& item); // ����� ������� world_any

        // ������, ����������� ��� �������������� ����������� ������
        typedef float metric_t;
        inline static metric_t  get_metric(item_t const& item, pos3 const& pos);

        // ������, ����������� ��� ��������� ��������
        inline static bool      get_is_point_in(item_t const& item, pos3 const& pos); // ��������� ������� x, y, z, ��� ��� �������� � ��� ��� ������� ������
    };

    template <typename xyz_item_t>
    inline bool create_sphere_boundary_box(xyz_item_t const& xyz_item, float radius, float& x_min, float& y_min, float& z_min, float& x_max, float& y_max, float& z_max) {
        if (radius < 0.0f) radius = 0.0f;
        x_min = xyz_item.x - radius;
        x_max = xyz_item.x + radius;
        y_min = xyz_item.y - radius;
        y_max = xyz_item.y + radius;
        z_min = xyz_item.z - radius;
        z_max = xyz_item.z + radius;
        return true;
    }
} // namespace streamer {

#endif // STREAMER_COORD_WORKS_HPP
