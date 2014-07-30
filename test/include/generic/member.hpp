#pragma once

#include <juble/detail/config.hpp>
#include <juble/detail/gen_config.hpp>
#include <juble/detail/traits/attributes.hpp>

#include "util.hpp"

namespace script
{
  template <typename... Systems>
  struct member
  {
    member()
    { std::cout.rdbuf(out.rdbuf()); }

    void clear()
    { out.str(""); }

    std::stringstream out;
  };
  using member_group = jest::group<member<JUBLE_SYSTEMS>>;
  member_group const member_obj{ "member" };

}

#if JUBLE_CHAI
  #include "../../chai/function/member.hpp"
#endif
#if JUBLE_RUBY
  #include "../../ruby/function/member.hpp"
#endif

namespace jest
{
  template <> template <>
  void script::member_group::test<0>() /* nullary function */
  {
  }
}
