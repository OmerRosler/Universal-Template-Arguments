#include <uta.hpp>

template<uta::basic_arg Arg>
int basic_universal()
{
    return 0;
}
template<uta::universal_arg Arg>
int variadic_universal()
{
    return 1;
}

template<uta::basic_arg... args>
constexpr int are_all_types()
{
    return ((decltype(args)::tag_enum == uta::basic_arg_type::type) && ...);
}

template<typename...>
struct templ {};

int main()
{
    basic_universal<uta::nttp_<3>{}>();
    basic_universal<uta::type_<int>{}>();

    variadic_universal<uta::variadic_arg_list{uta::type_<int>{}, uta::nttp_<3>{}}>();
    variadic_universal<uta::variadic_arg_list{uta::nttp_<3>{}, uta::type_<int>{}}>();

    variadic_universal<uta::variadic_arg_list{uta::type_<int>{}}>();
    variadic_universal<uta::variadic_arg_list{uta::nttp_<3>{}}>();
    variadic_universal<uta::variadic_arg_list{}>();

    are_all_types<uta::type_<int>{}>();

    static_assert(!are_all_types<uta::type_<int>{}, uta::nttp_<3>{}>());

    variadic_universal<uta::nttp_<3>{}>();
    variadic_universal<uta::type_<int>{}>();

    static_assert(are_all_types<uta::type_<int>{}>());

    static_assert(!are_all_types<uta::nttp_<42>{}>());

}