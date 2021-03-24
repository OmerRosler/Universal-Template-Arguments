#ifndef UTA_H
#define UTA_H

#include <type_traits>

namespace utils
{
template<typename T>
constexpr T&& constexpr_declval() noexcept;
}
namespace uta
{

enum class basic_arg_type
{
    type,
    nttp
};

enum class arg_type 
{
    type,
    nttp,
    templ
};


template<typename T>
struct is_tag_impl;

template<typename T>
concept tag_type = is_tag_impl<T>::value;

template<tag_type Tag>
struct basic_arg;

template<typename T>
struct generic_arg_like_impl;

struct nttp_p 
{
    static constexpr auto tag_enum = basic_arg_type::nttp;
};
struct type_p 
{
    static constexpr auto tag_enum = basic_arg_type::type;
};

template<typename T>
concept parameter_tag = std::is_same_v<T, nttp_p> || std::is_same_v<T, type_p>;

template<parameter_tag... Params>
struct template_signature;

template<std::size_t NestingLevel, tag_type>
struct universal_arg;

template<typename T>
concept generic_arg_like = generic_arg_like_impl<T>::value;


template<typename T>
struct type_ 
{
    static constexpr auto enum_value = basic_arg_type::type;
};

template<auto V>
struct nttp_ 
{
    static constexpr auto enum_value = basic_arg_type::nttp;
};

template<template<generic_arg_like auto...> typename Templ>
struct template_
{
    static constexpr auto enum_value = arg_type::templ;
};


template<typename T>
struct is_tag_impl : std::false_type {};

template<typename T>
struct is_tag_impl<type_<T>> : std::true_type {};

template<auto V>
struct is_tag_impl<nttp_<V>> : std::true_type {};

template<template<generic_arg_like auto...> typename Templ>
struct is_tag_impl<template_<Templ>> : std::true_type {};

template<parameter_tag... Params>
struct is_tag_impl<template_signature<Params...>> : std::true_type {};

template<typename T>
struct generic_arg_like_impl : std::false_type {};

template<std::size_t NestingLevel, tag_type Tag>
struct generic_arg_like_impl<universal_arg<NestingLevel, Tag>> : std::true_type {};

template<tag_type Tag>
struct generic_arg_like_impl<basic_arg<Tag>> : std::true_type {};


/*******************************************************/


template<typename T>
struct basic_arg<type_<T>>
{
    constexpr basic_arg(type_<T>) noexcept {}

    using type = T;

    static constexpr auto tag_enum = basic_arg_type::type;
};

template<auto V>
struct basic_arg<nttp_<V>> : std::integral_constant<decltype(V), V>
{
    constexpr basic_arg(nttp_<V>) noexcept {}

    static constexpr auto tag_enum = basic_arg_type::nttp;

};

template<typename T>
basic_arg(type_<T>) -> basic_arg<type_<T>>;

template<auto V>
basic_arg(nttp_<V>) -> basic_arg<nttp_<V>>;

/*************************************************************/


template<basic_arg... args>
struct variadic_arg_list;

template<basic_arg first, basic_arg... args>
struct variadic_arg_list<first, args...>
{
    template<typename T, tag_type... Args> 
        requires (decltype(first)::tag_enum == basic_arg_type::type) && (sizeof...(Args) == sizeof...(args))
    constexpr variadic_arg_list(type_<T>, Args...) {}

    template<auto V, tag_type... Args> 
        requires (decltype(first)::tag_enum == basic_arg_type::nttp) && (sizeof...(Args) == sizeof...(args))
    constexpr variadic_arg_list(nttp_<V>, Args...) {}

};


template<>
struct variadic_arg_list<>
{
    constexpr variadic_arg_list() {}
};

template<tag_type... TaggedTypes> requires (sizeof...(TaggedTypes) != 0)
variadic_arg_list(TaggedTypes...) -> variadic_arg_list<basic_arg{TaggedTypes()}...>;

variadic_arg_list() -> variadic_arg_list<>;

/***********************************************************/

template<parameter_tag... Params>
struct template_signature {};

namespace 
{
template<basic_arg_type ParamTag>
constexpr auto transform_param_to_arg_tag()
{
    if constexpr (ParamTag == basic_arg_type::nttp)
    {
        return utils::constexpr_declval<nttp_p>();
    }
    else if (ParamTag == basic_arg_type::type)
    {
        return utils::constexpr_declval<type_p>();
    }
}
}

template<basic_arg_type ParamTag>
using transform_param_to_arg_tag_t = decltype(transform_param_to_arg_tag<ParamTag>());

// template<basic_arg... Values> requires (sizeof...(Values) != 0)
// template_signature(variadic_arg_list<Values...>) -> template_signature<transform_param_to_arg_tag_t<decltype(Values)::tag_enum>...>;


/*********************************************************/



template<tag_type>
struct up_to_level_1;

template<typename T>
struct up_to_level_1<type_<T>> : basic_arg<type_<T>> 
{
    using base_t = basic_arg<type_<T>>;
    using base_t::base_t;

    static constexpr auto tag_enum = arg_type::type;
};

template<auto V>
struct up_to_level_1<nttp_<V>> : basic_arg<nttp_<V>>
{
    using base_t = basic_arg<nttp_<V>>;
    using base_t::base_t;

    static constexpr auto tag_enum = arg_type::nttp;
};


template<template<basic_arg...> typename Templ>
struct up_to_level_1<template_<Templ>>
{
    constexpr up_to_level_1(template_<Templ>) {}

    template<basic_arg... args>
    struct apply
    {
        using type = Templ<args...>;
    };

    static constexpr auto tag_enum = arg_type::templ;
};


template<typename T>
up_to_level_1(type_<T>) -> up_to_level_1<type_<T>>;

template<auto V>
up_to_level_1(nttp_<V>) -> up_to_level_1<nttp_<V>>;


template<template<basic_arg...> typename Templ>
up_to_level_1(template_<Templ>) -> up_to_level_1<template_<Templ>>;

template<tag_type Tag>
struct level_2;

template<parameter_tag... Params>
struct level_2<template_signature<Params...>>
{
    constexpr level_2(template_signature<Params...>) {};

    template<template<generic_arg_like auto...> typename Templ>
    struct apply
    {
        //TODO: link discussion in [std-Discussion] regarding why we need this work-around
        //TODO: DO NOT REMOVE THE WRAPPER, THERE IS A CLANG BUG PREVENTING SIMPLE ALIAS WHEN `Templ` is non variadic alias template
        template<basic_arg... Values>
        struct type_wrapper {using type = Templ<Values...>; };

        template<basic_arg... Values> requires (sizeof...(Params) == sizeof...(Values)) &&
            ((decltype(Values)::tag_enum == Params::tag_enum) && ...)
        using templ = typename type_wrapper<Values...>::type;


    };
};

template<parameter_tag... Params>
level_2(template_signature<Params...>) -> level_2<template_signature<Params...>>;


} //namespace uta

#endif // UTA_H