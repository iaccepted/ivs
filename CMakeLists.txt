cmake_minimum_required(VERSION 3.10)

project(ivs VERSION 1.0.0 LANGUAGES C CXX)

configure_file(config.h.in config.h)

set(IVS_SO_PATH "/usr/lib64/ivs" CACHE STRING "so install path")

add_subdirectory(lib)
add_subdirectory(app)
add_subdirectory(tests)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_FLAGS "-Wall -Werror -g -O3 -lpthread -rdynamic -fno-stric-aliasing -fstack-protector-strong -pie -Wtrampolines -Wl,-z,noexecstack,-z,relro,-z,now -fPIE")


set(CMAKE_C_FLAGS ${CMAKE_FLAGS})

include_directories(${PROJECT_SOURCE_DIR}/lib)

include_directories(${GLIB_INCLUDE_DIRS})
