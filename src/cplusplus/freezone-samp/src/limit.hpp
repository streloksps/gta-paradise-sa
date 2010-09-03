#ifndef LIMIT_HPP
#define LIMIT_HPP
struct limit_def_t {
    int limit_player_count; // Сколько может воспользоваться один игрок. Если = 0 - то без лимита
    int limit_player_time;  // Время действия ограничения, секунды
    int limit_global_count; // Глобальное количество. Если = 0 - то без лимита
    int limit_global_time;  // Глобальное время действия ограничения, секунды
    limit_def_t(): limit_player_count(0), limit_player_time(0), limit_global_count(0), limit_global_time(0) {}
};
#endif // LIMIT_HPP
