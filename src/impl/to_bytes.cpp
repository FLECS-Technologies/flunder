#include "flunder/impl/to_bytes.h"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>

std::ostream& operator<<(std::ostream& out, const z_owned_bytes_t& bytes)
{
    auto reader = z_bytes_get_reader(z_loan(bytes));
    while (z_bytes_reader_remaining(&reader)) {
        auto c = uint8_t{};
        z_bytes_reader_read(&reader, &c, 1);
        out << static_cast<char>(c);
    }

    return out;
}

std::istream& operator>>(std::istream& in, z_owned_bytes_t& bytes)
{
    auto str = std::string{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
    z_bytes_copy_from_str(&bytes, str.data());
    return in;
}

namespace flunder {
namespace impl {

template <typename T>
auto to_bytes_impl(T val) //
    -> z_owned_bytes_t
{
    return boost::lexical_cast<z_owned_bytes_t>(val);
}

auto to_bytes(std::int8_t val) //
    -> z_owned_bytes_t
{
    return to_bytes_impl(val);
}
auto to_bytes(std::int16_t val) //
    -> z_owned_bytes_t
{
    return to_bytes_impl(val);
}
auto to_bytes(std::int32_t val) //
    -> z_owned_bytes_t
{
    return to_bytes_impl(val);
}
auto to_bytes(std::int64_t val) //
    -> z_owned_bytes_t
{
    return to_bytes_impl(val);
}

auto to_bytes(std::uint8_t val) //
    -> z_owned_bytes_t
{
    return to_bytes_impl(val);
}
auto to_bytes(std::uint16_t val) //
    -> z_owned_bytes_t
{
    return to_bytes_impl(val);
}
auto to_bytes(std::uint32_t val) //
    -> z_owned_bytes_t
{
    return to_bytes_impl(val);
}
auto to_bytes(std::uint64_t val) //
    -> z_owned_bytes_t
{
    return to_bytes_impl(val);
}

auto to_bytes(float val) //
    -> z_owned_bytes_t
{
    return to_bytes_impl(val);
}
auto to_bytes(double val) //
    -> z_owned_bytes_t
{
    return to_bytes_impl(val);
}

auto to_bytes(bool val) //
    -> z_owned_bytes_t
{
    auto res = z_owned_bytes_t{};
    z_bytes_copy_from_str(&res, val ? "true" : "false");
    return res;
}

auto to_bytes(const char* val) //
    -> z_owned_bytes_t
{
    return to_bytes_impl(val);
}
auto to_bytes(std::string_view val) //
    -> z_owned_bytes_t
{
    return to_bytes_impl(val);
}

} // namespace impl
} // namespace flunder
