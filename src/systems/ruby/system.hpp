#pragma once

#include <iostream>
#include <memory>
#include <utility>

#include "system.hpp"
#include "types.hpp"
#include "traits/function.hpp"
#include "traits/attributes.hpp"
#include "assert.hpp"

#include "detail/callbacks.hpp"

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
  using ruby_system = system<ruby>;

  template <>
  struct system_traits<ruby>
  {
    static char constexpr const * const name{ "ruby" };
  };
}
