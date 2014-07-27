#pragma once

namespace script
{
  template <>
  char constexpr const * call_nullary_1<chai_tag>()
  { return "nullary_1();"; }
  template <>
  char constexpr const * call_nullary_2<chai_tag>()
  { return "nullary_2();"; }

  template <>
  char constexpr const * call_unary_1<chai_tag>()
  { return "unary_1(42);"; }
  template <>
  char constexpr const * call_unary_2<chai_tag>()
  { return "unary_2(\"string\");"; }

  template <>
  char constexpr const * call_get_name<chai_tag>()
  { return "get_name();"; }
  template <>
  char constexpr const * call_get_age<chai_tag>()
  { return "get_age();"; }
}
