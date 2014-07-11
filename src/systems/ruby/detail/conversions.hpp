#pragma once

#include <type_traits>
#include <string>
#include <memory>

#include "traits/attributes.hpp"

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

    /* TODO: to_ruby string, array, etc. from_ruby array, etc. */
    template <typename T, typename E = void>
    struct to_ruby_impl final
    {
      static value_type convert(T const&)
      {
        /* TODO: Class types. */
        return Qnil;
      }
    };
    template <typename T>
    struct to_ruby_impl<T, std::enable_if_t<std::is_integral<T>::value>> final
    {
      static value_type convert(T const data)
      { return INT2NUM(data); }
    };
    template <typename T>
    value_type to_ruby(T const &data)
    { return to_ruby_impl<T>::convert(data); }

    template <typename T, typename P>
    T regulate(std::unique_ptr<P> &data, std::enable_if_t<!(std::is_reference<T>::value || std::is_pointer<T>::value)>* = nullptr)
    { return *data; }
    template <typename T, typename P>
    T& regulate(std::unique_ptr<P> &data, std::enable_if_t<std::is_reference<T>::value>* = nullptr)
    { return *data; }
    template <typename T, typename P>
    T* regulate(std::unique_ptr<P> &data, std::enable_if_t<std::is_pointer<T>::value>* = nullptr)
    { return data.get(); }

    template <typename T, typename E = void>
    struct from_ruby_impl final
    {
      static T& convert(value_type const value)
      {
        using uptr_t = std::unique_ptr<detail::bare_t<T>>;

        uptr_t * data{};
        Data_Get_Struct(value, uptr_t, data);
        juble_assert(data && (*data).get(), "invalid object data");
        return regulate<T>(*data);
      }
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
    decltype(auto) from_ruby(value_type const value)
    { return from_ruby_impl<T>::convert(value); }


    template <typename T, typename F, typename... Args>
    std::enable_if_t<std::is_void<T>::value, value_type>
      build_return_value(F const &f, Args &&... args)
    { f(std::forward<Args>(args)...); return Qnil; }
    template <typename T, typename F, typename... Args>
    std::enable_if_t<!std::is_void<T>::value, value_type>
      build_return_value(F const &f, Args &&... args)
    { return to_ruby<T>(f(std::forward<Args>(args)...)); }
  }
}
