#ifndef STREAMER_COORD_WORKS_HPP
#define STREAMER_COORD_WORKS_HPP

struct pos3;
namespace streamer {
    // Класс, обеспечивающий возращения координат с итемов
    // Если даются ошибки линковки на данный класс, значит мы не специализировали данную структуру для нужных типов
    template <typename item_t>
    struct coord_works {
        
        // Методы, необходимые для грида.
        inline static bool      get_boundary_box(item_t const& item, float& x_min, float& y_min, float& z_min, float& x_max, float& y_max, float& z_max); // Если метод вернул лож, то считается что может быть любое знанчение (аргументы использоваться не должны)
        inline static int       get_interior(item_t const& item); // Может вернуть interior_any
        inline static int       get_world(item_t const& item); // Может вернуть world_any

        // Методы, необходимые для упорядочивания результатов поиска
        typedef float metric_t;
        inline static metric_t  get_metric(item_t const& item, pos3 const& pos);

        // Методы, необходимые для детальной проверки
        inline static bool      get_is_point_in(item_t const& item, pos3 const& pos); // Проверять толькоо x, y, z, так как интерьер и мир уже отобран гридом
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
