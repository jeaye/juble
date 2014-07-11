#pragma once

#include <type_traits>
#include <string>

#include "../lib/ruby/include/ruby.h"

namespace script
{
  namespace ruby_detail
  {
    using value_type = VALUE;
#undef VALUE
    using unary_func_t = value_type (*)(value_type);
    using any_func_t = value_type (*)(ANYARGS);
#undef ANYARGS
    auto get_type(value_type const value)
    { return TYPE(value); }
#undef TYPE

    /* TODO: to/from_ruby_impl with an enabler. */
    template <typename T>
    value_type to_ruby(T const &)
    { return 0; }


    template <typename T, typename E = void>
    struct from_ruby_impl final
    {
      static T convert(value_type const)
      { static typename std::remove_reference<T>::type t{}; return t; }
    };
    template <typename T>
    struct from_ruby_impl<T, std::enable_if_t<std::is_integral<T>::value>> final
    {
      static T convert(value_type const value)
      { return static_cast<T>(NUM2INT(value)); }
    };
    template <typename T>
    struct from_ruby_impl<T, std::enable_if_t<std::is_same<detail::bare_t<T>, std::string>::value>> final
    {
      static std::string convert(value_type const value)
      { return { RSTRING_PTR(value) }; }
    };
    template <typename T>
    auto from_ruby(value_type const value)
    { return from_ruby_impl<T>::convert(value); }
  }
}
