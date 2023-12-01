#pragma once

#include <cinttypes>
#include <string>
#include <type_traits>

namespace flunder {

FLECS_EXPORT auto to_string(std::int8_t val) //
    -> std::string;
FLECS_EXPORT auto to_string(std::int16_t val) //
    -> std::string;
FLECS_EXPORT auto to_string(std::int32_t val) //
    -> std::string;
FLECS_EXPORT auto to_string(std::int64_t val) //
    -> std::string;

FLECS_EXPORT auto to_string(std::uint8_t val) //
    -> std::string;
FLECS_EXPORT auto to_string(std::uint16_t val) //
    -> std::string;
FLECS_EXPORT auto to_string(std::uint32_t val) //
    -> std::string;
FLECS_EXPORT auto to_string(std::uint64_t val) //
    -> std::string;

FLECS_EXPORT auto to_string(float val) //
    -> std::string;
FLECS_EXPORT auto to_string(double val) //
    -> std::string;

FLECS_EXPORT auto to_string(bool val) //
    -> std::string;

FLECS_EXPORT auto to_string(const char* val) //
    -> std::string;
FLECS_EXPORT auto to_string(std::string_view val) //
    -> std::string;

} // namespace flunder
