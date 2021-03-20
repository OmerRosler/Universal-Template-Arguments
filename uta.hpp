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
struct type 
{
    static constexpr auto enum_value = basic_arg_type::type;
};

template<auto V>
struct nttp 
{
    static constexpr auto enum_value = basic_arg_type::nttp;
};

template<typename T>
struct is_tag_impl : std::false_type {};

template<typename T>
struct is_tag_impl<type<T>> : std::true_type {};

template<auto V>
struct is_tag_impl<nttp<V>> : std::true_type {};

template<typename T>
concept tag_type = is_tag_impl<T>::value;
/*******************************************************/

template<basic_arg_type tag>
struct basic_universal_arg;

template<>
struct basic_universal_arg<basic_arg_type::type>
{
    template<typename T>
    constexpr basic_universal_arg(type<T>) {}

    static constexpr auto enum_value = basic_arg_type::type;
};

template<>
struct basic_universal_arg<basic_arg_type::nttp>
{
    template<auto V>
    constexpr basic_universal_arg(nttp<V>) {}

    static constexpr auto enum_value = basic_arg_type::nttp;

};

template<typename T>
basic_universal_arg(type<T>) -> 
    basic_universal_arg<basic_arg_type::type>;

template<auto V>
basic_universal_arg(nttp<V>) -> 
    basic_universal_arg<basic_arg_type::nttp>;


/*******************************************************/


enum class arg_type 
{
    type,
    nttp,
    variadic,
    tmpl
};


template<basic_universal_arg... args>
struct variadic_arg_list;

template<basic_universal_arg first, basic_universal_arg... args>
struct variadic_arg_list<first, args...>
{
    template<typename T, tag_type... Args> 
        requires (decltype(first)::enum_value == basic_arg_type::type) && (sizeof...(Args) == sizeof...(args))
    constexpr variadic_arg_list(type<T>, Args...) {}

    template<auto V, tag_type... Args> 
        requires (decltype(first)::enum_value == basic_arg_type::nttp) && (sizeof...(Args) == sizeof...(args))
    constexpr variadic_arg_list(nttp<V>, Args...) {}
};


template<>
struct variadic_arg_list<>
{
    constexpr variadic_arg_list() {}
};

template<tag_type... TaggedTypes> requires (sizeof...(TaggedTypes) != 0)
variadic_arg_list(TaggedTypes...) -> variadic_arg_list<basic_universal_arg{TaggedTypes()}...>;

variadic_arg_list() -> variadic_arg_list<>;

/*******************************************************/

template<arg_type tag>
struct universal_arg;

template<>
struct universal_arg<arg_type::variadic>
{
    template<basic_universal_arg... args>
    constexpr universal_arg(variadic_arg_list<args...>) {}
};


template<>
struct universal_arg<arg_type::nttp>
{
    template<auto V>
    constexpr universal_arg(nttp<V>) {}
};

template<>
struct universal_arg<arg_type::type>
{
    template<typename T>
    constexpr universal_arg(type<T>) {}
};

template<auto V>
universal_arg(nttp<V>) -> universal_arg<arg_type::nttp>;


template<typename T>
universal_arg(type<T>) -> universal_arg<arg_type::type>;

template<basic_universal_arg... args>
universal_arg(variadic_arg_list<args...>) -> universal_arg<arg_type::variadic>;



} //namespace uta
/*
TODO:

3. Make a (more) higher argument type that includes template on 2.
4. Make an "any" argument type which is either of the three.
5. Convert this whole bussiness to "higher" templates (not very easy).
*/
template<uta::basic_universal_arg Arg>
int basic_universal()
{
    return 0;
}

template<uta::universal_arg Arg>
int variadic_universal()
{
    return 1;
}

template<uta::universal_arg... args>
int simpler_variadic()
{
    return 2;
}

int main()
{
    basic_universal<uta::nttp<3>{}>();
    basic_universal<uta::type<int>{}>();

    variadic_universal<uta::variadic_arg_list{uta::type<int>{}, uta::nttp<3>{}}>();
    variadic_universal<uta::variadic_arg_list{uta::nttp<3>{}, uta::type<int>{}}>();

    variadic_universal<uta::variadic_arg_list{uta::type<int>{}}>();
    variadic_universal<uta::variadic_arg_list{uta::nttp<3>{}}>();
    variadic_universal<uta::variadic_arg_list{}>();

    simpler_variadic<uta::type<int>{}>();

    simpler_variadic<uta::type<int>{}, uta::nttp<3>{}>();

    variadic_universal<uta::nttp<3>{}>();
    variadic_universal<uta::type<int>{}>();
}
