#pragma once

#include <cstdint>

namespace script
{
  namespace detail
  {
    template <typename F>
    struct arg_count;

    template <typename R, typename... Args>
    struct arg_count<R (Args...)>
    { static size_t constexpr const value{ sizeof...(Args) }; };
    template <typename R, typename... Args>
    struct arg_count<R (Args...) const>
    { static size_t constexpr const value{ sizeof...(Args) }; };

    template <typename F>
    struct is_const;
    template <typename R, typename... Args>
    struct is_const<R (Args...)>
    { static bool constexpr const value{ false }; };
    template <typename R, typename... Args>
    struct is_const<R (Args...) const>
    { static bool constexpr const value{ true }; };

    template <typename F>
    struct return_type_impl;
    template <typename R, typename... Args>
    struct return_type_impl<R (Args...)>
    { using type = R; };
    template <typename R, typename... Args>
    struct return_type_impl<R (Args...) const>
    { using type = R; };
    template <typename F>
    using return_type = typename return_type_impl<F>::type;
  }

  template <typename F>
  struct function_taits
  {
    using return_type = detail::return_type<F>;

    static size_t constexpr arg_count()
    { return detail::arg_count<F>::value; }

    static bool constexpr is_const()
    { return detail::is_const<F>::value; }
  };
}
