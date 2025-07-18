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

cmake_minimum_required(VERSION 3.25)

project(flunder VERSION 4.1.0 LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Werror -Wfatal-errors)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if(FLUNDER_USE_SYSTEM_GTEST)
    find_package(GTest 1.12.1 REQUIRED)
else()
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.17.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

if(FLUNDER_USE_SYSTEM_ZENOHC)
    find_package(zenohc 1.4.0 REQUIRED)
else()
    FetchContent_Declare(
        zenohc
        GIT_REPOSITORY https://github.com/eclipse-zenoh/zenoh-c.git
        GIT_TAG        1.4.0
    )
    set(ZENOHC_BUILD_WITH_UNSTABLE_API TRUE)
    FetchContent_MakeAvailable(zenohc)
endif()

if(FLUNDER_USE_SYSTEM_BOOST)
    if(POLICY CMP0167)
        cmake_policy(SET CMP0167 NEW)
    endif()
    find_package(Boost 1.81.0 REQUIRED)
else()
    # Workaround Boost compilation warning
    add_compile_options("-Wno-aggressive-loop-optimizations")
    foreach(module IN ITEMS
        assert
        config
        container
        core
        intrusive
        lexical_cast
        move
        static_assert
        throw_exception
        type_traits
    )
        FetchContent_Declare(boost_${module}
            GIT_REPOSITORY https://github.com/boostorg/${module}.git
            GIT_TAG boost-1.88.0
        )
        FetchContent_MakeAvailable(boost_${module})
    endforeach()
endif()

if(FLUNDER_USE_SYSTEM_NLOHMANN_JSON)
    find_package(nlohmann::json 3.11.0 REQUIRED)
else()
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.12.0
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# provide macro to export symbols
add_definitions("-DFLECS_EXPORT=__attribute__((visibility(\"default\")))")

if(FLECS_BUILD_TESTS)
    include(CTest)
    add_compile_options(-fprofile-arcs -ftest-coverage)
    add_link_options(-lgcov --coverage)
endif()

set(SRC_LIB
    src/client.cpp
    src/to_string.cpp
    src/variable.cpp
    src/impl/client.cpp
    src/impl/to_bytes.cpp
)

set(HEADER_LIB
    include/flunder/client.h
    include/flunder/to_string.h
    include/flunder/variable.h
    include/flunder/impl/client.h
    include/flunder/impl/to_bytes.h
)

add_library(flunder OBJECT ${SRC_LIB} ${HEADER_LIB})
target_compile_options(flunder PUBLIC -Wno-error=restrict)

set_property(TARGET flunder PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(flunder.shared SHARED $<TARGET_OBJECTS:flunder>)
set_target_properties(flunder.shared PROPERTIES OUTPUT_NAME flunder)
set_target_properties(flunder.shared PROPERTIES SOVERSION 3)

add_library(flunder.static STATIC $<TARGET_OBJECTS:flunder>)
set_target_properties(flunder.static PROPERTIES OUTPUT_NAME flunder_static)

target_include_directories(flunder PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(flunder PRIVATE
    Boost::lexical_cast
    nlohmann_json::nlohmann_json
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
add_subdirectory(pkg)

add_custom_command(TARGET flunder.shared POST_BUILD
    COMMAND echo -n ${PROJECT_VERSION} >latest
)
