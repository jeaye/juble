#pragma once

#include "system.hpp"

namespace script
{
  struct ruby final
  {};

  template <>
  class system<ruby> final
  {
    public:
      static system& get()
      {
        static system s;
        return s;
      }

      template <typename E>
      void add(E const & /* entry */)
      { }
      template <typename G>
      void add_global(G const & /* entry */)
      { }
  };

  template <>
  struct system_traits<ruby>
  {
    static char constexpr const * const name{ "ruby" };
  };
}
