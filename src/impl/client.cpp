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

#include "flunder/impl/client.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <nlohmann/json.hpp>
#include <thread>
#include <tuple>

#include "flunder/to_string.h"

namespace flunder {
namespace impl {

/** @todo */
template <class... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};

static auto lib_subscribe_callback(z_loaned_sample_t* sample, void* arg) //
    -> void
{
    const auto* ctx = static_cast<const client_t::subscribe_ctx_t*>(arg);
    if (!ctx->_once) {
        return;
    }

    auto keyexpr = z_view_string_t{};
    z_keyexpr_as_view_string(z_sample_keyexpr(sample), &keyexpr);

    auto payload_reader = z_bytes_get_reader(z_sample_payload(sample));
    const auto payload_len = z_bytes_reader_remaining(&payload_reader);
    auto payload = std::string(payload_len, '\0');
    z_bytes_reader_read(&payload_reader, reinterpret_cast<uint8_t*>(payload.data()), payload_len);

    auto encoding = z_owned_string_t{};
    z_encoding_to_string(z_sample_encoding(sample), &encoding);
    auto timestamp = z_sample_timestamp(sample);

    const auto var = variable_t{
        std::string{z_string_data(z_loan(keyexpr)), z_string_len(z_loan(keyexpr))},
        std::move(payload),
        std::string{z_string_data(z_loan(encoding)), z_string_len(z_loan(encoding))},
        flunder::to_string(ntp64_to_unix_time(z_timestamp_ntp64_time(timestamp)))};

    z_drop(z_move(encoding));

    std::visit(
        overload{
            // call callback without userdata
            [&](client_t::subscribe_cbk_t cbk) { cbk(ctx->_client, &var); },
            // call callback with userdata
            [&](client_t::subscribe_cbk_userp_t cbk) { cbk(ctx->_client, &var, ctx->_userp); }},
        ctx->_cbk);
}

client_t::client_t()
    : _mem_storages{}
    , _host{}
    , _port{}
    , _z_session{}
    , _subscriptions{}
{}

client_t::~client_t()
{}

auto client_t::connect(std::string_view host, int port) //
    -> int
{
    disconnect();

    _host = host;
    _port = port;

    const auto len = std::snprintf(nullptr, 0, "[\"tcp/%s:%d\"]", host.data(), port);
    auto remote = std::string(len, '\0');
    std::snprintf(remote.data(), remote.length() + 1, "[\"tcp/%s:%d\"]", host.data(), port);

    auto config = z_owned_config_t{};
    {
        auto res = z_config_default(&config);
        res |= zc_config_insert_json5(z_loan_mut(config), Z_CONFIG_CONNECT_KEY, remote.c_str());
        res |= zc_config_insert_json5(z_loan_mut(config), Z_CONFIG_MODE_KEY, R"#("client")#");
        res |= zc_config_insert_json5(z_loan_mut(config), Z_CONFIG_MULTICAST_SCOUTING_KEY, "false");
        res |= zc_config_insert_json5(z_loan_mut(config), "timestamping/enabled", "true");
        if (res) {
            return -1;
        }
    }

    const auto res = z_open(&_z_session, z_move(config), nullptr);
    if (res < 0) {
        std::fprintf(stderr, "[flunder] Could not connect to %s:%d: %d\n", host.data(), port, res);
        z_drop(z_move(_z_session));
        _z_session = z_owned_session_t{};
        return -1;
    }

    return 0;
}

auto client_t::is_connected() const noexcept //
    -> bool
{
    return z_internal_check(_z_session);
}

auto client_t::reconnect() //
    -> int
{
    const auto host = _host;
    const auto port = _port;

    disconnect();
    return connect(host, port);
}

auto client_t::disconnect() //
    -> int
{
    while (!_subscriptions.empty()) {
        unsubscribe(_subscriptions.rbegin()->first);
    }
    while (!_mem_storages.empty()) {
        remove_mem_storage(_mem_storages.rbegin()->name);
    }
    if (is_connected()) {
        auto opt = z_close_options_t{};
        z_close_options_default(&opt);
        z_close(z_loan_mut(_z_session), &opt);
        z_drop(z_move(_z_session));
        _z_session = z_owned_session_t{};
    }
    _host.clear();
    _port = 0;

    return 0;
}

auto client_t::publish(
    std::string_view topic,
    z_owned_bytes_t value,
    std::string_view encoding) const //
    -> int
{
    auto enc = z_owned_encoding_t{};
    z_encoding_from_str(&enc, encoding.data());
    return do_publish(topic, enc, value);
}

auto client_t::publish_bool(
    std::string_view topic,
    z_owned_bytes_t value) const //
    -> int
{
    return publish(std::move(topic), std::move(value), "text/plain;bool");
}

auto client_t::publish_int8(
    std::string_view topic,
    z_owned_bytes_t value) const //
    -> int
{
    return publish(std::move(topic), std::move(value), "text/plain;int8");
}
auto client_t::publish_int16(
    std::string_view topic,
    z_owned_bytes_t value) const //
    -> int
{
    return publish(std::move(topic), std::move(value), "text/plain;int16");
}
auto client_t::publish_int32(
    std::string_view topic,
    z_owned_bytes_t value) const //
    -> int
{
    return publish(std::move(topic), std::move(value), "text/plain;int32");
}
auto client_t::publish_int64(
    std::string_view topic,
    z_owned_bytes_t value) const //
    -> int
{
    return publish(std::move(topic), std::move(value), "text/plain;int64");
}
auto client_t::publish_int128(
    std::string_view topic,
    z_owned_bytes_t value) const //
    -> int
{
    return publish(std::move(topic), std::move(value), "text/plain;int128");
}

auto client_t::publish_uint8(
    std::string_view topic,
    z_owned_bytes_t value) const //
    -> int
{
    return publish(std::move(topic), std::move(value), "text/plain;uint8");
}
auto client_t::publish_uint16(
    std::string_view topic,
    z_owned_bytes_t value) const //
    -> int
{
    return publish(std::move(topic), std::move(value), "text/plain;uint16");
}
auto client_t::publish_uint32(
    std::string_view topic,
    z_owned_bytes_t value) const //
    -> int
{
    return publish(std::move(topic), std::move(value), "text/plain;uint32");
}
auto client_t::publish_uint64(
    std::string_view topic,
    z_owned_bytes_t value) const //
    -> int
{
    return publish(std::move(topic), std::move(value), "text/plain;uint64");
}
auto client_t::publish_uint128(
    std::string_view topic,
    z_owned_bytes_t value) const //
    -> int
{
    return publish(std::move(topic), std::move(value), "text/plain;uint128");
}

auto client_t::publish_float(
    std::string_view topic,
    z_owned_bytes_t value) const //
    -> int
{
    static_assert(sizeof(float) == 4);
    return publish(std::move(topic), value, "text/plain;float32");
}
auto client_t::publish_double(
    std::string_view topic,
    z_owned_bytes_t value) const //
    -> int
{
    static_assert(sizeof(double) == 8);
    return publish(std::move(topic), value, "text/plain;float64");
}

auto client_t::publish_string(
    std::string_view topic,
    z_owned_bytes_t value) const //
    -> int
{
    return publish(std::move(topic), value, "text/plain");
}

auto client_t::publish_raw(
    std::string_view topic,
    const void* payload,
    size_t payloadlen) const //
    -> int
{
    return publish_custom(topic, payload, payloadlen, "application/octet-stream");
}

auto client_t::publish_custom(
    std::string_view topic,
    const void* payload,
    size_t payloadlen,
    std::string_view encoding) const //
    -> int
{
    auto enc = z_owned_encoding_t{};
    z_encoding_from_str(&enc, encoding.data());

    auto value = z_owned_bytes_t{};
    z_bytes_copy_from_buf(&value, reinterpret_cast<const uint8_t*>(payload), payloadlen);
    return do_publish(topic, enc, value);
}

auto client_t::do_publish(
    std::string_view topic,
    z_owned_encoding_t encoding,
    z_owned_bytes_t value) const //
    -> int
{
    if (!is_connected()) {
        return -1;
    }

    auto options = z_put_options_t{};
    z_put_options_default(&options);
    options.encoding = z_move(encoding);
    options.congestion_control = z_congestion_control_t::Z_CONGESTION_CONTROL_BLOCK;
    options.reliability = z_reliability_t::Z_RELIABILITY_RELIABLE;

    auto keyexpr = z_view_keyexpr_t{};
    z_view_keyexpr_from_str(&keyexpr, topic.starts_with('/') ? topic.data() + 1 : topic.data());

    const auto res = z_put(z_loan(_z_session), z_loan(keyexpr), z_move(value), &options);

    return (res == 0) ? 0 : -1;
}

auto client_t::subscribe(
    flunder::client_t* client,
    std::string_view topic,
    client_t::subscribe_cbk_t cbk) //
    -> int
{
    return do_subscribe(client, topic, subscribe_cbk_var_t{cbk}, nullptr);
}

auto client_t::subscribe(
    flunder::client_t* client,
    std::string_view topic,
    client_t::subscribe_cbk_userp_t cbk,
    const void* userp) //
    -> int
{
    return do_subscribe(client, topic, subscribe_cbk_var_t{cbk}, userp);
}

auto client_t::do_subscribe(
    flunder::client_t* client,
    std::string_view topic,
    subscribe_cbk_var_t cbk,
    const void* userp) //
    -> int
{
    if (!is_connected()) {
        return -1;
    }

    const char* topic_str = topic.starts_with('/') ? topic.data() + 1 : topic.data();
    if (_subscriptions.contains(topic_str)) {
        return -1;
    }

    auto keyexpr = z_view_keyexpr_t{};
    z_view_keyexpr_from_str(&keyexpr, topic_str);

    auto res = _subscriptions.emplace(topic_str, subscribe_ctx_t{client, {}, cbk, userp, false});
    if (!res.second) {
        return -1;
    }
    auto& ctx = res.first->second;

    auto options = z_subscriber_options_t{};
    z_subscriber_options_default(&options);

    auto closure = z_owned_closure_sample_t{};
    z_closure(&closure, lib_subscribe_callback, nullptr, &ctx);
    const auto subscribe_res = z_declare_subscriber(
        z_loan(_z_session),
        &ctx._sub,
        z_loan(keyexpr),
        z_move(closure),
        &options);

    if (subscribe_res < 0) {
        _subscriptions.erase(res.first);
        return subscribe_res;
    }

    const auto [unused, vars] = get(topic_str);
    for (const auto& var : vars) {
        std::visit(
            overload{
                // call callback without userdata
                [&](client_t::subscribe_cbk_t cbk) { cbk(ctx._client, &var); },
                // call callback with userdata
                [&](client_t::subscribe_cbk_userp_t cbk) { cbk(ctx._client, &var, ctx._userp); }},
            ctx._cbk);
    }
    ctx._once = true;

    return 0;
}

auto client_t::determine_connected_router_count() const //
    -> int
{
    int routers = 0;
    auto lambda = [](const struct z_id_t*, void* counter) { *static_cast<int*>(counter) += 1; };

    auto callback = z_owned_closure_zid_t{};
    z_closure(&callback, lambda, nullptr, &routers);
    if (z_info_routers_zid(
            z_session_loan(&_z_session),
            reinterpret_cast<z_moved_closure_zid_t*>(&callback)) != 0) {
        return 0;
    }
    return routers;
}

auto client_t::unsubscribe(std::string_view topic) //
    -> int
{
    const auto keyexpr = topic.starts_with('/') ? topic.data() + 1 : topic.data();

    auto it = _subscriptions.find(keyexpr);
    if (it == _subscriptions.cend()) {
        return -1;
    }

    z_undeclare_subscriber(z_move(it->second._sub));
    _subscriptions.erase(it);

    return 0;
}

static auto router_zid(const z_id_t* zid, void* ctx) //
    -> void
{
    auto& res = *reinterpret_cast<std::string*>(ctx);
    for (int i = 0; i < 16; ++i) {
        sprintf(&res.data()[2 * i], "%02x", zid->id[15 - i]);
    }
}

auto client_t::add_mem_storage(
    std::string name,
    std::string_view topic) //
    -> int
{
    if (!is_connected()) {
        return -1;
    }

    if (_mem_storages.contains(mem_storage_t{name, {}})) {
        return -1;
    }

    auto zid = std::string(32, '0');
    {
        auto cbk = z_owned_closure_zid_t{};
        z_closure(&cbk, router_zid, nullptr, reinterpret_cast<void*>(&zid));
        const auto res = z_info_routers_zid(z_loan(_z_session), z_move(cbk));
        if (res != 0) {
            return -1;
        }
    }

    const auto keyexpr = topic.starts_with('/') ? topic.data() + 1 : topic.data();

    const auto req = nlohmann::json({{"key_expr", keyexpr}, {"volume", "memory"}}).dump();
    const auto admin_keyexpr =
        ("@/" + zid + "/router/config/plugins/storage_manager/storages/").append(name);

    const auto res = publish_custom(admin_keyexpr, req.data(), req.size(), "application/json");
    if (res != 0) {
        return -1;
    }

    _mem_storages.insert(mem_storage_t{std::move(name), std::move(zid)});

    return 0;
}

auto client_t::remove_mem_storage(std::string name) //
    -> int
{
    const auto it = _mem_storages.find(mem_storage_t{name, {}});
    if (it == _mem_storages.end()) {
        return -1;
    }

    const auto admin_keyexpr =
        ("@/" + it->zid + "/router/config/plugins/storage_manager/storages/").append(name);

    auto keyexpr = z_view_keyexpr_t{};
    z_view_keyexpr_from_str(&keyexpr, admin_keyexpr.c_str());

    auto opts = z_delete_options_t{};
    z_delete_options_default(&opts);
    const auto res = z_delete(z_loan(_z_session), z_loan(keyexpr), &opts);

    if (res != 0) {
        return -1;
    }

    _mem_storages.erase(mem_storage_t{std::move(name), {}});

    return 0;
}

auto client_t::get(std::string_view topic) const //
    -> std::tuple<int, std::vector<variable_t>>
{
    auto vars = std::vector<variable_t>{};

    if (!is_connected()) {
        return {-1, vars};
    }

    auto keyexpr = z_view_keyexpr_t{};
    const auto res =
        z_view_keyexpr_from_str(&keyexpr, topic.starts_with('/') ? topic.data() + 1 : topic.data());
    if (res < 0) {
        return {res, vars};
    }

    auto options = z_get_options_t{};
    z_get_options_default(&options);
    options.target = Z_QUERY_TARGET_ALL;

    auto handler = z_owned_fifo_handler_reply_t{};
    auto closure = z_owned_closure_reply_t{};
    z_fifo_channel_reply_new(&closure, &handler, 64);

    z_get(z_loan(_z_session), z_loan(keyexpr), "", z_move(closure), &options);

    auto reply = z_owned_reply_t{};
    for (z_result_t res = z_recv(z_loan(handler), &reply); res == Z_OK;
         res = z_recv(z_loan(handler), &reply)) {
        if (z_reply_is_ok(z_loan(reply))) {
            const auto sample = z_reply_ok(z_loan(reply));

            auto keyexpr = z_view_string_t{};
            z_keyexpr_as_view_string(z_sample_keyexpr(sample), &keyexpr);

            auto keystr = std::string{"/"} + std::string{z_string_data(z_loan(keyexpr))};
            if (keystr.starts_with("/@")) {
                continue;
            }

            auto payload_reader = z_bytes_get_reader(z_sample_payload(sample));
            auto payload_len = z_bytes_reader_remaining(&payload_reader);
            auto payload = std::string(payload_len + 1, '\0');
            z_bytes_reader_read(
                &payload_reader,
                reinterpret_cast<uint8_t*>(payload.data()),
                payload_len);

            vars.emplace_back(
                std::move(keystr),
                std::move(payload),
                to_string(z_sample_encoding(sample)),
                flunder::to_string(
                    ntp64_to_unix_time(z_timestamp_ntp64_time(z_sample_timestamp(sample)))));
        }
    }

    z_drop(z_move(reply));
    z_drop(z_move(handler));

    return {0, vars};
}

auto client_t::erase(std::string_view topic) //
    -> int
{
    auto keyexpr = z_view_keyexpr_t{};
    z_view_keyexpr_from_str(&keyexpr, topic.starts_with('/') ? topic.data() + 1 : topic.data());

    auto options = z_delete_options_t{};
    z_delete_options_default(&options);

    const auto res = z_delete(z_loan(_z_session), z_loan(keyexpr), &options);

    return res;
}

auto to_string(const z_loaned_encoding_t* encoding) //
    -> std::string
{
    auto tmp = z_owned_string_t{};
    z_encoding_to_string(encoding, &tmp);

    auto str = std::string{z_string_data(z_loan(tmp)), z_string_len(z_loan(tmp))};
    z_drop(z_move(tmp));

    return str;
}

auto ntp64_to_unix_time(std::uint64_t ntp_time) //
    -> uint64_t
{
    //           ntp 64-bit time
    // byte    7        6        5        4
    //  -------- -------- -------- --------
    // |             seconds               |
    //  -------- -------- -------- --------
    //
    // byte    3        2        1        0
    //  -------- -------- -------- --------
    // |            fractions              |
    //  -------- -------- -------- --------
    //
    // 1 fraction == 1/2^32 seconds (approx 232 ps)

    const auto seconds = ntp_time >> 32;
    const auto fractions = static_cast<double>(ntp_time & 0xffffffff);
    const auto unix_time = static_cast<uint64_t>(
        (seconds + (fractions / std::numeric_limits<std::uint32_t>::max())) * 1'000'000'000);

    return unix_time;
}

} // namespace impl
} // namespace flunder
