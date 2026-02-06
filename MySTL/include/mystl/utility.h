#pragma once

#include <type_traits>

namespace mystl
{

template <typename T>
constexpr std::remove_reference_t<T>&& move(T&& t) noexcept
{
	return static_cast<std::remove_reference_t<T>&&>(t);
}

template <typename T>
constexpr std::conditional_t<std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>, const T&, T&&>
move_if_noexcept(T& x) noexcept
{
	return move(x);
}

template <typename T>
	requires std::is_move_constructible_v<T> && std::is_move_assignable_v<T>
constexpr void swap(T& a, T& b) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>)
{
	T tmp = move(a);
	a = move(b);
	b = move(tmp);
}

template <typename T2, std::size_t N>
	requires std::is_swappable_v<T2>
constexpr void swap(T2(&a)[N], T2(&b)[N]) noexcept(std::is_nothrow_swappable_v<T2>)
{
	if (&a == &b)
	{
		return;
	}

	for (std::size_t i = 0; i < N; ++i)
	{
		swap(a[i], b[i]);
	}
}

template <typename T>
constexpr T&& forward(std::remove_reference_t<T>& t) noexcept
{
	return static_cast<T&&>(t);
}

template <typename T>
constexpr T&& forward(std::remove_reference_t<T>&& t) noexcept
{
	static_assert(!std::is_lvalue_reference_v<T>, "cannot forward rvalue as lvalue");
	return static_cast<T&&>(t);
}

template <typename T, typename U = T>
constexpr T exchange(T& obj, U&& new_value) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_assignable_v<T&, U>)
{
	T old_value = static_cast<T&&>(obj);
	obj = static_cast<U&&>(new_value);
	return old_value;
}

template <typename T>
constexpr std::add_const_t<T>& as_const(T& t) noexcept
{
	return t;
}

template <typename T>
void as_const(const T&&) = delete;

template <typename T>
std::add_rvalue_reference_t<T> declval() noexcept
{
	static_assert(false, "declval not allowed in an evaluated context");
}

} // namespace mystl
