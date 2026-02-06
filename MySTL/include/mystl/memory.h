#pragma once

#include <type_traits>

#include "utility.h"
#include "detail/iterator_base.h"

namespace mystl
{

template <typename T>
constexpr T* addressof(T& arg) noexcept
{
	return __builtin_addressof(arg);
}

template <typename T>
const T* addressof(const T&&) = delete;

template <typename Ptr>
struct pointer_traits
{
	using pointer = Ptr;
	using element_type = typename Ptr::element_type;
	using difference_type = std::ptrdiff_t;

	template <typename U>
	using rebind = typename Ptr::template rebind<U>;

	static pointer pointer_to(element_type& r)
	{
		return Ptr::pointer_to(r);
	}
};

template <typename T>
struct pointer_traits<T*>
{
	using pointer = T*;
	using element_type = T;
	using difference_type = std::ptrdiff_t;

	template <typename U>
	using rebind = U*;

	static constexpr pointer pointer_to(element_type& r) noexcept
	{
		return mystl::addressof(r);
	}
};

template <typename T>
constexpr T* to_address(T* p) noexcept
{
	static_assert(!std::is_function_v<T>);
	return p;
}

template <typename T>
constexpr auto to_address(const T& p) noexcept
{
	if constexpr (requires{ pointer_traits<T>::to_address(p); })
	{
		return pointer_traits<T>::to_address(p);
	}
	else
	{
		return to_address(p.operator->());
	}
}

template <typename T, typename... Args>
constexpr T* construct_at(T* location, Args&&... args)
{
	return ::new (static_cast<void*>(location)) T(mystl::forward<Args>(args)...);
}

template <typename T>
constexpr void destroy_at(T* p)
{
	if constexpr (std::is_array_v<T>)
	{
		for (auto& elem : *p)
		{
			(destroy_at)(mystl::addressof(elem));
		}
	}
	else
	{
		p->~T();
	}
}

template <typename ForwardIt>
constexpr void destroy(ForwardIt first, ForwardIt last)
{
	for (; first != last; ++first)
	{
		mystl::destroy_at(mystl::addressof(*first));
	}
}

template <typename ForwardIt, typename Size>
constexpr ForwardIt destroy_n(ForwardIt first, Size n)
{
	for (; n > 0; (void) ++first, --n)
	{
		mystl::destroy_at(mystl::addressof(*first));
	}
	return first;
}

namespace detail
{

template <typename NoThrowForwardIt>
struct uninitialized_backout
{
	~uninitialized_backout()
	{
		for (; first != current; ++first)
		{
			mystl::destroy_at(mystl::addressof(*first));
		}
	}

	void release() noexcept
	{
		first = current;
	}

	NoThrowForwardIt first;
	NoThrowForwardIt current;
};

} // namespace detail

template <typename InputIt, typename NoThrowForwardIt>
NoThrowForwardIt uninitialized_copy(InputIt first, InputIt last, NoThrowForwardIt d_first)
{
	using T = typename mystl::iterator_traits<NoThrowForwardIt>::value_type;
	detail::uninitialized_backout<NoThrowForwardIt> backout{ d_first, d_first };
	for (; first != last; ++first, (void)++backout.current)
	{
		::new (static_cast<void*>(mystl::addressof(*backout.current))) T(*first);
	}
	backout.release();
	return backout.current;
}

template <typename InputIt, typename Size, typename NoThrowForwardIt>
NoThrowForwardIt uninitialized_copy_n(InputIt first, Size count, NoThrowForwardIt d_first)
{
	using T = typename mystl::iterator_traits<NoThrowForwardIt>::value_type;
	detail::uninitialized_backout<NoThrowForwardIt> backout{ d_first, d_first };
	for (; count > 0; ++first, (void)++backout.current, --count)
	{
		::new (static_cast<void*>(mystl::addressof(*backout.current))) T(*first);
	}
	backout.release();
	return backout.current;
}

template <typename NoThrowForwardIt, typename T>
void uninitialized_fill(NoThrowForwardIt first, NoThrowForwardIt last, const T& value)
{
	using V = typename mystl::iterator_traits<NoThrowForwardIt>::value_type;
	detail::uninitialized_backout<NoThrowForwardIt> backout{ first, first };
	for (; backout.current != last; ++backout.current)
	{
		::new (static_cast<void*>(mystl::addressof(*backout.current))) V(value);
	}
	backout.release();
}

template <typename NoThrowForwardIt, typename Size, typename T>
NoThrowForwardIt uninitialized_fill_n(NoThrowForwardIt first, Size count, const T& value)
{
	using V = typename mystl::iterator_traits<NoThrowForwardIt>::value_type;
	detail::uninitialized_backout<NoThrowForwardIt> backout{ first, first };
	for (; count > 0; ++backout.current, (void) --count)
	{
		::new (static_cast<void*>(mystl::addressof(*backout.current))) V(value);
	}
	backout.release();
	return backout.current;
}

template <typename InputIt, typename NoThrowForwardIt>
NoThrowForwardIt uninitialized_move(InputIt first, InputIt last, NoThrowForwardIt d_first)
{
	using ValueType = typename mystl::iterator_traits<NoThrowForwardIt>::value_type;
	detail::uninitialized_backout<NoThrowForwardIt> backout{ d_first, d_first };
	for (; first != last; ++first, (void) ++backout.current)
	{
		auto addr = static_cast<void*>(mystl::addressof(*backout.current));
		if constexpr (std::is_lvalue_reference_v<decltype(*first)>)
		{
			::new (addr) ValueType(mystl::move(*first));
		}
		else
		{
			::new (addr) ValueType(*first);
		}
	}
	backout.release();
	return backout.current;
}

template <typename InputIt, typename Size, typename NoThrowForwardIt>
pair<InputIt, NoThrowForwardIt> uninitialized_move_n(InputIt first, Size count, NoThrowForwardIt d_first)
{
	using ValueType = typename mystl::iterator_traits<NoThrowForwardIt>::value_type;
	detail::uninitialized_backout<NoThrowForwardIt> backout{ d_first, d_first };
	for (; count > 0; ++first, (void) ++backout.current, --count)
	{
		auto addr = static_cast<void*>(mystl::addressof(*backout.current));
		if constexpr (std::is_lvalue_reference_v<decltype(*first)>)
		{
			::new (addr) ValueType(mystl::move(*first));
		}
		else
		{
			::new (addr) ValueType(*first);
		}
	}
	backout.release();
	return { first, backout.current };
}

template <typename NoThrowForwardIt>
void uninitialized_default_construct(NoThrowForwardIt first, NoThrowForwardIt last)
{
	using Value = typename mystl::iterator_traits<NoThrowForwardIt>::value_type;
	detail::uninitialized_backout<NoThrowForwardIt> backout{ first, first };
	for (; backout.current != last; ++backout.current)
	{
		::new (static_cast<void*>(mystl::addressof(*backout.current))) Value;
	}
	backout.release();
}

template <typename NoThrowForwardIt, typename Size>
NoThrowForwardIt uninitialized_default_construct_n(NoThrowForwardIt first, Size count)
{
	using Value = typename mystl::iterator_traits<NoThrowForwardIt>::value_type;
	detail::uninitialized_backout<NoThrowForwardIt> backout{ first, first };
	for (; count > 0; (void) ++backout.current, --count)
	{
		::new (static_cast<void*>(mystl::addressof(*backout.current))) Value;
	}
	backout.release();
	return backout.current;
}

template <typename NoThrowForwardIt>
void uninitialized_value_construct(NoThrowForwardIt first, NoThrowForwardIt last)
{
	using Value = typename mystl::iterator_traits<NoThrowForwardIt>::value_type;
	detail::uninitialized_backout<NoThrowForwardIt> backout{ first, first };
	for (; backout.current != last; ++backout.current)
	{
		::new (static_cast<void*>(mystl::addressof(*backout.current))) Value();
	}
	backout.release();
}

template <typename NoThrowForwardIt, typename Size>
NoThrowForwardIt uninitialized_value_construct_n(NoThrowForwardIt first, Size count)
{
	using Value = typename mystl::iterator_traits<NoThrowForwardIt>::value_type;
	detail::uninitialized_backout<NoThrowForwardIt> backout{ first, first };
	for (; count > 0; --count, (void)++backout.current)
	{
		::new (static_cast<void*>(mystl::addressof(*backout.current))) Value();
	}
	backout.release();
	return backout.current;
}

} // namespace mystl
