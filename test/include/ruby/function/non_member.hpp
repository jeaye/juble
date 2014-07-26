#pragma once

namespace script
{
  template <>
  char constexpr const * call_nullary_1<ruby_tag>()
  { return "nullary_1();"; }
  template <>
  char constexpr const * call_nullary_2<ruby_tag>()
  { return "nullary_2();"; }
}
