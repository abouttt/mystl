#pragma once

#include "type_traits.h"

namespace mystl
{

using size_t = decltype(sizeof(0));
using ptrdiff_t = decltype(static_cast<int*>(nullptr) - static_cast<int*>(nullptr));
using nullptr_t = decltype(nullptr);
using max_align_t = long double;

enum class byte : unsigned char {};

template <typename IntegerType> requires is_integral_v<IntegerType>
constexpr byte operator<<(byte b, IntegerType shift) noexcept
{
	return byte(static_cast<unsigned int>(b) << shift);
}

template <typename IntegerType> requires is_integral_v<IntegerType>
constexpr byte operator>>(byte b, IntegerType shift) noexcept
{
	return byte(static_cast<unsigned int>(b) >> shift);
}

constexpr byte operator|(byte l, byte r) noexcept
{
	return byte(static_cast<unsigned int>(l) | static_cast<unsigned int>(r));
}

constexpr byte operator&(byte l, byte r) noexcept
{
	return byte(static_cast<unsigned int>(l) & static_cast<unsigned int>(r));
}

constexpr byte operator^(byte l, byte r) noexcept
{
	return byte(static_cast<unsigned int>(l) ^ static_cast<unsigned int>(r));
}

constexpr byte operator~(byte b) noexcept
{
	return byte(~static_cast<unsigned int>(b));
}

template <typename IntegerType> requires is_integral_v<IntegerType>
constexpr byte& operator<<=(byte& b, IntegerType shift) noexcept
{
	return b = b << shift;
}

template <typename IntegerType> requires is_integral_v<IntegerType>
constexpr byte& operator>>=(byte& b, IntegerType shift) noexcept
{
	return b = b >> shift;
}

constexpr byte& operator|=(byte& l, byte r) noexcept
{
	return l = l | r;
}

constexpr byte& operator&=(byte& l, byte r) noexcept
{
	return l = l & r;
}

constexpr byte& operator^=(byte& l, byte r) noexcept
{
	return l = l ^ r;
}

template <typename IntegerType> requires is_integral_v<IntegerType>
constexpr IntegerType to_integer(byte b) noexcept
{
	return static_cast<IntegerType>(b);
}

} // namespace mystl
