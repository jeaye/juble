#pragma once

#include "system.hpp"
#include "types.hpp"
#include "file.hpp"

#include <chaiscript/chaiscript.hpp>
#include <chaiscript/chaiscript_stdlib.hpp>

namespace script
{
  struct chai final
  {};

  /* TODO: Provide static functions for ctors and normal ctors. */

  template <>
  class system<chai> final
  {
    public:
      static system& get()
      {
        static system s;
        return s;
      }

      template <template <typename...> class T, typename... E>
      void add(T<E...> const &)
      { juble_assert(false, "unknown chai system entry"); }
      template <typename S>
      void add(type<S> const &entry)
      { chai_.add(chaiscript::user_type<S>(), entry.name); }
      template <typename S>
      void add(ctor<S> const &entry)
      { chai_.add(chaiscript::constructor<S>(), entry.name); }
      template <typename S>
      void add(func_impl<S> const &entry)
      { chai_.add(chaiscript::fun(entry.func), entry.name); }
      template <typename... Ss>
      void add(mem_func_impl<Ss...> const &entry)
      { chai_.add(chaiscript::fun(entry.func), entry.name); }
      template <typename... Ss>
      void add(mem_var_impl<Ss...> const &entry)
      { chai_.add(chaiscript::fun(entry.func), entry.name); }
      template <typename S, std::enable_if_t<!std::is_const<S>::value>>
      void add(var_impl<S> const &entry)
      { chai_.add(chaiscript::var(entry.value), entry.name); }
      template <typename S, std::enable_if_t<std::is_const<S>::value>>
      void add(var_impl<S> const &entry)
      { chai_.add(chaiscript::const_var(entry.value), entry.name); }

      template <template <typename...> class T, typename E>
      void add_global(T<E> const &)
      { juble_assert(false, "unknown chai global type"); }
      template <typename S, std::enable_if_t<!std::is_const<S>::value>>
      void add_global(global_var_impl<S> const &entry)
      { chai_.add_global(chaiscript::var(entry.value), entry.name); }
      template <typename S, std::enable_if_t<std::is_const<S>::value>>
      void add_global(global_var_impl<S> const &entry)
      { chai_.add_global(chaiscript::const_var(entry.value), entry.name); }

      template <typename T = void>
      static T eval(std::string const &code)
      { return get().chai_.eval<T>(code); }
      static void eval(std::string const &code)
      { get().chai_.eval(code); }

      template <typename T = void>
      static T eval(file const &f)
      { return get().chai_.eval_file<T>(f.data); }
      static void eval(file const &f)
      { get().chai_.eval_file(f.data); }

      static void use(std::string const &file)
      { get().chai_.use(file); }

      template <typename T>
      static T get(std::string const &name)
      { return get().chai_.eval<T>(name); }
      template <typename T>
      static std::function<T> get_func(std::string const &name)
      { return get().chai_.eval<std::function<T>>(name); }

    private:
      system()
        : chai_(chaiscript::Std_Lib::library())
      { }

      chaiscript::ChaiScript chai_;
  };
  template <>
  struct system_traits<chai> final
  {
    static char constexpr const * const name{ "chai" };
  };
}
