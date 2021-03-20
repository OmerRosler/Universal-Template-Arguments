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
    basic_universal<uta::nttp_tag<3>{}>();
    basic_universal<uta::type_tag<int>{}>();

    variadic_universal<uta::variadic_arg_list{uta::type_tag<int>{}, uta::nttp_tag<3>{}}>();
    variadic_universal<uta::variadic_arg_list{uta::nttp_tag<3>{}, uta::type_tag<int>{}}>();

    variadic_universal<uta::variadic_arg_list{uta::type_tag<int>{}}>();
    variadic_universal<uta::variadic_arg_list{uta::nttp_tag<3>{}}>();
    variadic_universal<uta::variadic_arg_list{}>();

    are_all_types<uta::type_tag<int>{}>();

    static_assert(!are_all_types<uta::type_tag<int>{}, uta::nttp_tag<3>{}>());

    variadic_universal<uta::nttp_tag<3>{}>();
    variadic_universal<uta::type_tag<int>{}>();

    static_assert(are_all_types<uta::type_tag<int>{}>());

    static_assert(!are_all_types<uta::nttp_tag<42>{}>());

}