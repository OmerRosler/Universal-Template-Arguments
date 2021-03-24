#ifndef UTA_H
#define UTA_H
#include <type_traits>

namespace uta
{
enum class arg_type 
{
    type,
    nttp,
    templ
};

namespace
{
template<typename T>
struct tag {};


/* Forward Declarations */


template<typename T>
struct is_tag_impl : std::false_type {};

template<typename T>
struct is_tag_impl<tag<T>> : std::true_type {};

template<typename T>
concept tag_type = is_tag_impl<T>::value;
}

template<typename T>
struct template_argument_impl;

/* Public template paramters API types */
struct nttp_p
{
    static constexpr auto tag_enum = arg_type::nttp;
};
struct type_p 
{
    static constexpr auto tag_enum = arg_type::type;
};

/* Concepts Definitions */

//TODO: Extend parameters to include template parameters
template<typename T>
concept template_parameter = std::is_same_v<T, nttp_p> || std::is_same_v<T, type_p>;

template<typename T>
concept template_argument = template_argument_impl<T>::value;


/* Public template arguments API types */

template<template_parameter... Params>
struct template_parameter_accepting;

template<typename T>
struct type_ 
{
    static constexpr auto enum_value = arg_type::type;
};

template<auto V>
struct nttp_ 
{
    static constexpr auto enum_value = arg_type::nttp;
};

template<template<template_argument auto...> typename Templ>
struct template_
{
    static constexpr auto enum_value = arg_type::templ;
};


/* Concepts implementation */

template<tag_type>
struct universal_argument;





/*******************************************************/

template<typename T>
struct template_argument_impl : std::false_type {};

template<tag_type Tag>
struct level_0;

template<tag_type Tag>
struct level_1;

template<tag_type Tag>
struct template_argument_impl<level_0<Tag>> : std::true_type {};

template<tag_type Tag>
struct template_argument_impl<level_1<Tag>> : std::true_type {};

template<tag_type Tag>
struct template_argument_impl<universal_argument<Tag>> : std::true_type {};

template<template_parameter... Params>
struct template_parameter_accepting {};



/* template argument API types */


template<typename T>
struct level_0<tag<type_<T>>>
{
    constexpr level_0(type_<T>) noexcept {}

    using type = T;

    static constexpr auto tag_enum = arg_type::type;
};

template<auto V>
struct level_0<tag<nttp_<V>>> : std::integral_constant<decltype(V), V>
{
    constexpr level_0(nttp_<V>) noexcept {}

    static constexpr auto tag_enum = arg_type::nttp;

};

template<typename T>
level_0(type_<T>) -> level_0<tag<type_<T>>>;

template<auto V>
level_0(nttp_<V>) -> level_0<tag<nttp_<V>>>;

/*************************************************************/


template<tag_type>
struct level_1;

template<template<level_0...> typename Templ>
struct level_1<tag<template_<Templ>>>
{
    constexpr level_1(template_<Templ>) {}

    /* DO NOT REMOVE THE WRAPPER
     * There are clang and MSVC bugs when `Templ` is a non variadic alias template
     * See https://lists.isocpp.org/std-discussion/2021/03/1099.php for the details
    */
    template<level_0... args>
    struct apply
    {
        using type = Templ<args...>;
    };

    static constexpr auto tag_enum = arg_type::templ;
};


template<template<level_0...> typename Templ>
level_1(template_<Templ>) -> level_1<tag<template_<Templ>>>;


/*************************************************************/

template<tag_type Tag>
struct level_2;

template<template_parameter... Params>
struct level_2<tag<template_parameter_accepting<Params...>>>
{
    constexpr level_2(template_parameter_accepting<Params...>) {};

    template<template<template_argument auto...> typename Templ>
    struct apply
    {
        /* DO NOT REMOVE THE WRAPPER
         * There are clang and MSVC bugs when `Templ` is a non variadic alias template
         * See https://lists.isocpp.org/std-discussion/2021/03/1099.php for the details
        */
        template<level_0... Values>
        struct type_wrapper 
        {
            using type = Templ<Values...>; 
        };

        template<level_0... Values> requires (sizeof...(Params) == sizeof...(Values)) &&
            ((decltype(Values)::tag_enum == Params::tag_enum) && ...)
        using templ = typename type_wrapper<Values...>::type;
    };
};

template<template_parameter... Params>
level_2(template_parameter_accepting<Params...>) ->
    level_2<tag<template_parameter_accepting<Params...>>>;


/*************************************************************/

template<tag_type>
struct universal_argument;

template<typename T>
struct universal_argument<tag<type_<T>>> : level_0<tag<type_<T>>> 
{
    using base_t = level_0<tag<type_<T>>>;
    using base_t::base_t;

    static constexpr auto tag_enum = arg_type::type;
};

template<auto V>
struct universal_argument<tag<nttp_<V>>> : level_0<tag<nttp_<V>>>
{
    using base_t = level_0<tag<nttp_<V>>>;
    using base_t::base_t;

    static constexpr auto tag_enum = arg_type::nttp;
};

template<template<level_0...> typename Templ>
struct universal_argument<tag<template_<Templ>>> : level_1<tag<template_<Templ>>>
{
    using base_t = level_1<tag<template_<Templ>>>;
    using base_t::base_t;
};

template<typename T>
universal_argument(type_<T>) -> universal_argument<tag<type_<T>>>;

template<auto V>
universal_argument(nttp_<V>) -> universal_argument<tag<nttp_<V>>>;

template<template<level_0...> typename Templ>
universal_argument(template_<Templ>) -> universal_argument<tag<template_<Templ>>>;


} //namespace uta

#endif // UTA_H