#include "config.hpp"
#include "path_works.hpp"

#ifdef WIN32
#include <windows.h>

std::string get_executable_path() {
    char result[MAX_PATH];
    return std::string(result, GetModuleFileNameA(0, result, sizeof(result)/sizeof(result[0])));
}
#else // #ifdef WIN32
#include <limits.h>
#include <unistd.h>
#include <stdlib.h> 

std::string get_executable_path() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, sizeof(result)/sizeof(result[0]));
    std::string rezult_str(result, (count > 0) ? count : 0);
    if ("unknown" == rezult_str) {
        rezult_str = getenv("_");
    }
    return rezult_str;
}
#endif // #ifdef WIN32
