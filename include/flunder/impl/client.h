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

#include <zenoh.h>

#include <map>
#include <set>
#include <string>
#include <tuple>
#include <variant>

#include "flunder/client.h"

namespace flunder {
namespace impl {

struct mem_storage_t
{
    std::string name;
    std::string zid;
};

inline auto operator<=>(const mem_storage_t& lhs, const mem_storage_t& rhs)
{
    return lhs.name <=> rhs.name;
}

class client_t
{
public:
    client_t();
    ~client_t();

    FLECS_EXPORT auto connect(std::string_view host, int port) //
        -> int;

    FLECS_EXPORT auto reconnect() //
        -> int;

    FLECS_EXPORT auto is_connected() const noexcept //
        -> bool;

    FLECS_EXPORT auto disconnect() //
        -> int;

    FLECS_EXPORT auto publish(
        std::string_view topic,
        z_owned_bytes_t value,
        std::string_view encoding) const //
        -> int;

    FLECS_EXPORT auto publish_bool(
        std::string_view topic,
        z_owned_bytes_t value) const //
        -> int;

    FLECS_EXPORT auto publish_int8(
        std::string_view topic,
        z_owned_bytes_t value) const //
        -> int;
    FLECS_EXPORT auto publish_int16(
        std::string_view topic,
        z_owned_bytes_t value) const //
        -> int;
    FLECS_EXPORT auto publish_int32(
        std::string_view topic,
        z_owned_bytes_t value) const //
        -> int;
    FLECS_EXPORT auto publish_int64(
        std::string_view topic,
        z_owned_bytes_t value) const //
        -> int;
    FLECS_EXPORT auto publish_int128(
        std::string_view topic,
        z_owned_bytes_t value) const //
        -> int;

    FLECS_EXPORT auto publish_uint8(
        std::string_view topic,
        z_owned_bytes_t value) const //
        -> int;
    FLECS_EXPORT auto publish_uint16(
        std::string_view topic,
        z_owned_bytes_t value) const //
        -> int;
    FLECS_EXPORT auto publish_uint32(
        std::string_view topic,
        z_owned_bytes_t value) const //
        -> int;
    FLECS_EXPORT auto publish_uint64(
        std::string_view topic,
        z_owned_bytes_t value) const //
        -> int;
    FLECS_EXPORT auto publish_uint128(
        std::string_view topic,
        z_owned_bytes_t value) const //
        -> int;

    FLECS_EXPORT auto publish_float(
        std::string_view topic,
        z_owned_bytes_t value) const //
        -> int;

    FLECS_EXPORT auto publish_double(
        std::string_view topic,
        z_owned_bytes_t value) const //
        -> int;

    FLECS_EXPORT auto publish_string(
        std::string_view topic,
        z_owned_bytes_t value) const //
        -> int;

    FLECS_EXPORT auto publish_raw(
        std::string_view topic,
        const void* payload,
        size_t payloadlen) const //
        -> int;

    FLECS_EXPORT auto publish_custom(
        std::string_view topic,
        const void* payload,
        size_t payloadlen,
        std::string_view encoding) const //
        -> int;

    using subscribe_cbk_t = flunder::client_t::subscribe_cbk_t;
    using subscribe_cbk_userp_t = flunder::client_t::subscribe_cbk_userp_t;
    FLECS_EXPORT auto subscribe(
        flunder::client_t* client, std::string_view topic, subscribe_cbk_t cbk) //
        -> int;

    FLECS_EXPORT auto subscribe(
        flunder::client_t* client,
        std::string_view topic,
        subscribe_cbk_userp_t cbk,
        const void* userp) //
        -> int;

    FLECS_EXPORT auto unsubscribe(std::string_view topic) //
        -> int;

    FLECS_EXPORT auto add_mem_storage(std::string topic, std::string_view name) //
        -> int;

    FLECS_EXPORT auto remove_mem_storage(std::string name) //
        -> int;

    FLECS_EXPORT auto get(std::string_view topic) const //
        -> std::tuple<int, std::vector<variable_t>>;

    FLECS_EXPORT auto erase(std::string_view topic) //
        -> int;

    /*! Function pointer to receive callback */
    using subscribe_cbk_var_t = std::variant<subscribe_cbk_t, subscribe_cbk_userp_t>;

    struct subscribe_ctx_t
    {
        flunder::client_t* _client;
        z_owned_subscriber_t _sub;
        subscribe_cbk_var_t _cbk;
        const void* _userp;
        bool _once;
    };

private:
    FLECS_EXPORT auto do_publish(
        std::string_view topic,
        z_owned_encoding_t encoding,
        z_owned_bytes_t value) const //
        -> int;

    FLECS_EXPORT auto do_subscribe(
        flunder::client_t* client,
        std::string_view topic,
        subscribe_cbk_var_t cbk,
        const void* userp) //
        -> int;

    FLECS_EXPORT auto determine_connected_router_count() const //
        -> int;

    std::set<mem_storage_t> _mem_storages;

    std::string _host;
    std::uint16_t _port;
    z_owned_session_t _z_session;
    std::map<std::string, subscribe_ctx_t> _subscriptions;
};

auto to_string(const z_loaned_encoding_t* encoding) //
    -> std::string;

auto ntp64_to_unix_time(std::uint64_t ntp_time) //
    -> uint64_t;

} // namespace impl
} // namespace flunder
