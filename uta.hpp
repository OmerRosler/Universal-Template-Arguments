
#include <type_traits>

namespace uta
{

enum class basic_arg_type
{
    type,
    nttp
};


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

template<typename T>
struct is_tag_impl : std::false_type {};

template<typename T>
struct is_tag_impl<type_<T>> : std::true_type {};

template<auto V>
struct is_tag_impl<nttp_<V>> : std::true_type {};

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
struct basic_arg<type_<T>>
{
    constexpr basic_arg(type_<T>) noexcept {}

    using type = T;

    static constexpr auto tag_enum = basic_arg_type::type;
};

template<auto V>
struct basic_arg<nttp_<V>>
{
    constexpr basic_arg(nttp_<V>) noexcept {}

    static constexpr auto tag_enum = basic_arg_type::nttp;

    static constexpr auto value = V;
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

/* TODO: The logic that unified `auto` and `typename` needs to be extended
to unify `template<auto>`, `template<typename>`.

This will be used to unify `nttp_` and `type_` 

Then we could create a new tag template for `template_<nttp_, type_, type_>` or something similar
*/



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
struct template_tag
{
    template<basic_arg... Values> requires (sizeof...(Params) == sizeof...(Values)) &&
        ((decltype(Values)::tag_enum == Params::tag_enum) && ...)
    constexpr template_tag(variadic_arg_list<Values...>) {
    }
};

template<basic_arg_type ParamTag>
constexpr auto transform_param_to_arg_tag()
{
    if constexpr (ParamTag == basic_arg_type::nttp)
    {
        return constexpr_declval<nttp_p>();
    }
    else if (ParamTag == basic_arg_type::type)
    {
        return constexpr_declval<type_p>();
    }
}

template<basic_arg_type ParamTag>
using transform_param_to_arg_tag_t = decltype(transform_param_to_arg_tag<ParamTag>());

template<basic_arg... Values> requires (sizeof...(Values) != 0)
template_tag(variadic_arg_list<Values...>) -> template_tag<transform_param_to_arg_tag_t<decltype(Values)::tag_enum>...>;



} //namespace uta