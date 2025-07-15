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

#include "flunder/client.h"

#include "flunder/impl/client.h"
#include "flunder/impl/to_bytes.h"
#include "flunder/to_string.h"

namespace flunder {

client_t::client_t()
    : _impl{new impl::client_t{}}
{}

client_t::client_t(client_t&& other)
    : client_t{}
{
    swap(*this, other);
}

client_t& client_t::operator=(client_t&& other)
{
    swap(*this, other);
    return *this;
}

client_t::~client_t()
{
    disconnect();
}

auto client_t::connect() //
    -> int
{
    return connect(FLUNDER_HOST, FLUNDER_PORT);
}

auto client_t::connect(std::string_view host, int port) //
    -> int
{
    return _impl->connect(host, port);
}

auto client_t::is_connected() const noexcept //
    -> bool
{
    return _impl->is_connected();
}

auto client_t::reconnect() //
    -> int
{
    return _impl->reconnect();
}

auto client_t::disconnect() //
    -> int
{
    return _impl->disconnect();
}

/* bool */
auto client_t::publish(std::string_view topic, bool value) const //
    -> int
{
    return _impl->publish_bool(topic, impl::to_bytes(value));
}
/* integer-types */
auto client_t::publish(std::string_view topic, std::int8_t value) const //
    -> int
{
    return _impl->publish_int8(topic, impl::to_bytes(value));
}
auto client_t::publish(std::string_view topic, std::int16_t value) const //
    -> int
{
    return _impl->publish_int16(topic, impl::to_bytes(value));
}
auto client_t::publish(std::string_view topic, std::int32_t value) const //
    -> int
{
    return _impl->publish_int32(topic, impl::to_bytes(value));
}
auto client_t::publish(std::string_view topic, std::int64_t value) const //
    -> int
{
    return _impl->publish_int64(topic, impl::to_bytes(value));
}
auto client_t::publish(std::string_view topic, std::uint8_t value) const //
    -> int
{
    return _impl->publish_uint8(topic, impl::to_bytes(value));
}
auto client_t::publish(std::string_view topic, std::uint16_t value) const //
    -> int
{
    return _impl->publish_uint16(topic, impl::to_bytes(value));
}
auto client_t::publish(std::string_view topic, std::uint32_t value) const //
    -> int
{
    return _impl->publish_uint32(topic, impl::to_bytes(value));
}
auto client_t::publish(std::string_view topic, std::uint64_t value) const //
    -> int
{
    return _impl->publish_uint64(topic, impl::to_bytes(value));
}
/* floating-point-types */
auto client_t::publish(std::string_view topic, float value) const //
    -> int
{
    return _impl->publish_float(topic, impl::to_bytes(value));
}
auto client_t::publish(std::string_view topic, double value) const //
    -> int
{
    return _impl->publish_double(topic, impl::to_bytes(value));
}
/* string-types */
auto client_t::publish(std::string_view topic, const std::string& value) const //
    -> int
{
    return _impl->publish_string(topic, impl::to_bytes(value));
}
auto client_t::publish(std::string_view topic, const std::string_view& value) const //
    -> int
{
    return _impl->publish_string(topic, impl::to_bytes(value));
}
auto client_t::publish(std::string_view topic, const char* value) const //
    -> int
{
    return _impl->publish_string(topic, impl::to_bytes(value));
}

auto client_t::publish(std::string_view topic, const void* data, size_t len) const //
    -> int
{
    return _impl->publish_raw(topic, data, len);
}

auto client_t::publish(
    std::string_view topic, const void* data, size_t len, std::string_view encoding) const //
    -> int
{
    return _impl->publish_custom(topic, data, len, encoding);
}

auto client_t::subscribe(std::string_view topic, subscribe_cbk_t cbk) //
    -> int
{
    return _impl->subscribe(this, std::move(topic), std::move(cbk));
}

auto client_t::subscribe(
    std::string_view topic,
    subscribe_cbk_userp_t cbk,
    const void* userp) //
    -> int
{
    return _impl->subscribe(this, std::move(topic), std::move(cbk), userp);
}

auto client_t::unsubscribe(std::string_view topic) //
    -> int
{
    return _impl->unsubscribe(topic);
}

auto client_t::add_mem_storage(
    std::string_view name,
    std::string_view topic) //
    -> int
{
    return _impl->add_mem_storage(std::string{name}, topic);
}

auto client_t::remove_mem_storage(std::string_view name) //
    -> int
{
    return _impl->remove_mem_storage(std::string{name});
}

auto client_t::get(std::string_view topic) const //
    -> std::tuple<int, std::vector<variable_t>>
{
    return _impl->get(topic);
}

auto client_t::erase(std::string_view topic) //
    -> int
{
    return _impl->erase(topic);
}

auto swap(client_t& lhs, client_t& rhs) noexcept //
    -> void
{
    using std::swap;
    swap(lhs._impl, rhs._impl);
}

extern "C" {

FLECS_EXPORT void* flunder_client_new(void)
{
    return static_cast<void*>(new flunder::client_t{});
}

FLECS_EXPORT void flunder_client_destroy(void* flunder)
{
    delete static_cast<flunder::client_t*>(flunder);
}

FLECS_EXPORT int flunder_connect(void* flunder, const char* host, int port)
{
    return static_cast<flunder::client_t*>(flunder)->connect(host, port);
}

FLECS_EXPORT int flunder_is_connected(const void* flunder)
{
    return static_cast<const flunder::client_t*>(flunder)->is_connected();
}

FLECS_EXPORT int flunder_reconnect(void* flunder)
{
    return static_cast<flunder::client_t*>(flunder)->reconnect();
}

FLECS_EXPORT int flunder_disconnect(void* flunder)
{
    return static_cast<flunder::client_t*>(flunder)->disconnect();
}

FLECS_EXPORT int flunder_subscribe(void* flunder, const char* topic, flunder_subscribe_cbk_t cbk)
{
    auto p = reinterpret_cast<void (*)(client_t*, const variable_t*)>(cbk);
    return static_cast<flunder::client_t*>(flunder)->subscribe(topic, p);
}
FLECS_EXPORT int flunder_subscribe_userp(
    void* flunder, const char* topic, flunder_subscribe_cbk_userp_t cbk, const void* userp)
{
    auto p = reinterpret_cast<void (*)(client_t*, const variable_t*, const void*)>(cbk);
    return static_cast<flunder::client_t*>(flunder)->subscribe(topic, p, userp);
}

FLECS_EXPORT int flunder_unsubscribe(void* flunder, const char* topic)
{
    return static_cast<flunder::client_t*>(flunder)->unsubscribe(topic);
}

FLECS_EXPORT int flunder_get(const void* flunder, const char* topic, variable_t** vars, size_t* n)
{
    *n = 0;
    *vars = nullptr;

    auto [res, v] = static_cast<const flunder::client_t*>(flunder)->get(topic);
    if (v.empty()) {
        return res;
    }

    *n = v.size();
    *vars = new variable_t[*n];
    for (std::size_t i = 0; i < v.size(); ++i) {
        (*vars)[i] = std::move(v[i]);
    }

    return res;
}

FLECS_EXPORT int flunder_publish_bool(const void* flunder, const char* topic, bool value)
{
    return static_cast<const flunder::client_t*>(flunder)->publish(topic, value);
}

FLECS_EXPORT int flunder_publish_int(const void* flunder, const char* topic, int value)
{
    return static_cast<const flunder::client_t*>(flunder)->publish(topic, value);
}

FLECS_EXPORT int flunder_publish_int8(const void* flunder, const char* topic, int8_t value)
{
    return static_cast<const flunder::client_t*>(flunder)->publish(topic, value);
}

FLECS_EXPORT int flunder_publish_int16(const void* flunder, const char* topic, int16_t value)
{
    return static_cast<const flunder::client_t*>(flunder)->publish(topic, value);
}

FLECS_EXPORT int flunder_publish_int32(const void* flunder, const char* topic, int32_t value)
{
    return static_cast<const flunder::client_t*>(flunder)->publish(topic, value);
}

FLECS_EXPORT int flunder_publish_int64(const void* flunder, const char* topic, int64_t value)
{
    return static_cast<const flunder::client_t*>(flunder)->publish(topic, value);
}

FLECS_EXPORT int flunder_publish_uint(const void* flunder, const char* topic, unsigned value)
{
    return static_cast<const flunder::client_t*>(flunder)->publish(topic, value);
}

FLECS_EXPORT int flunder_publish_uint8(const void* flunder, const char* topic, uint8_t value)
{
    return static_cast<const flunder::client_t*>(flunder)->publish(topic, value);
}

FLECS_EXPORT int flunder_publish_uint16(const void* flunder, const char* topic, uint16_t value)
{
    return static_cast<const flunder::client_t*>(flunder)->publish(topic, value);
}

FLECS_EXPORT int flunder_publish_uint32(const void* flunder, const char* topic, uint32_t value)
{
    return static_cast<const flunder::client_t*>(flunder)->publish(topic, value);
}

FLECS_EXPORT int flunder_publish_uint64(const void* flunder, const char* topic, uint64_t value)
{
    return static_cast<const flunder::client_t*>(flunder)->publish(topic, value);
}

FLECS_EXPORT int flunder_publish_float(const void* flunder, const char* topic, float value)
{
    return static_cast<const flunder::client_t*>(flunder)->publish(topic, value);
}

FLECS_EXPORT int flunder_publish_double(const void* flunder, const char* topic, double value)
{
    return static_cast<const flunder::client_t*>(flunder)->publish(topic, value);
}

FLECS_EXPORT int flunder_publish_string(const void* flunder, const char* topic, const char* value)
{
    return static_cast<const flunder::client_t*>(flunder)->publish(topic, value);
}

FLECS_EXPORT int flunder_publish_raw(
    const void* flunder, const char* topic, const void* value, size_t payloadlen)
{
    return static_cast<const flunder::client_t*>(flunder)->publish(topic, value, payloadlen);
}

FLECS_EXPORT int flunder_publish_custom(
    const void* flunder,
    const char* topic,
    const void* value,
    size_t payloadlen,
    const char* encoding)
{
    return static_cast<const flunder::client_t*>(flunder)
        ->publish(topic, value, payloadlen, std::string_view{encoding});
}

FLECS_EXPORT int flunder_add_mem_storage(void* flunder, const char* name, const char* topic)
{
    return static_cast<flunder::client_t*>(flunder)->add_mem_storage(name, topic);
}

FLECS_EXPORT int flunder_remove_mem_storage(void* flunder, const char* name)
{
    return static_cast<flunder::client_t*>(flunder)->remove_mem_storage(name);
}

} // extern "C"

} // namespace flunder
