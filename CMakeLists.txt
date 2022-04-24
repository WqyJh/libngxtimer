cmake_minimum_required(VERSION 2.8)

project(ngx-timer)

#add_definitions(-std=c++17)

# generating build/compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-march=native)
add_definitions(-DNGX_TIMER_ALWAYS_INLINE)

option(NGX_TIMER_ALWAYS_INLINE "Enable always inline" ON)
if (NGX_TIMER_ALWAYS_INLINE)
    message(STATUS "NGX_TIMER_ALWAYS_INLINE enabled")
    add_definitions(-DNGX_TIMER_ALWAYS_INLINE)
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

set(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall")

set(PUBLIC_HEADERS
    ngx_rbtree.h
    ngx_timer.h
)

include_directories(${PROJECT_SOURCE_DIR})
set(SRCS
    ngx_timer.c
)

add_library(ngxtimer SHARED ${SRCS})
set_target_properties(ngxtimer PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
set_target_properties(ngxtimer PROPERTIES VERSION 1.0 SOVERSION 1)

add_library(ngxtimer_static STATIC ${SRCS})
set_target_properties(ngxtimer_static PROPERTIES OUTPUT_NAME "ngxtimer")
set_target_properties(ngxtimer_static PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")


include(GNUInstallDirs)
install(TARGETS ngxtimer ngxtimer_static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# -DENABLE_TESTING=ON
option(ENABLE_TESTING "Enable make test" ON)
if (ENABLE_TESTING)
    # apt install libgtest-dev
    enable_testing()
    add_subdirectory(tests)
endif()

# -DENABLE_BENCH=ON
option(ENABLE_BENCH "Enable make bench" ON)
if (ENABLE_BENCH)
    # using the following command to install benchmark library
    # apt install libbenchmark-dev
    add_subdirectory(bench)
endif()
