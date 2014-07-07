#pragma once

#include <string>
#include <functional>

namespace script
{
  /* These are wrapped in factory functions to help
   * with type deduction. For references in vars, use
   * std::reference_wrapper. */

  template <typename T>
  struct type
  {
    type(std::string const &name, std::string const &desc = "")
      : name{ name }, description{ desc }
    { }

    std::string const name;
    std::string const description;
  };

  template <typename T>
  struct ctor;
  template <typename Ret, typename... Args>
  struct ctor<Ret (Args...)>
  {
    /* ctors are non-addressable, so wrap them in a lambda function. */
    ctor(std::string const &name, std::string const &desc = "")
      /* XXX: explicitly call ctor; do not use an initializer_list */
      : func{ [](Args &&... args){ return Ret(std::forward<Args>(args)...); } } 
      , name{ name }, description{ desc }
    { }

    std::function<Ret (Args...)> const func;
    std::string const name;
    std::string const description;
  };

  template <typename T>
  struct func_impl;
  template <typename Ret, typename... Args>
  struct func_impl<Ret (Args...)>
  {
    func_impl(std::function<Ret (Args...)> const &f,
              std::string const &name, std::string const &desc)
      : func{ f }, name{ name }, description{ desc }
    { }

    std::function<Ret (Args...)> const func;
    std::string const name;
    std::string const description;
  };
  template <typename T>
  func_impl<T> func(T * const f, std::string const &name,
                    std::string const &desc = "")
  { return { f, name, desc }; }
  template <typename T>
  func_impl<T> func(T const &f, std::string const &name,
                    std::string const &desc = "")
  { return { f, name, desc }; }

  template <typename T, typename F>
  struct mem_func_impl;
  template <typename T, typename Ret, typename... Args>
  struct mem_func_impl<T, Ret (Args...)>
  {
    mem_func_impl(Ret (T::* const f)(Args...), std::string const &name,
                  std::string const &desc)
      : func{ f }, base_func{ f }, name{ name }, description{ desc }
    { }

    std::function<Ret (T&, Args...)> const func;
    Ret (T::* const base_func)(Args...);
    std::string const name;
    std::string const description;
  };
  template <typename T, typename Ret, typename... Args>
  mem_func_impl<T, Ret (Args...)> mem_func(Ret (T::* const f)(Args...),
                              std::string const &name, std::string const &desc = "")
  { return { f, name, desc }; }

  /* Support for const member functions. */
  template <typename T, typename Ret, typename... Args>
  struct mem_func_impl<T, Ret (Args...) const>
  {
    mem_func_impl(Ret (T::* const f)(Args...) const,
                  std::string const &name, std::string const &desc)
      : func{ f }, base_func{ f }, name{ name }, description{ desc }
    { }

    std::function<Ret (T&, Args...)> const func;
    Ret (T::* const base_func)(Args...) const;
    std::string const name;
    std::string const description;
  };
  template <typename T, typename Ret, typename... Args>
  mem_func_impl<T, Ret (Args...) const>  mem_func(Ret (T::* const f)(Args...) const,
                              std::string const &name, std::string const &desc = "")
  { return { f, name, desc }; }

  template <typename C, typename T>
  struct mem_var_impl
  {
    mem_var_impl(T C::* const m, std::string const &name, std::string const &desc)
      : func{ m }, name{ name }, description{ desc }
    { }

    T C::* const func;
    std::string const name;
    std::string const description;
  };
  template <typename C, typename T>
  mem_var_impl<C, T> mem_var(T C::* const m, std::string const &name,
                             std::string const &desc = "")
  { return { m, name, desc }; }

  template <typename T>
  struct var_impl
  {
    var_impl(T const &v, std::string const &name,
             std::string const &desc)
      : value{ v }, name{ name }, description{ desc }
    { }
    var_impl(T &&v, std::string const &name,
             std::string const &desc)
      : value{ std::move(v) }, name{ name }, description{ desc }
    { }

    T const value;
    std::string const name;
    std::string const description;
  };
  template <typename T>
  var_impl<T> var(T &&v, std::string const &name,
                  std::string const &desc = "")
  { return { std::forward<T>(v), name, desc }; }

  template <typename T>
  struct global_var_impl
  {
    global_var_impl(T const &v, std::string const &name,
                   std::string const &desc)
      : value{ v }, name{ name }, description{ desc }
    { }
    global_var_impl(T &&v, std::string const &name,
                   std::string const &desc)
      : value{ std::move(v) }, name{ name }, description{ desc }
    { }

    T const value;
    std::string const name;
    std::string const description;
  };
  template <typename T>
  global_var_impl<T> global_var(T &&v, std::string const &name,
                                std::string const &desc = "")
  { return { std::forward<T>(v), name, desc }; }
}
