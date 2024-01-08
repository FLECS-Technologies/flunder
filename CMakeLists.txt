# Copyright 2021-2023 FLECS Technologies GmbH
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.21)

include(${CMAKE_SOURCE_DIR}/flecs-build/cmake/flecs-build.cmake)

project(flunder)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    add_subdirectory(flecs-external)
endif()

if(FLECS_BUILD_TESTS)
    include(CTest)
    add_compile_options(-fprofile-arcs -ftest-coverage)
    add_link_options(-lgcov --coverage)
    add_definitions(-DFLECS_UNIT_TEST)
    add_definitions(-DFLECS_CONSTEXPR_UNLESS_TESTED=)
    add_definitions(-DFLECS_FINAL_UNLESS_TESTED=)
else()
    add_definitions(-DFLECS_CONSTEXPR_UNLESS_TESTED=constexpr)
    add_definitions(-DFLECS_FINAL_UNLESS_TESTED=final)
endif()

set(SRC_LIB
    src/client.cpp
    src/to_string.cpp
    src/variable.cpp
    src/impl/client.cpp
)

set(HEADER_LIB
    include/flunder/client.h
    include/flunder/to_string.h
    include/flunder/variable.h
    include/flunder/impl/client.h
)

add_library(flunder OBJECT ${SRC_LIB} ${HEADER_LIB})

set_property(TARGET flunder PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(flunder.shared SHARED $<TARGET_OBJECTS:flunder>)
set_target_properties(flunder.shared PROPERTIES OUTPUT_NAME flunder)

add_library(flunder.static STATIC $<TARGET_OBJECTS:flunder>)
set_target_properties(flunder.static PROPERTIES OUTPUT_NAME flunder_static)

target_include_directories(flunder PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(flunder PRIVATE
    zenohc::lib
)

target_link_libraries(flunder.static INTERFACE
    zenohc::lib
)
target_include_directories(flunder.static INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(flunder.shared INTERFACE
    zenohc::lib
)
target_include_directories(flunder.shared INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(examples)
add_subdirectory(test)
