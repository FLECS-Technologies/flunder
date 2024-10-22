#pragma once

#include <zenoh.h>

#include <cinttypes>
#include <string>
#include <type_traits>

namespace flunder {
namespace impl {

auto to_bytes(std::int8_t val) //
    -> z_owned_bytes_t;
auto to_bytes(std::int16_t val) //
    -> z_owned_bytes_t;
auto to_bytes(std::int32_t val) //
    -> z_owned_bytes_t;
auto to_bytes(std::int64_t val) //
    -> z_owned_bytes_t;

auto to_bytes(std::uint8_t val) //
    -> z_owned_bytes_t;
auto to_bytes(std::uint16_t val) //
    -> z_owned_bytes_t;
auto to_bytes(std::uint32_t val) //
    -> z_owned_bytes_t;
auto to_bytes(std::uint64_t val) //
    -> z_owned_bytes_t;

auto to_bytes(float val) //
    -> z_owned_bytes_t;
auto to_bytes(double val) //
    -> z_owned_bytes_t;

auto to_bytes(bool val) //
    -> z_owned_bytes_t;

auto to_bytes(const char* val) //
    -> z_owned_bytes_t;
auto to_bytes(std::string_view val) //
    -> z_owned_bytes_t;

} // namespace impl
} // namespace flunder
