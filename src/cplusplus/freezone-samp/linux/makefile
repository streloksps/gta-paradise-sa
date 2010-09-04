#
#  Мейк файл для компиляции мода под линуксом
#
boost_root        := $(FREE_ZONE_BOOST_DIR)
include_glob_dirs := $(boost_root)/boost/tr1/tr1 $(boost_root)

rezult_name       := freezone
source_dirs       := ../../samp-plugin-sdk ../../samp-plugin-sdk/amx ../../hashlib2plus ../../maxmind-geoip ../../utility . core core/buffer core/command core/container core/geo core/geo/detail core/logger core/messages core/messages/player_messages core/player core/samp core/samp/pawn core/serialization core/utility
boost_libs        := signals system regex filesystem thread
boost_libs_prefix := $(boost_root)/stage/lib/libboost_
include_app_dirs  := . ../..
prefix_app_dir    := ../../src/cplusplus/freezone-samp/src/
#link_flags        := -fPIC -shared

boost_libs        := $(addprefix $(boost_libs_prefix), $(boost_libs))
boost_libs        := $(addsuffix $(boost_libs_suffix), $(boost_libs))

source_dirs       := $(addprefix $(prefix_app_dir), $(source_dirs))
include_app_dirs  := $(addprefix $(prefix_app_dir), $(include_app_dirs))

source_files_cpp := $(wildcard $(addsuffix /*.cpp, $(source_dirs)))
object_files_cpp := $(notdir $(source_files_cpp))
object_files_cpp := $(object_files_cpp:.cpp=.o)

source_files_c := $(wildcard $(addsuffix /*.c, $(source_dirs)))
object_files_c := $(notdir $(source_files_c))
object_files_c := $(object_files_c:.c=.o)

$(rezult_name): $(object_files_cpp) $(object_files_c)
	g++ $^ $(boost_libs) -o $@ $(link_flags)
    
VPATH := $(source_dirs)
    
%.o: %.cpp
	g++ -MMD $< -c $(compile_flags) $(addprefix -I, $(include_app_dirs)) $(addprefix -I, $(include_glob_dirs))

%.o: %.c
	gcc -MMD $< -c $(compile_flags) $(addprefix -I, $(include_app_dirs))

include $(wildcard *.d) 
