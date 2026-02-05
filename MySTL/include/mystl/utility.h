#pragma once

#include "cstddef.h"
#include "type_traits.h"

namespace mystl
{

template <typename T>
constexpr remove_reference_t<T>&& move(T&& t) noexcept
{
	return static_cast<remove_reference_t<T>&&>(t);
}

template <typename T>
constexpr conditional_t<is_nothrow_move_constructible_v<T> || !is_copy_constructible_v<T>, const T&, T&&>
move_if_noexcept(T& x) noexcept
{
	return move(x);
}

template <typename T> requires is_move_constructible_v<T> && is_move_assignable_v<T>
constexpr void swap(T& a, T& b) noexcept(is_nothrow_constructible_v<T> && is_nothrow_assignable_v<T>)
{
	T tmp = move(a);
	a = move(b);
	b = move(tmp);
}

template <typename T2, size_t N> requires is_swappable_v<T2>
constexpr void swap(T2(&a)[N], T2(&b)[N]) noexcept(is_nothrow_swappable_v<T2>)
{
	if (&a == &b)
	{
		return;
	}

	for (size_t i = 0; i < N; ++i)
	{
		swap(a[i], b[i]);
	}
}

template <typename T>
constexpr T&& forward(remove_reference_t<T>& t) noexcept
{
	return static_cast<T&&>(t);
}

template <typename T>
constexpr T&& forward(remove_reference_t<T>&& t) noexcept
{
	static_assert(!is_lvalue_reference_v<T>, "cannot forward rvalue as lvalue");
	return static_cast<T&&>(t);
}

template <typename T, typename U = T>
constexpr T exchange(T& obj, U&& new_value) noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_assignable_v<T&, U>)
{
	T old_value = move(obj);
	obj = forward<U>(new_value);
	return old_value;
}

template <typename T>
constexpr add_const_t<T>& as_const(T& t) noexcept
{
	return t;
}

template <typename T>
void as_const(const T&&) = delete;

template <typename T>
add_rvalue_reference_t<T> declval() noexcept
{
	static_assert(false, "declval not allowed in an evaluated context");
}

} // namespace mystl
