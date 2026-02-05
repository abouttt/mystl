#pragma once

namespace mystl
{

template <typename T, T v>
struct integral_constant
{
	static constexpr T value = v;

	using value_type = T;
	using type = integral_constant;

	constexpr operator value_type() const noexcept
	{
		return value;
	}

	constexpr value_type operator()() const noexcept
	{
		return value;
	}
};

template <bool B>
using bool_constant = integral_constant<bool, B>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

template <bool B, typename T = void>
struct enable_if {};

template <typename T>
struct enable_if<true, T>
{
	using type = T;
};

template <bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;

template <bool B, typename T, typename F>
struct conditional
{
	using type = T;
};

template <typename T, typename F>
struct conditional<false, T, F>
{
	using type = F;
};

template <bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;

template <typename...>
using void_t = void;

template <typename T>
struct type_identity
{
	using type = T;
};

template <typename T>
using type_identity_t = type_identity<T>::type;

template <typename... B>
struct conjunction : true_type {};

template <typename B1>
struct conjunction<B1> : B1 {};

template <typename B1, typename... Bn>
struct conjunction<B1, Bn...> : conditional_t<bool(B1::value), conjunction<Bn...>, B1> {};

template <typename... B>
constexpr bool conjunction_v = conjunction<B...>::value;

template <typename... B>
struct disjunction : false_type {};

template <typename B1>
struct disjunction<B1> : B1 {};

template <typename B1, typename... Bn>
struct disjunction<B1, Bn...> : conditional_t<bool(B1::value), B1, disjunction<Bn...>> {};

template <typename... B>
constexpr bool disjunction_v = disjunction<B...>::value;

template <typename B>
struct negation : bool_constant<!bool(B::value)> {};

template <typename B>
constexpr bool negation_v = negation<B>::value;

template <typename T>
struct remove_const
{
	using type = T;
};

template <typename T>
struct remove_const<const T>
{
	using type = T;
};

template <typename T>
using remove_const_t = typename remove_const<T>::type;

template <typename T>
struct remove_volatile
{
	using type = T;
};

template <typename T>
struct remove_volatile<volatile T>
{
	using type = T;
};

template <typename T>
using remove_volatile_t = typename remove_volatile<T>::type;

template <typename T>
struct remove_cv
{
	using type = T;
};

template <typename T>
struct remove_cv<const T>
{
	using type = T;
};

template <typename T>
struct remove_cv<volatile T>
{
	using type = T;
};

template <typename T>
struct remove_cv<const volatile T>
{
	using type = T;
};

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

template <typename T>
struct remove_reference
{
	using type = T;
};

template <typename T>
struct remove_reference<T&>
{
	using type = T;
};

template <typename T>
struct remove_reference<T&&>
{
	using type = T;
};

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

template <typename T>
struct remove_pointer
{
	using type = T;
};

template <typename T>
struct remove_pointer<T*>
{
	using type = T;
};

template <typename T>
struct remove_pointer<T* const>
{
	using type = T;
};

template <typename T>
struct remove_pointer<T* volatile>
{
	using type = T;
};

template <typename T>
struct remove_pointer<T* const volatile>
{
	using type = T;
};

template <typename T>
using remove_pointer_t = typename remove_pointer<T>::type;

template <typename T>
struct remove_extent
{
	using type = T;
};

template <typename T>
struct remove_extent<T[]>
{
	using type = T;
};

template <typename T, decltype(sizeof(0)) N>
struct remove_extent<T[N]>
{
	using type = T;
};

template <typename T>
using remove_extent_t = typename remove_extent<T>::type;

template <typename T>
struct remove_all_extents
{
	using type = T;
};

template <typename T>
struct remove_all_extents<T[]>
{
	using type = typename remove_all_extents<T>::type;
};

template <typename T, decltype(sizeof(0)) N>
struct remove_all_extents<T[N]>
{
	using type = typename remove_all_extents<T>::type;
};

template <typename T>
using remove_all_extents_t = typename remove_all_extents<T>::type;

template <typename T>
struct add_const
{
	using type = const T;
};

template <typename T>
using add_const_t = typename add_const<T>::type;

template <typename T>
struct add_volatile
{
	using type = volatile T;
};

template <typename T>
using add_volatile_t = typename add_volatile<T>::type;

template <typename T>
struct add_cv
{
	using type = const volatile T;
};

template <typename T>
using add_cv_t = typename add_cv<T>::type;

namespace detail
{

template <typename T>
auto try_add_lvalue_reference(int) -> type_identity<T&> {};

template <typename T>
auto try_add_lvalue_reference(...) -> type_identity<T> {};

template <typename T>
auto try_add_rvalue_reference(int) -> type_identity<T&&> {};

template <typename T>
auto try_add_rvalue_reference(...) -> type_identity<T> {};

} // namespace detail

template <typename T>
struct add_lvalue_reference : decltype(detail::try_add_lvalue_reference<T>(0)) {};

template <typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

template <typename T>
struct add_rvalue_reference : decltype(detail::try_add_rvalue_reference<T>(0)) {};

template <typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

namespace detail
{

template <typename T>
auto try_add_pointer(int) -> type_identity<remove_reference_t<T>*> {};

template <typename T>
auto try_add_pointer(...) -> type_identity<T> {};

} // namespace detail

template <typename T>
struct add_pointer : decltype(detail::try_add_pointer<T>(0)) {};

template <typename T>
using add_pointer_t = typename add_pointer<T>::type;

template <typename T, typename U>
struct is_same : false_type {};

template <typename T>
struct is_same<T, T> : true_type {};

template <typename T, typename U>
constexpr bool is_same_v = is_same<T, U>::value;

template <typename T>
struct is_const : false_type {};

template <typename T>
struct is_const<const T> : true_type {};

template <typename T>
constexpr bool is_const_v = is_const<T>::value;

template <typename T>
struct is_reference : false_type {};

template <typename T>
struct is_reference<T&> : true_type {};

template <typename T>
struct is_reference<T&&> : true_type {};

template <typename T>
constexpr bool is_reference_v = is_reference<T>::value;

template <typename T>
struct is_lvalue_reference : false_type {};

template <typename T>
struct is_lvalue_reference<T&> : true_type {};

template <typename T>
constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

template <typename T>
struct is_rvalue_reference : false_type {};

template <typename T>
struct is_rvalue_reference<T&&> : true_type {};

template <typename T>
constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

template <typename T>
struct is_pointer : false_type {};

template <typename T>
struct is_pointer<T*> : true_type {};

template <typename T>
struct is_pointer<T* const> : true_type {};

template <typename T>
struct is_pointer<T* volatile> : true_type {};

template <typename T>
struct is_pointer<T* const volatile> : true_type {};

template <typename T>
constexpr bool is_pointer_v = is_pointer<T>::value;

template <typename T>
struct is_null_pointer : is_same<decltype(nullptr), remove_cv_t<T>> {};

template <typename T>
constexpr bool is_null_pointer_v = is_null_pointer<T>::value;

template <typename T>
struct is_void : is_same<void, typename remove_cv<T>::type> {};

template <typename T>
constexpr bool is_void_v = is_void<T>::value;

namespace detail
{

template <typename T, typename... Types>
constexpr bool is_any_of_v = (is_same_v<T, Types> || ...);

} // namespace detail

template <typename T>
struct is_integral : bool_constant<detail::is_any_of_v<remove_cv_t<T>, bool, char, signed char, unsigned char, wchar_t,
#if defined(__cpp_char8_t)
	char8_t,
#endif
	char16_t, char32_t, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>> {};

template <typename T>
constexpr bool is_integral_v = is_integral<T>::value;

template <typename T>
struct is_floating_point : bool_constant<detail::is_any_of_v<remove_cv_t<T>, float, double, long double>> {};

template <typename T>
constexpr bool is_floating_point_v = is_floating_point<T>::value;

template <typename T>
struct is_arithmetic : integral_constant<bool, is_integral<T>::value || is_floating_point<T>::value> {};

template <typename T>
constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

namespace detail
{

template <typename T, bool = is_arithmetic<T>::value>
struct is_signed : integral_constant<bool, T(-1) < T(0)> {};

template <typename T>
struct is_signed<T, false> : false_type {};

template <typename T, bool = is_arithmetic<T>::value>
struct is_unsigned : integral_constant<bool, T(0) < T(-1)> {};

template <typename T>
struct is_unsigned<T, false> : false_type {};

} // namespace detail

template <typename T>
struct is_signed : detail::is_signed<T>::type {};

template <typename T>
constexpr bool is_signed_v = is_signed<T>::value;

template <typename T>
struct is_unsigned : detail::is_unsigned<T>::type {};

template <typename T>
constexpr bool is_unsigned_v = is_unsigned<T>::value;

template <typename T>
struct is_array : false_type {};

template <typename T>
struct is_array<T[]> : true_type {};

template <typename T, decltype(sizeof(0)) N>
struct is_array<T[N]> : true_type {};

template <typename T>
constexpr bool is_array_v = is_array<T>::value;

template <typename T>
struct is_function : bool_constant<!is_const_v<const T> && !is_reference_v<T>> {};

template <typename T>
constexpr bool is_function_v = is_function<T>::value;

template <typename T>
struct decay
{
private:
	using U = remove_reference_t<T>;

public:
	using type = conditional_t<is_array_v<U>, remove_extent_t<U>*, conditional_t<is_function_v<U>, add_pointer_t<U>, remove_cv_t<U>>>;
};

template <typename T>
using decay_t = typename decay<T>::type;

} // namespace mystl
