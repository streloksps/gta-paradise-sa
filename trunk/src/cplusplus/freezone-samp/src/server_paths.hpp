#ifndef SERVER_PATHS_HPP
#define SERVER_PATHS_HPP

// Корень для всех файлов
#define PATH_ROOT_DIR                           "freemode"

// Первый уровень
#define PATH_CONFIG_DIR                         PATH_ROOT_DIR "/etc"
#define PATH_GEO_DIR                            PATH_ROOT_DIR "/geo"
#define PATH_MTA10_DIR                          PATH_ROOT_DIR "/mta10"
#define PATH_IPL_DIR                            PATH_ROOT_DIR "/ipl"
#define PATH_REC_DIR                            PATH_ROOT_DIR "/rec"
#define PATH_TEXTRC_DIR                         PATH_ROOT_DIR "/text_rc"
#define PATH_VAR_DIR                            PATH_ROOT_DIR "/var"


#define PATH_VAR_LOG_DIR                        PATH_VAR_DIR "/log"
#define PATH_VAR_DEBUG_DIR                      PATH_VAR_DIR "/debug"
#define PATH_VAR_CACHE_DIR                      PATH_VAR_DIR "/cache"
#define PATH_VAR_LAST_STATE_DIR                 PATH_VAR_DIR "/state"

#define PATH_GEO_REGEX                          ".*\\.dat"
#define PATH_TEXTRC_REGEX                       ".*\\.text_rc"
#define PATH_CONFIG_REGEX                       ".*\\.conf"

#define PATH_VAR_LOG_FILENAME                   PATH_VAR_LOG_DIR   "/%Y/%m/log%Y%m%d.txt"
#define PATH_VAR_DEBUG_FILENAME                 PATH_VAR_DEBUG_DIR "/%Y/%m/debug%Y%m%d.txt"

#define PATH_CONFIG_CACHE_FILENAME              PATH_VAR_CACHE_DIR "/config.conf"
#define PATH_TEXTRC_CACHE_FILENAME              PATH_VAR_CACHE_DIR "/config.text_rc"

#define PATH_CONFIG_PROCESSED_FILENAME          PATH_VAR_LAST_STATE_DIR "/config.conf"
#define PATH_TEXTRC_PROCESSED_FILENAME          PATH_VAR_LAST_STATE_DIR "/config.text_rc"
#define PATH_VEHICLES_DUMP_FILENAME             PATH_VAR_LAST_STATE_DIR "/vehicles.conf"

#define PATH_DEV_LOG_FILENAME                   "dev_coords.txt"

#endif // SERVER_PATHS_HPP
