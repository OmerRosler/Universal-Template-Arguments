
#include <uta.hpp>

template<uta::level_0 Arg>
int basic_universal()
{
    return 0;
}

template<uta::level_0... args>
constexpr int are_all_types()
{
    return ((decltype(args)::tag_enum == uta::arg_type::type) && ...);
}

template<typename T, auto V>
struct templ_example {};

template<uta::level_0 T, uta::level_0 V>
using templ_adaptor = templ_example<typename decltype(T)::type, decltype(V)::value>;

template<uta::level_0... Args>
struct test_variadic_template {};

template<template<uta::level_0, uta::level_0> typename Templ>
int test_templ() { return 42;}


template<uta::universal_argument Any>
int test_templated_arg() { return 42;}


template<uta::universal_argument Any>
auto test_template1_apply()
{
    return typename decltype(Any)::template apply<uta::type_<int>{}, uta::nttp_<42>{}>::type();
}

template<uta::level_2 Sig>
auto test_template2_usage()
{
    return typename decltype(Sig)::template apply<templ_adaptor>::template templ<uta::type_<int>{}, uta::nttp_<42>{}>();
}

int main()
{
    basic_universal<uta::nttp_<3>{}>();
    basic_universal<uta::type_<int>{}>();

    are_all_types<uta::type_<int>{}>();

    static_assert(!are_all_types<uta::type_<int>{}, uta::nttp_<3>{}>());

    static_assert(are_all_types<uta::type_<int>{}>());

    static_assert(!are_all_types<uta::nttp_<42>{}>());

    test_templ<templ_adaptor>();

    test_templated_arg<uta::type_<int>{}>();

    test_templated_arg<uta::template_<templ_adaptor>{}>();

    test_template1_apply<uta::template_<templ_adaptor>{}>();

    constexpr uta::template_parameter_accepting<uta::type_p, uta::nttp_p> sig{};

    test_template2_usage<sig>();


}