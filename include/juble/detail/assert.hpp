#pragma once

#include <cassert>

namespace script
{
  template <typename T>
  void juble_assert(T const &test, char const * const str)
  {
    if(!static_cast<bool>(test))
    {
      std::cerr << "assertion failure: " << str << std::endl;
      assert(false);
    }
  }
  template <typename T>
  void juble_assert(T const &test, std::string const &str)
  { juble_assert(static_cast<bool>(test), str.c_str()); }
}
