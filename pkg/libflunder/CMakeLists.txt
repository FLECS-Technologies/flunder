    # Copyright 2021-2024 FLECS Technologies GmbH
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

include(GNUInstallDirs)

install(
    TARGETS flunder.shared LIBRARY
    NAMELINK_SKIP
    COMPONENT runtime
    EXCLUDE_FROM_ALL
)
get_target_property(zenohc-alias zenohc::lib ALIASED_TARGET)

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    get_target_property(zenohc-lib ${zenohc-alias} IMPORTED_LOCATION_RELEASE)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
    get_target_property(zenohc-lib ${zenohc-alias} IMPORTED_LOCATION_MINSIZEREL)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    get_target_property(zenohc-lib ${zenohc-alias} IMPORTED_LOCATION_RELWITHDEBINFO)
else()
    get_target_property(zenohc-lib ${zenohc-alias} IMPORTED_LOCATION_DEBUG)
endif()

install(
    FILES ${zenohc-lib}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    PERMISSIONS GROUP_READ GROUP_EXECUTE
    PERMISSIONS WORLD_READ WORLD_EXECUTE
    COMPONENT runtime
    EXCLUDE_FROM_ALL
)
install(FILES copyright
    DESTINATION share/doc/libflunder
    COMPONENT runtime
    EXCLUDE_FROM_ALL
)
