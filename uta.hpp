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
struct template_signature
{
    template<basic_arg... Values> requires (sizeof...(Params) == sizeof...(Values)) &&
        ((decltype(Values)::tag_enum == Params::tag_enum) && ...)
    constexpr template_signature(variadic_arg_list<Values...>) {

    }
};

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

template<basic_arg... Values> requires (sizeof...(Values) != 0)
template_signature(variadic_arg_list<Values...>) -> template_signature<transform_param_to_arg_tag_t<decltype(Values)::tag_enum>...>;


/*********************************************************/



template<std::size_t NestingLevel, tag_type>
struct universal_arg;

template<typename T>
struct universal_arg<0, type_<T>> : basic_arg<type_<T>> 
{
    using base_t = basic_arg<type_<T>>;
    using base_t::base_t;

    static constexpr auto tag_enum = arg_type::type;
};

template<auto V>
struct universal_arg<0, nttp_<V>> : basic_arg<nttp_<V>>
{
    using base_t = basic_arg<nttp_<V>>;
    using base_t::base_t;

    static constexpr auto tag_enum = arg_type::nttp;
};


template<template<basic_arg...> typename Templ>
struct universal_arg<1, template_<Templ>>
{
    constexpr universal_arg(template_<Templ>) {}

    template<basic_arg... args>
    struct apply
    {
        using type = Templ<args...>;
    };

    static constexpr auto tag_enum = arg_type::templ;
};


template<typename T>
universal_arg(type_<T>) -> universal_arg<0, type_<T>>;

template<auto V>
universal_arg(nttp_<V>) -> universal_arg<0, nttp_<V>>;


template<template<basic_arg...> typename Templ>
universal_arg(template_<Templ>) -> universal_arg<1, template_<Templ>>;


template<std::size_t NestingLevel, generic_arg_like auto... Args>
struct wrap_universal
{
    template<template<generic_arg_like auto...> typename Templ>
    using templ = universal_arg<NestingLevel, type_<Templ<Args...>>>;
};

template<template<template<generic_arg_like auto...> typename> typename Templ>
struct wrap_higher_template
{
    template<generic_arg_like auto... Args>
    using templ = Templ<wrap_universal<1, Args...>::template templ>;
};
//TODO: Change level 2 to any level > 1

template<
    template<
        template<generic_arg_like auto...> typename
    > typename Templ>
struct universal_arg<2, template_<wrap_higher_template<Templ>::template templ>>
{
    //TODO: Add requirement for instantiating Templ with values of the parameters
    template<parameter_tag... Params>
    constexpr universal_arg(template_signature<Params...>) {}
};


template<parameter_tag... Params>
universal_arg(template_signature<Params...>) -> universal_arg<2, template_<wrap_universal<1>::template templ>>;
} //namespace uta