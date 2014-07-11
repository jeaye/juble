#pragma once

#include <iostream>
#include <memory>
#include <utility>

#include "system.hpp"
#include "types.hpp"
#include "traits/function.hpp"
#include "assert.hpp"

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

    /* value_type func(value_type const self, value_type const arg...) */
    inline int callback_variadic(int const n)
    { return n; }
    /* value_type func(int const argc, value_type * const argv, value_type const self) */
    int constexpr const callback_argc{ -1 };
    /* value_type func(value_type const self, value_type const args_arr) */
    int constexpr const callback_array{ -2 };

    template <typename T>
    value_type value_class(std::string const &name)
    {
      static value_type const value{ rb_define_class(name.c_str(), rb_cObject) };

      rb_undef_method(value, "initialize");
      auto const init(static_cast<ruby_detail::unary_func_t>(
      [](ruby_detail::value_type const self)
      { return self; }));
      rb_define_method(value, "initialize",
              reinterpret_cast<ruby_detail::any_func_t>(init), 0);
      return value;
    }

    /* TODO: to/from_ruby_impl with an enabler. */
    template <typename T>
    value_type to_ruby(T const &)
    { return 0; }


    template <typename T>
    T from_ruby(value_type const)
    { static typename std::remove_reference<T>::type t; return t; }

    template <>
    int16_t from_ruby<int16_t>(value_type const value)
    { return NUM2INT(value); }

    template <typename Class, typename F>
    class mem_func_wrapper;
    template <typename Class, typename R, typename... Args>
    class mem_func_wrapper<Class, R (Args...)>
    {
      public:
        mem_func_wrapper() = delete;
        mem_func_wrapper(std::function<R (Class&, Args...)> const &func)
        { func_ = func; }

        static R call(value_type const self, Args &&... args)
        {
          juble_assert(func_, "invalid function call");
          std::unique_ptr<Class> * data{};
          Data_Get_Struct(self, std::unique_ptr<Class>, data);
          juble_assert(data && data->get(), "invalid object data");
          return func_(*(*data), std::forward<Args>(args)...);
        }

      private:
        static std::function<R (Class&, Args...)> func_;
    };
    template <typename Class, typename R, typename... Args>
    class mem_func_wrapper<Class, R (Args...) const>
    {
      public:
        mem_func_wrapper() = delete;
        mem_func_wrapper(std::function<R (Class&, Args...)> const &func)
        { func_ = func; }

        static R call(value_type const self, Args &&... args)
        {
          juble_assert(func_, "invalid function call");
          std::unique_ptr<Class> * data{};
          Data_Get_Struct(self, std::unique_ptr<Class>, data);
          juble_assert(data && data->get(), "invalid object data");
          return func_(*(*data), std::forward<Args>(args)...);
        }

      private:
        static std::function<R (Class&, Args...)> func_;
    };
    template <typename Class, typename R, typename... Args>
    std::function<R (Class&, Args...)> mem_func_wrapper<Class, R (Args...)>::func_;
    template <typename Class, typename R, typename... Args>
    std::function<R (Class&, Args...)> mem_func_wrapper<Class, R (Args...) const>::func_;

    template <typename F>
    class ctor_wrapper;
    template <typename Class, typename... Args>
    class ctor_wrapper<Class (Args...)>
    {
      public:
        ctor_wrapper() = delete;
        ctor_wrapper(std::function<Class* (Args...)> const &func)
        { func_ = func; }

        /* TODO: should take argc and iterate with param pack */
        static value_type call(int const argc, value_type * const argv, value_type const self)
        {
          juble_assert(func_, "invalid ctor call");
          juble_assert(sizeof...(Args) == argc, "invalid argument count");

          auto * const data(new std::unique_ptr<Class>
          { call_impl(std::index_sequence_for<Args...>{}, argv) });

          auto const deleter([](void * const data)
          { delete static_cast<std::unique_ptr<Class>*>(data); });

          value_type const obj{ Data_Wrap_Struct(self, nullptr, deleter, data) };
          return obj;
        }

      private:
        template <size_t... Ns>
        static Class* call_impl(std::index_sequence<Ns...> const,
                                value_type * const argv)
        {
          (void)argv; /* XXX: When Ns is 0, argv is not used. */
          return func_(from_ruby<Args>(argv[Ns])...);
        }

        static std::function<Class* (Args...)> func_;
    };
    template <typename Class, typename... Args>
    std::function<Class* (Args...)> ctor_wrapper<Class (Args...)>::func_;
  }
  
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
      template <typename C>
      void add(type<C> const &entry)
      { ruby_detail::value_class<C>(entry.name); }
      template <typename C, typename... Args>
      void add(ctor<C (Args...)> const &entry)
      {
        ruby_detail::ctor_wrapper<C (Args...)>{ entry.func };
        rb_define_singleton_method(
            ruby_detail::value_class<C>(entry.name),
            entry.name.c_str(),
            reinterpret_cast<ruby_detail::any_func_t>
              (&ruby_detail::ctor_wrapper<C (Args...)>::call),
            ruby_detail::callback_argc);
      }
      template <typename C, typename F>
      void add(mem_func_impl<C, F> const &entry)
      {
        /* TODO: store somewhere? */
        ruby_detail::mem_func_wrapper<C, F>{ entry.func };
        rb_define_method(ruby_detail::value_class<C>(entry.name),
            entry.name.c_str(),
            reinterpret_cast<ruby_detail::any_func_t>(&ruby_detail::mem_func_wrapper<C, F>::call),
                              function_taits<F>::arg_count()); /* TODO: argc */
      }
      template <typename G>
      void add_global(G const & /* entry */)
      { }

      static void eval(std::string const &src)
      {
        int err{};
        rb_protect([](ruby_detail::value_type const s)
                   { rb_eval_string((char const*)s); return Qnil; },
                   (ruby_detail::value_type)src.c_str(),
                   &err);
        if(err)
        { std::cout << "ruby error" << std::endl; }
      }

    private:
      system()
      {
        ruby_init();
        ruby_script("juble");
      }
  };
  using ruby_system = system<ruby>;

  template <>
  struct system_traits<ruby>
  {
    static char constexpr const * const name{ "ruby" };
  };
}
