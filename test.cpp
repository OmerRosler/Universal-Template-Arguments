#include <uta.hpp>

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
