#pragma once

#include "conversions.hpp"

namespace script
{
  namespace ruby
  {
    /* TODO: Multiple functions of same type? */

    template <typename F, typename C, typename R, typename... Args, size_t... Ns>
    static value_type mem_func_call_impl(F const &f, C &c,
                                std::index_sequence<Ns...> const,
                                value_type * const argv)
    {
      (void)argv; /* XXX: When Ns is 0, argv is not used. */
      return build_return_value<R>(f, c, from_ruby<Args>(argv[Ns])...);
    }
    template <typename F, typename C, typename R, typename... Args>
    value_type mem_func_call(F const &f, int const argc,
                         value_type * const argv, value_type const self)
    {
      juble_assert(f, "invalid function call");
      juble_assert(sizeof...(Args) == argc, "invalid argument count");

      using uptr_t = std::unique_ptr<detail::bare_t<C>>;
      uptr_t * data{};
      Data_Get_Struct(self, uptr_t, data);
      juble_assert(data && data->get(), "invalid object data");

      return mem_func_call_impl<F, C, R, Args...>
        (f, **data, std::index_sequence_for<Args...>{}, argv);
    }

    template <typename Class, typename F>
    class mem_func_wrapper;
    template <typename Class, typename R, typename... Args>
    class mem_func_wrapper<Class, R (Args...)> final
    {
      public:
        mem_func_wrapper() = delete;
        mem_func_wrapper(std::function<R (Class&, Args...)> const &func)
        { func_ = func; }

        static value_type call(int const argc, value_type * const argv,
                               value_type const self)
        {
          return mem_func_call<decltype(func_), Class, R, Args...>
                 (func_, argc, argv, self);
        }

      private:
        static std::function<R (Class&, Args...)> func_;
    };
    template <typename Class, typename R, typename... Args>
    class mem_func_wrapper<Class, R (Args...) const> final
    {
      public:
        mem_func_wrapper() = delete;
        mem_func_wrapper(std::function<R (Class&, Args...)> const &func)
        { func_ = func; }

        static value_type call(int const argc, value_type * const argv,
                               value_type const self)
        {
          return mem_func_call<decltype(func_), Class, R, Args...>
                 (func_, argc, argv, self);
        }

      private:
        static std::function<R (Class&, Args...)> func_;
    };
    template <typename Class, typename R, typename... Args>
    std::function<R (Class&, Args...)> mem_func_wrapper<Class, R (Args...)>::func_;
    template <typename Class, typename R, typename... Args>
    std::function<R (Class&, Args...)> mem_func_wrapper<Class, R (Args...) const>::func_;
  }
}
