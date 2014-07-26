#pragma once

namespace script
{
  template <>
  char constexpr const * call_nullary_1<chai_tag>()
  { return "nullary_1();"; }
  template <>
  char constexpr const * call_nullary_2<chai_tag>()
  { return "nullary_2();"; }
}
