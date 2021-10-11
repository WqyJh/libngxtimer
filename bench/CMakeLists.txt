# Copyright 2021 Qiying Wang. All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

# using the following command to install benchmark library
# apt install libbenchmark-dev
find_package(benchmark REQUIRED)

add_executable(ngxtimer_bench
    ngxtimer_bench.cc)

target_link_libraries(ngxtimer_bench
    benchmark::benchmark
    ngxtimer
)

add_custom_target(bench
    COMMAND ./ngxtimer_bench
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} # build/bench/
)
