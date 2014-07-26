#pragma once

namespace script
{
  template <>
  char constexpr const * call_nullary<chai_tag>()
  { return "nullary();"; }
}
