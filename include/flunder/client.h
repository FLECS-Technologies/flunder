// Copyright 2021-2023 FLECS Technologies GmbH
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifndef FLECS_EXPORT
#define FLECS_EXPORT
#endif // FLECS_EXPORT

#ifndef __cplusplus
#ifndef FLECS_FLUNDER_HOST
#define FLECS_FLUNDER_HOST "flecs-flunder"
#endif // FLECS_FLUNDER_HOST
#ifndef FLECS_FLUNDER_PORT
#define FLECS_FLUNDER_PORT 7447
#endif // FLECS_FLUNDER_PORT
#endif // __cplusplus

#include "flunder/variable.h"

#ifndef __cplusplus
#include <inttypes.h>
#include <stdbool.h>
#else

#include <cinttypes>
#include <functional>
#include <memory>
#include <string_view>
#include <tuple>
#include <vector>

namespace flunder {
namespace impl {
class client_t;
} // namespace impl

/*! DNS name of the default flunder broker */
constexpr const char* FLUNDER_HOST = "flecs-flunder";
/*! Port of the default flunder broker */
constexpr const int FLUNDER_PORT = 7447;

class client_t
{
public:
    /*! @brief Constructor
     */
    FLECS_EXPORT client_t();

    /*! @brief Copy constructor (deleted)
     */
    FLECS_EXPORT client_t(const client_t&) = delete;

    /*! @brief Move constructor
     */
    FLECS_EXPORT client_t(client_t&& other);

    /*! @brief copy-assignment operator (deleted)
     */
    FLECS_EXPORT client_t& operator=(const client_t&) = delete;

    /*! @brief move-assignment operator
     */
    FLECS_EXPORT client_t& operator=(client_t&& other);

    /*! @brief Destructor
     */
    FLECS_EXPORT ~client_t();

    FLECS_EXPORT auto connect() //
        -> int;
    FLECS_EXPORT auto connect(std::string_view host, int port) //
        -> int;

    FLECS_EXPORT auto is_connected() const noexcept //
        -> bool;

    FLECS_EXPORT auto reconnect() //
        -> int;

    FLECS_EXPORT auto disconnect() //
        -> int;

    /* publish typed data to live subscribers */
    /* bool */
    FLECS_EXPORT auto publish(std::string_view topic, bool value) const //
        -> int;
    /* integer-types */
    FLECS_EXPORT auto publish(std::string_view topic, std::int8_t value) const //
        -> int;
    FLECS_EXPORT auto publish(std::string_view topic, std::int16_t value) const //
        -> int;
    FLECS_EXPORT auto publish(std::string_view topic, std::int32_t value) const //
        -> int;
    FLECS_EXPORT auto publish(std::string_view topic, std::int64_t value) const //
        -> int;
    FLECS_EXPORT auto publish(std::string_view topic, std::uint8_t value) const //
        -> int;
    FLECS_EXPORT auto publish(std::string_view topic, std::uint16_t value) const //
        -> int;
    FLECS_EXPORT auto publish(std::string_view topic, std::uint32_t value) const //
        -> int;
    FLECS_EXPORT auto publish(std::string_view topic, std::uint64_t value) const //
        -> int;
    /* floating-point-types */
    FLECS_EXPORT auto publish(std::string_view topic, float value) const //
        -> int;
    FLECS_EXPORT auto publish(std::string_view topic, double value) const //
        -> int;
    /* string-types */
    FLECS_EXPORT auto publish(std::string_view topic, const std::string& value) const //
        -> int;
    FLECS_EXPORT auto publish(std::string_view topic, const std::string_view& value) const //
        -> int;
    FLECS_EXPORT auto publish(std::string_view topic, const char* value) const //
        -> int;
    /* raw data */
    FLECS_EXPORT auto publish(std::string_view topic, const void* data, size_t len) const //
        -> int;
    /* custom data */
    FLECS_EXPORT auto publish(
        std::string_view topic, const void* data, size_t len, std::string_view encoding) const //
        -> int;

    using subscribe_cbk_t = std::function<void(client_t*, const variable_t*)>;
    using subscribe_cbk_userp_t = std::function<void(client_t*, const variable_t*, const void*)>;

    /* subscribe to live data */
    FLECS_EXPORT auto subscribe(std::string_view topic, subscribe_cbk_t cbk) //
        -> int;
    /* subscribe to live data with userdata */
    FLECS_EXPORT auto subscribe(
        std::string_view topic, subscribe_cbk_userp_t cbk, const void* userp) //
        -> int;
    /* unsubscribe from live data */
    FLECS_EXPORT auto unsubscribe(std::string_view topic) //
        -> int;

    FLECS_EXPORT auto add_mem_storage(std::string_view name, std::string_view topic) //
        -> int;
    FLECS_EXPORT auto remove_mem_storage(std::string_view name) //
        -> int;

    /* get data from storage */
    FLECS_EXPORT auto get(std::string_view topic) const //
        -> std::tuple<int, std::vector<variable_t> >;
    /* delete data from storage */
    FLECS_EXPORT auto erase(std::string_view topic) //
        -> int;

private:
    FLECS_EXPORT friend auto swap(client_t& lhs, client_t& rhs) noexcept //
        -> void;

    std::unique_ptr<impl::client_t> _impl;
};

extern "C" {
#endif // __cplusplus

typedef void (*flunder_subscribe_cbk_t)(void*, const variable_t*);
typedef void (*flunder_subscribe_cbk_userp_t)(void*, const variable_t*, void*);

FLECS_EXPORT void* flunder_client_new(void);

FLECS_EXPORT void flunder_client_destroy(void* flunder);

FLECS_EXPORT int flunder_connect(void* flunder, const char* host, int port);

FLECS_EXPORT int flunder_reconnect(void* flunder);

FLECS_EXPORT int flunder_disconnect(void* flunder);

FLECS_EXPORT int flunder_subscribe(void* flunder, const char* topic, flunder_subscribe_cbk_t cbk);
FLECS_EXPORT int flunder_subscribe_userp(
    void* flunder, const char* topic, flunder_subscribe_cbk_userp_t cbk, const void* userp);

FLECS_EXPORT int flunder_unsubscribe(void* flunder, const char* topic);

/** make sure to call flunder_variable_list_destroy with the exact values returned */
FLECS_EXPORT int flunder_get(const void* flunder, const char* topic, variable_t** vars, size_t* n);

FLECS_EXPORT int flunder_publish_bool(const void* flunder, const char* topic, bool value);

FLECS_EXPORT int flunder_publish_int(const void* flunder, const char* topic, int value);
FLECS_EXPORT int flunder_publish_int8(const void* flunder, const char* topic, int8_t value);
FLECS_EXPORT int flunder_publish_int16(const void* flunder, const char* topic, int16_t value);
FLECS_EXPORT int flunder_publish_int32(const void* flunder, const char* topic, int32_t value);
FLECS_EXPORT int flunder_publish_int64(const void* flunder, const char* topic, int64_t value);
FLECS_EXPORT int flunder_publish_uint8(const void* flunder, const char* topic, uint8_t value);
FLECS_EXPORT int flunder_publish_uint16(const void* flunder, const char* topic, uint16_t value);
FLECS_EXPORT int flunder_publish_uint32(const void* flunder, const char* topic, uint32_t value);
FLECS_EXPORT int flunder_publish_uint64(const void* flunder, const char* topic, uint64_t value);

FLECS_EXPORT int flunder_publish_float(const void* flunder, const char* topic, float value);
FLECS_EXPORT int flunder_publish_double(const void* flunder, const char* topic, double value);
FLECS_EXPORT int flunder_publish_string(const void* flunder, const char* topic, const char* value);
FLECS_EXPORT int flunder_publish_raw(
    const void* flunder, const char* topic, const void* value, size_t payloadlen);
FLECS_EXPORT int flunder_publish_custom(
    const void* flunder,
    const char* topic,
    const void* value,
    size_t payloadlen,
    const char* encoding);

FLECS_EXPORT int flunder_add_mem_storage(void* flunder, const char* name, const char* topic);
FLECS_EXPORT int flunder_remove_mem_storage(void* flunder, const char* name);

#ifdef __cplusplus
} // extern "C"
} // namespace flunder
#endif // __cplusplus
