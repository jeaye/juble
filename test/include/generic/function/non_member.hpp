#pragma once

#include <juble/detail/config.hpp>
#include <juble/detail/gen_config.hpp>
#include <juble/detail/traits/attributes.hpp>

#include "util.hpp"

namespace script
{
  template <typename... Systems>
  struct non_member
  {
    non_member()
    { std::cout.rdbuf(out.rdbuf()); }

    void clear()
    { out.str(""); }

    std::stringstream out;
  };
  using non_member_group = jest::group<non_member<JUBLE_SYSTEMS>>;
  non_member_group const non_member_obj{ "non-member" };

  template <typename System>
  char constexpr const * call_nullary_1()
  { return ""; }
  template <typename System>
  char constexpr const * call_nullary_2()
  { return ""; }
}

#include "../../chai/function/non_member.hpp"
#include "../../ruby/function/non_member.hpp"

namespace jest
{
  namespace detail
  {
    void nullary_1()
    { std::cout << "excellent"; }
    void nullary_2()
    { std::cout << "\nlines\n"; }
  }

  template <> template <>
  void script::non_member_group::test<0>() /* nullary function */
  {
    script::registrar::add(script::func(&detail::nullary_1, "nullary_1"));
    script::registrar::add(script::func(&detail::nullary_2, "nullary_2"));

    clear();
    detail::each_system([&](auto tag)
    {
      script::system<decltype(tag)>::eval(script::call_nullary_1<decltype(tag)>());
      expect_equal(out.str(), "excellent");

      this->clear();
      script::system<decltype(tag)>::eval(script::call_nullary_2<decltype(tag)>());
      expect_equal(out.str(), "\nlines\n");

      this->clear();
    });
  }
}
