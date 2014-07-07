#pragma once

#include <stdexcept>
#include <string>

namespace script
{
  /* Specialize this interface for any new script systems. */
  template <typename T>
  class system
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
      { throw std::runtime_error{ "system not implemented" }; }
  };

  /* Specialize to describe your system. */
  template <typename T>
  struct system_traits
  {
    static char constexpr const * const name{ "unknown" };
  };
}
