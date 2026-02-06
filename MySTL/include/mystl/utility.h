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
	return mystl::move(x);
}

template <typename T>
constexpr void swap(T& a, T& b)
{
	T tmp = mystl::move(a);
	a = mystl::move(b);
	b = mystl::move(tmp);
}

template <typename T2, std::size_t N>
constexpr void swap(T2(&a)[N], T2(&b)[N])
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
constexpr T exchange(T& obj, U&& new_value)
{
	T old_value = mystl::move(obj);
	obj = mystl::forward<U>(new_value);
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

	constexpr pair()
		: first()
		, second()
	{
	}

	constexpr pair(const T1& x, const T2& y)
		: first(x)
		, second(y)
	{
	}

	template <typename U1, typename U2>
	constexpr pair(U1&& x, U2&& y)
		: first(mystl::forward<U1>(x))
		, second(mystl::forward<U2>(y))
	{
	}

	template <typename U1, typename U2>
	constexpr pair(const pair<U1, U2>& p)
		: first(p.first)
		, second(p.second)
	{
	}

	template <typename U1, typename U2>
	constexpr pair(pair<U1, U2>&& p)
		: first(mystl::forward<U1>(p.first))
		, second(mystl::forward<U2>(p.second))
	{
	}

	pair(const pair& p) = default;
	pair(pair&& p) = default;

	constexpr pair& operator=(const pair& other)
	{
		first = other.first;
		second = other.second;
		return *this;
	}

	constexpr pair& operator=(pair&& other) noexcept
	{
		first = mystl::move(other.first);
		second = mystl::move(other.second);
		return *this;
	}

	template <typename U1, typename U2>
	constexpr pair& operator=(const pair<U1, U2>& other)
	{
		first = other.first;
		second = other.second;
		return *this;
	}

	template <typename U1, typename U2>
	constexpr pair& operator=(pair<U1, U2>&& p)
	{
		first = mystl::forward<U1>(p.first);
		second = mystl::forward<U2>(p.second);
		return *this;
	}

	constexpr void swap(pair& other) noexcept
	{
		mystl::swap(first, other.first);
		mystl::swap(second, other.second);
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
	return pair<std::unwrap_ref_decay_t<T1>, std::unwrap_ref_decay_t<T2>>(mystl::forward<T1>(x), mystl::forward<T2>(y));
}

template <typename T1, typename T2>
constexpr void swap(pair<T1, T2>& x, pair<T1, T2>& y) noexcept
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
		return mystl::move(p.first);
	}
	else
	{
		return mystl::move(p.second);
	}
}

template <std::size_t I, typename T1, typename T2>
constexpr const tuple_element_t<I, pair<T1, T2> >&& get(const pair<T1, T2>&& p) noexcept
{
	if constexpr (I == 0)
	{
		return mystl::move(p.first);
	}
	else
	{
		return mystl::move(p.second);
	}
}

template <typename T, typename U>
constexpr T& get(pair<T, U>& p) noexcept
{
	return p.first;
}

template <typename T, typename U>
constexpr const T& get(const pair<T, U>& p) noexcept
{
	return p.first;
}

template <typename T, typename U>
constexpr T&& get(pair<T, U>&& p) noexcept
{
	return mystl::move(p.first);
}

template <typename T, typename U>
constexpr const T&& get(const pair<T, U>&& p) noexcept
{
	return mystl::move(p.first);
}

template <typename T, typename U>
constexpr T& get(pair<U, T>& p) noexcept
{
	return p.second;
}

template <typename T, typename U>
constexpr const T& get(const pair<U, T>& p) noexcept
{
	return p.second;
}

template <typename T, typename U>
constexpr T&& get(pair<U, T>&& p) noexcept
{
	return mystl::move(p.second);
}

template <typename T, typename U>
constexpr const T&& get(const pair<U, T>&& p) noexcept
{
	return mystl::move(p.second);
}

} // namespace mystl
