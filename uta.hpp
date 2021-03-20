
#include <type_traits>

namespace uta
{
template<typename T>
constexpr T&& declval();

enum class basic_arg_type
{
    type,
    nttp
};


template<typename T>
struct type_tag 
{
    static constexpr auto enum_value = basic_arg_type::type;
};

template<auto V>
struct nttp_tag 
{
    static constexpr auto enum_value = basic_arg_type::nttp;
};

template<typename T>
struct is_tag_impl : std::false_type {};

template<typename T>
struct is_tag_impl<type_tag<T>> : std::true_type {};

template<auto V>
struct is_tag_impl<nttp_tag<V>> : std::true_type {};

template<typename T>
concept tag_type = is_tag_impl<T>::value;
/*******************************************************/

enum class arg_type 
{
    type,
    nttp,
    variadic,
    tmpl
};

template<tag_type Tag>
struct basic_arg;

template<typename T>
struct basic_arg<type_tag<T>>
{
    constexpr basic_arg(type_tag<T>) noexcept {}

    using type = T;

    static constexpr auto tag_enum = basic_arg_type::type;
};

template<auto V>
struct basic_arg<nttp_tag<V>>
{
    constexpr basic_arg(nttp_tag<V>) noexcept {}

    static constexpr auto tag_enum = basic_arg_type::nttp;

    static constexpr auto value = V;
};

template<typename T>
basic_arg(type_tag<T>) -> basic_arg<type_tag<T>>;

template<auto V>
basic_arg(nttp_tag<V>) -> basic_arg<nttp_tag<V>>;

/*************************************************************/


template<basic_arg... args>
struct variadic_arg_list;

template<basic_arg first, basic_arg... args>
struct variadic_arg_list<first, args...>
{
    template<typename T, tag_type... Args> 
        requires (decltype(first)::tag_enum == basic_arg_type::type) && (sizeof...(Args) == sizeof...(args))
    constexpr variadic_arg_list(type_tag<T>, Args...) {}

    template<auto V, tag_type... Args> 
        requires (decltype(first)::tag_enum == basic_arg_type::nttp) && (sizeof...(Args) == sizeof...(args))
    constexpr variadic_arg_list(nttp_tag<V>, Args...) {}

    static constexpr auto tag_enum = arg_type::variadic;
};


template<>
struct variadic_arg_list<>
{
    constexpr variadic_arg_list() {}
};

template<tag_type... TaggedTypes> requires (sizeof...(TaggedTypes) != 0)
variadic_arg_list(TaggedTypes...) -> variadic_arg_list<basic_arg{TaggedTypes()}...>;

variadic_arg_list() -> variadic_arg_list<>;

} //namespace uta