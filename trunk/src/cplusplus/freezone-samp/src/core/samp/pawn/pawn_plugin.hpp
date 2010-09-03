#ifndef PAWN_PLUGIN_HPP
#define PAWN_PLUGIN_HPP
#include <vector>

typedef std::vector<int> security_tokens_t;

// Возращает коды всех подключенных модулей
security_tokens_t get_security_tokens();

#endif // PAWN_PLUGIN_HPP
