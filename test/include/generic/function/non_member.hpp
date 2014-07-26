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
  char constexpr const * call_nullary()
  { return ""; }
}

#include "../../chai/function/non_member.hpp"
#include "../../ruby/function/non_member.hpp"

namespace jest
{
  namespace detail
  {
    void nullary()
    { std::cout << "excellent"; }
  }

  template <> template <>
  void script::non_member_group::test<0>() /* nullary function */
  {
    script::registrar::add(script::func(&detail::nullary, "nullary"));

    clear();
    detail::each_system([&](auto tag)
    {
      script::system<decltype(tag)>::eval(script::call_nullary<decltype(tag)>());
      expect_equal(out.str(), "excellent");
      this->clear(); /* XXX: GCC bug requires this-> in generic lambda. */
    });
  }
}
