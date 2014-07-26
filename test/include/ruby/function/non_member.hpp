#pragma once

namespace script
{
  template <>
  char constexpr const * call_nullary<ruby_tag>()
  { return "nullary();"; }
}
