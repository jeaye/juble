#pragma once

#include <string>
#include <functional>

namespace script
{
  /* These are wrapped in factory functions to help
   * with type deduction. For references in vars, use
   * std::reference_wrapper. */

  template <typename T>
  struct type final
  {
    explicit type(std::string const &name)
      : name{ name }
    { }

    std::string const name;
  };

  template <typename T>
  struct ctor;
  template <typename Ret, typename... Args>
  struct ctor<Ret (Args...)> final
  {
    /* ctors are non-addressable, so wrap them in a lambda function. */
    explicit ctor(std::string const &name)
      /* XXX: explicitly call ctor; do not use an initializer_list */
      : func{ [](Args &&... args){ return new Ret(std::forward<Args>(args)...); } } 
      , name{ name }
    { }

    std::function<Ret* (Args...)> const func;
    std::string const name; /* TODO: registrar::name_list for name-per-system */
  };

  template <typename T>
  struct func_impl;
  template <typename Ret, typename... Args>
  struct func_impl<Ret (Args...)> final
  {
    explicit func_impl(std::function<Ret (Args...)> const &f,
              std::string const &name)
      : func{ f }, name{ name }
    { }

    std::function<Ret (Args...)> const func;
    std::string const name;
  };
  template <typename T>
  auto func(T * const f, std::string const &name)
  { return func_impl<T>{ f, name }; }
  template <typename T>
  auto func(T const &f, std::string const &name)
  { return func_impl<T>{ f, name }; }

  template <typename T, typename F>
  struct mem_func_impl;
  template <typename T, typename Ret, typename... Args>
  struct mem_func_impl<T, Ret (Args...)> final
  {
    explicit mem_func_impl(Ret (T::* const f)(Args...), std::string const &name)
      : func{ f }, base_func{ f }, name{ name }
    { }

    std::function<Ret (T&, Args...)> const func;
    Ret (T::* const base_func)(Args...);
    std::string const name;
  };
  template <typename T, typename Ret, typename... Args>
  auto mem_func(Ret (T::* const f)(Args...),
                              std::string const &name)
  { return mem_func_impl<T, Ret (Args...)>{ f, name }; }

  /* Support for const member functions. */
  template <typename T, typename Ret, typename... Args>
  struct mem_func_impl<T, Ret (Args...) const> final
  {
    explicit mem_func_impl(Ret (T::* const f)(Args...) const,
                  std::string const &name)
      : func{ f }, base_func{ f }, name{ name }
    { }

    std::function<Ret (T&, Args...)> const func;
    Ret (T::* const base_func)(Args...) const;
    std::string const name;
  };
  template <typename T, typename Ret, typename... Args>
  auto mem_func(Ret (T::* const f)(Args...) const,
                              std::string const &name)
  { return mem_func_impl<T, Ret (Args...) const>{ f, name }; }

  template <typename C, typename T>
  struct mem_var_impl final
  {
    explicit mem_var_impl(T C::* const m, std::string const &name)
      : func{ m }, name{ name }
    { }

    T C::* const func;
    std::string const name;
  };
  template <typename C, typename T>
  auto mem_var(T C::* const m, std::string const &name)
  { return mem_var_impl<C, T>{ m, name }; }

  template <typename T>
  struct var_impl final
  {
    explicit var_impl(T const &v, std::string const &name)
      : value{ v }, name{ name }
    { }
    explicit var_impl(T &&v, std::string const &name)
      : value{ std::move(v) }, name{ name }
    { }

    T const value;
    std::string const name;
  };
  template <typename T>
  var_impl<T> var(T &&v, std::string const &name)
  { return { std::forward<T>(v), name }; }

  template <typename T>
  struct global_var_impl final
  {
    explicit global_var_impl(T const &v, std::string const &name)
      : value{ v }, name{ name }
    { }
    explicit global_var_impl(T &&v, std::string const &name)
      : value{ std::move(v) }, name{ name }
    { }

    T const value;
    std::string const name;
  };
  template <typename T>
  global_var_impl<T> global_var(T &&v, std::string const &name)
  { return { std::forward<T>(v), name }; }
}
