#pragma once

#include <stdexcept>
#include <string>

#include <juble/detail/assert.hpp>

namespace script
{
  /* Specialize this interface for any new script systems. */
  template <typename T>
  class system final
  {
    public:
      [[noreturn]] static system& get()
      { die(); }

      /* Overload/specialize these as required. */
      template <typename E>
      [[noreturn]] void add(E const & /* entry */)
      { die(); }
      template <typename G>
      [[noreturn]] void add_global(G const & /* entry */)
      { die(); }

    private:
      [[noreturn]] system()
      { die(); }

      [[noreturn]] static void die() 
      { juble_assert(false, "system not implemented"); }
  };

  /* Specialize to describe your system. */
  template <typename T>
  struct system_traits final
  {
    static char constexpr const * const name{ "unknown" };
  };
}
