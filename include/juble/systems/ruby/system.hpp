#pragma once

#include <iostream>
#include <memory>
#include <utility>

#include <juble/system.hpp>
#include <juble/types.hpp>
#include <juble/detail/traits/function.hpp>
#include <juble/detail/traits/attributes.hpp>
#include <juble/detail/assert.hpp>

#include <juble/systems/ruby/detail/conversions.hpp>
#include <juble/systems/ruby/detail/class.hpp>
#include <juble/systems/ruby/detail/ctor.hpp>
#include <juble/systems/ruby/detail/function.hpp>
#include <juble/systems/ruby/detail/mem_function.hpp>

namespace script
{
  struct ruby_tag final
  {};
  template <>
  class system<ruby_tag> final
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
      { ruby::value_class<C>(entry.name); }
      template <typename C, typename... Args>
      void add(ctor<C (Args...)> const &entry)
      {
        ruby::ctor_wrapper<C (Args...)>{ entry.func };
        rb_define_singleton_method(
            ruby::value_class<C>(entry.name),
            entry.name.c_str(),
            reinterpret_cast<ruby::any_func_t>
              (&ruby::ctor_wrapper<C (Args...)>::call),
            ruby::callback_argc);
      }
      template <typename F>
      void add(func_impl<F> const &entry)
      {
        ruby::func_wrapper<F>::add(entry.func, entry.name);
        rb_define_global_function(entry.name.c_str(),
            reinterpret_cast<ruby::any_func_t>(&ruby::func_wrapper<F>::call),
            ruby::callback_argc);
      }
      template <typename C, typename F>
      void add(mem_func_impl<C, F> const &entry)
      {
        /* TODO: store somewhere? */
        ruby::mem_func_wrapper<C, F>{ entry.func };
        rb_define_method(ruby::value_class<C>(entry.name),
            entry.name.c_str(),
            reinterpret_cast<ruby::any_func_t>(&ruby::mem_func_wrapper<C, F>::call),
            ruby::callback_argc);
      }
      template <typename G>
      void add_global(G const & /* entry */)
      { }

      template <typename R = void>
      static R eval(std::string const &src)
      {
        int err{};
        ruby::value_type const val
        {
          rb_protect([](ruby::value_type const s)
          { return rb_eval_string(reinterpret_cast<char const*>(s)); },
          (ruby::value_type)src.c_str(), &err)
        };
        if(err)
        { throw std::runtime_error{ "ruby error" }; }
        return ruby::from_ruby<R>(val);
      }
      static void eval(std::string const &src)
      { return eval<>(src); }

    private:
      system()
      {
        ruby_init();
        ruby_script("juble");
      }
  };
  using ruby_system = system<ruby_tag>;

  template <>
  struct system_traits<ruby_tag>
  {
    static char constexpr const * const name{ "ruby" };
  };
}
