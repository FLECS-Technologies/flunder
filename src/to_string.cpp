#include <boost/lexical_cast.hpp>
#include <string>
#include <type_traits>

#include "flunder/to_string.h"

namespace flunder {

template <typename T>
auto to_string_impl(T val) //
    -> std::string
{
    return boost::lexical_cast<std::string>(val);
}

auto to_string(std::int8_t val) //
    -> std::string
{
    return to_string_impl(val);
}
auto to_string(std::int16_t val) //
    -> std::string
{
    return to_string_impl(val);
}
auto to_string(std::int32_t val) //
    -> std::string
{
    return to_string_impl(val);
}
auto to_string(std::int64_t val) //
    -> std::string
{
    return to_string_impl(val);
}

auto to_string(std::uint8_t val) //
    -> std::string
{
    return to_string_impl(val);
}
auto to_string(std::uint16_t val) //
    -> std::string
{
    return to_string_impl(val);
}
auto to_string(std::uint32_t val) //
    -> std::string
{
    return to_string_impl(val);
}
auto to_string(std::uint64_t val) //
    -> std::string
{
    return to_string_impl(val);
}

auto to_string(float val) //
    -> std::string
{
    return to_string_impl(val);
}
auto to_string(double val) //
    -> std::string
{
    return to_string_impl(val);
}

auto to_string(bool val) //
    -> std::string
{
    return val ? "true" : "false";
}

auto to_string(const char* val) //
    -> std::string
{
    return to_string_impl(val);
}
auto to_string(std::string_view val) //
    -> std::string
{
    return to_string_impl(val);
}

} // namespace flunder