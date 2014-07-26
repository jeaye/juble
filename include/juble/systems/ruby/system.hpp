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
#include <juble/systems/ruby/detail/ctor.hpp>
#include <juble/systems/ruby/detail/function.hpp>
#include <juble/systems/ruby/detail/mem_function.hpp>

namespace script
{
  namespace ruby_detail
  {
    /* TODO: detail/class.hpp */
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
  }
  
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
      template <typename F>
      void add(func_impl<F> const &entry)
      {
        ruby_detail::func_wrapper<F>::add(entry.func, entry.name);
        rb_define_global_function(entry.name.c_str(),
            reinterpret_cast<ruby_detail::any_func_t>(&ruby_detail::func_wrapper<F>::call),
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
            ruby_detail::callback_argc);
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
  using ruby_system = system<ruby_tag>;

  template <>
  struct system_traits<ruby_tag>
  {
    static char constexpr const * const name{ "ruby" };
  };
}
