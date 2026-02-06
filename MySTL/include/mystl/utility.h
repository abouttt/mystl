#pragma once

#include <compare>
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
	T old_value = move(obj);
	obj = forward<U>(new_value);
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

template <typename T1, typename T2>
struct pair
{
	using first_type = T1;
	using second_type = T2;

	constexpr pair() requires std::is_default_constructible_v<T1> && std::is_default_constructible_v<T2>
		: first()
		, second()
	{
	}

	constexpr explicit(!std::is_convertible_v<const T1&, T1> || !std::is_convertible_v<const T2&, T2>)
		pair(const T1& x, const T2& y) requires std::is_copy_constructible_v<T1> && std::is_copy_constructible_v<T2>
		: first(x)
		, second(y)
	{
	}

	template <typename U1, typename U2>
	constexpr explicit(!std::is_convertible_v<const U1&, T1> || !std::is_convertible_v<const U2&, T2>)
		pair(const pair<U1, U2>& p) requires std::is_constructible_v<T1, const U1&>&& std::is_constructible_v<T2, const U2&>
		: first(p.first)
		, second(p.second)
	{
	}

	template <typename U1, typename U2>
	constexpr explicit(!std::is_convertible_v<U1, T1> || !std::is_convertible_v<U2, T2>)
		pair(U1&& x, U2&& y) requires std::is_constructible_v<T1, U1> && std::is_constructible_v<T2, U2>
		: first(forward<U1>(x))
		, second(forward<U2>(y))
	{
	}

	template <typename U1, typename U2>
	constexpr explicit(!std::is_convertible_v<U1, T1> || !std::is_convertible_v<U2, T2>)
		pair(pair<U1, U2>&& p) requires std::is_constructible_v<T1, U1> && std::is_constructible_v<T2, U2>
		: first(forward<U1>(p.first))
		, second(forward<U2>(p.second))
	{
	}

	pair(const pair& p) = default;
	pair(pair&& p) = default;

	constexpr pair& operator=(const pair& other) requires std::is_copy_assignable_v<T1> && std::is_copy_assignable_v<T2>
	{
		first = other.first;
		second = other.second;
		return *this;
	}

	template <typename U1, typename U2>
		requires std::is_assignable_v<T1&, const U1&> && std::is_assignable_v<T2&, const U2&>
	constexpr pair& operator=(const pair<U1, U2>& other)
	{
		first = other.first;
		second = other.second;
		return *this;
	}

	constexpr pair& operator=(pair&& other)
		noexcept(std::is_nothrow_move_assignable_v<T1> && std::is_nothrow_move_assignable_v<T2>)
		requires std::is_move_assignable_v<T1> && std::is_move_assignable_v<T2>
	{
		first = move(other.first);
		second = move(other.second);
		return *this;
	}

	template <typename U1, typename U2>
		requires std::is_assignable_v<T1&, U1> && std::is_assignable_v<T2&, U2>
	constexpr pair& operator=(pair<U1, U2>&& p)
	{
		first = forward<U1>(p.first);
		second = forward<U2>(p.second);
		return *this;
	}

	constexpr void swap(pair& other)
		noexcept(std::is_nothrow_swappable_v<T1> && std::is_nothrow_swappable_v<T2>)
		requires std::is_swappable_v<T1> && std::is_swappable_v<T2>
	{
		swap(first, other.first);
		swap(second, other.second);
	}

	T1 first;
	T2 second;
};

template <typename T1, typename T2, typename U1, typename U2>
constexpr bool operator==(const pair<T1, T2>& lhs, const pair<U1, U2>& rhs)
{
	return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <typename T1, typename T2, typename U1, typename U2>
constexpr std::common_comparison_category_t<std::compare_three_way_result_t<T1, U1>, std::compare_three_way_result_t<T2, U2>>
operator<=>(const pair<T1, T2>& lhs, const pair<U1, U2>& rhs)
{
	if (auto c = lhs.first <=> rhs.first; c != 0)
	{
		return c;
	}

	return lhs.second <=> rhs.second;
}

template <typename T1, typename T2>
constexpr pair<std::unwrap_ref_decay_t<T1>, std::unwrap_ref_decay_t<T2>>
make_pair(T1&& x, T2&& y)
{
	return pair<std::unwrap_ref_decay_t<T1>, std::unwrap_ref_decay_t<T2>>(forward<T1>(x), forward<T2>(y));
}

template <typename T1, typename T2>
constexpr void swap(pair<T1, T2>& x, pair<T1, T2>& y) noexcept(noexcept(x.swap(y)))
{
	x.swap(y);
}

template <typename T>
struct tuple_size;

template <typename T1, typename T2>
struct tuple_size<pair<T1, T2>> : std::integral_constant<std::size_t, 2> {};

template <std::size_t I, typename T>
struct tuple_element;

template <std::size_t I, typename T1, typename T2>
struct tuple_element<I, pair<T1, T2>>
{
	static_assert(I < 2, "mystl::pair has only 2 elements!");
};

template <typename T1, typename T2>
struct tuple_element<0, pair<T1, T2>>
{
	using type = T1;
};

template <typename T1, typename T2>
struct tuple_element<1, pair<T1, T2>>
{
	using type = T2;
};

template <std::size_t I, typename T>
using tuple_element_t = typename tuple_element<I, T>::type;

template <std::size_t I, typename T1, typename T2>
constexpr tuple_element_t<I, pair<T1, T2>>& get(pair<T1, T2>& p) noexcept
{
	if constexpr (I == 0)
	{
		return p.first;
	}
	else
	{
		return p.second;
	}
}

template <std::size_t I, typename T1, typename T2>
constexpr const tuple_element_t<I, pair<T1, T2>>& get(const pair<T1, T2>& p) noexcept
{
	if constexpr (I == 0)
	{
		return p.first;
	}
	else
	{
		return p.second;
	}
}

template <std::size_t I, typename T1, typename T2>
constexpr tuple_element_t<I, pair<T1, T2>>&& get(pair<T1, T2>&& p) noexcept
{
	if constexpr (I == 0)
	{
		return move(p.first);
	}
	else
	{
		return move(p.second);
	}
}

template <std::size_t I, typename T1, typename T2>
constexpr const tuple_element_t<I, pair<T1, T2> >&& get(const pair<T1, T2>&& p) noexcept
{
	if constexpr (I == 0)
	{
		return move(p.first);
	}
	else
	{
		return move(p.second);
	}
}

template <typename T, typename U>
	requires (!std::is_same_v<T, U>)
constexpr T& get(pair<T, U>& p) noexcept
{
	return p.first;
}

template <typename T, typename U>
	requires (!std::is_same_v<T, U>)
constexpr const T& get(const pair<T, U>& p) noexcept
{
	return p.first;
}

template <typename T, typename U>
	requires (!std::is_same_v<T, U>)
constexpr T&& get(pair<T, U>&& p) noexcept
{
	return move(p.first);
}

template <typename T, typename U>
	requires (!std::is_same_v<T, U>)
constexpr const T&& get(const pair<T, U>&& p) noexcept
{
	return move(p.first);
}

template <typename T, typename U>
	requires (!std::is_same_v<T, U>)
constexpr T& get(pair<U, T>& p) noexcept
{
	return p.second;
}

template <typename T, typename U>
	requires (!std::is_same_v<T, U>)
constexpr const T& get(const pair<U, T>& p) noexcept
{
	return p.second;
}

template <typename T, typename U>
	requires (!std::is_same_v<T, U>)
constexpr T&& get(pair<U, T>&& p) noexcept
{
	return move(p.second);
}

template <typename T, typename U>
	requires (!std::is_same_v<T, U>)
constexpr const T&& get(const pair<U, T>&& p) noexcept
{
	return move(p.second);
}

} // namespace mystl
