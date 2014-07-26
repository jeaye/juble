#pragma once

namespace jest
{
  namespace detail
  {
    template <typename... Systems, typename F>
    void each_system_impl(F const &f)
    { 
      int const _[]
      {(
          std::cerr << "  - testing system '"
                    << script::system_traits<Systems>::name
                    << "'" << std::endl,
          f(Systems{})
          , 0)...
      };
      (void)_;
    }
    template <typename F>
    void each_system(F const &f)
    { each_system_impl<JUBLE_SYSTEMS>(f); }
  }
}
