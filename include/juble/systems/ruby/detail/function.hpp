#pragma once

#include <map>

#include "conversions.hpp"
#include "callback.hpp"

namespace script
{
  namespace ruby
  {
    template <typename F>
    class func_wrapper;
    template <typename R, typename... Args>
    class func_wrapper<R (Args...)>
    {
      public:
        static void add(std::function<R (Args...)> const &func,
                        std::string const &name)
        {
          juble_assert(funcs_.find(name) == funcs_.end(),
                       "non-member function already exists");
          funcs_[name] = func;
        }

        static value_type call(int const argc, value_type * const argv,
                               value_type const )
        {
          /* TODO: Move out into separate util function. */
          auto const name(current_method());
          auto const it(funcs_.find(name));

          juble_assert(it != funcs_.end(),
                       std::string{ "unknown non-member function: " } + name);
          juble_assert(sizeof...(Args) == argc, "invalid argument count");

          return call_impl(it->second, std::index_sequence_for<Args...>{}, argv);
        }

      private:
        template <size_t... Ns>
        static value_type call_impl(std::function<R (Args...)> const &f,
                                    std::index_sequence<Ns...> const,
                                    value_type * const argv)
        {
          (void)argv; /* XXX: When Ns is 0, argv is not used. */
          return build_return_value<R>(f, from_ruby<Args>(argv[Ns])...);
        }

        static std::map<std::string, std::function<R (Args...)>> funcs_;
    };
    template <typename R, typename... Args>
    std::map<std::string, std::function<R (Args...)>> func_wrapper<R (Args...)>::funcs_;
  }
}
