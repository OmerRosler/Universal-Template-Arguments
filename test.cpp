
#include <uta.hpp>

template<uta::basic_arg Arg>
int basic_universal()
{
    return 0;
}

template<uta::basic_arg... args>
constexpr int are_all_types()
{
    return ((decltype(args)::tag_enum == uta::basic_arg_type::type) && ...);
}

template<typename T, auto V>
struct templ_example {};

template<uta::basic_arg T, uta::basic_arg V>
using templ_adaptor = templ_example<typename decltype(T)::type, decltype(V)::value>;

template<template<uta::basic_arg, uta::basic_arg> typename Templ>
int test_templ() { return 42;}

template<uta::template_signature templ>
int test_sig_type() { return 42;}

template<uta::universal_arg Any>
int test_templated_arg() { return 42;}

int main()
{
    basic_universal<uta::nttp_<3>{}>();
    basic_universal<uta::type_<int>{}>();

    are_all_types<uta::type_<int>{}>();

    static_assert(!are_all_types<uta::type_<int>{}, uta::nttp_<3>{}>());

    static_assert(are_all_types<uta::type_<int>{}>());

    static_assert(!are_all_types<uta::nttp_<42>{}>());

    constexpr uta::template_signature t{uta::variadic_arg_list{uta::nttp_<42>{}, uta::type_<int>{}, uta::type_<double>{}}};

    test_sig_type<t>();

    test_templ<templ_adaptor>();

    static_assert(std::is_same_v<decltype(t), const uta::template_signature<uta::nttp_p, uta::type_p, uta::type_p>>);

    test_templated_arg<uta::type_<int>{}>();

    test_templated_arg<uta::template_<templ_adaptor>{}>();


}