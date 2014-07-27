#pragma once

#include <juble/detail/config.hpp>
#include <juble/detail/gen_config.hpp>
#include <juble/detail/traits/attributes.hpp>

#include "util.hpp"

namespace script
{
  template <typename... Systems>
  struct non_member
  {
    non_member()
    { std::cout.rdbuf(out.rdbuf()); }

    void clear()
    { out.str(""); }

    std::stringstream out;
  };
  using non_member_group = jest::group<non_member<JUBLE_SYSTEMS>>;
  non_member_group const non_member_obj{ "non-member" };

  template <typename System>
  char constexpr const * call_nullary_1()
  { return ""; }
  template <typename System>
  char constexpr const * call_nullary_2()
  { return ""; }
  template <typename System>
  char constexpr const * call_unary_1()
  { return ""; }
  template <typename System>
  char constexpr const * call_unary_2()
  { return ""; }
  template <typename System>
  char constexpr const * call_get_name()
  { return ""; }
  template <typename System>
  char constexpr const * call_get_age()
  { return ""; }
}

#if JUBLE_CHAI
  #include "../../chai/function/non_member.hpp"
#endif
#if JUBLE_RUBY
  #include "../../ruby/function/non_member.hpp"
#endif

namespace jest
{
  namespace detail
  {
    void nullary_1()
    { std::cout << "excellent"; }
    void nullary_2()
    { std::cout << "\nlines\n"; }

    void unary_1(int const i)
    { std::cout << i; }
    void unary_2(std::string const &s)
    { std::cout << s; }

    std::string get_name()
    { return "Thomas"; }
    size_t get_age()
    { return 22; }
  }

  template <> template <>
  void script::non_member_group::test<0>() /* nullary function */
  {
    script::registrar::add(script::func(&detail::nullary_1, "nullary_1"));
    script::registrar::add(script::func(&detail::nullary_2, "nullary_2"));

    clear();
    detail::each_system([&](auto tag)
    {
      using T = decltype(tag);

      script::system<T>::eval(script::call_nullary_1<T>());
      expect_equal(out.str(), "excellent");
      this->clear();

      script::system<T>::eval(script::call_nullary_2<T>());
      expect_equal(out.str(), "\nlines\n");
      this->clear();
    });
  }

  template <> template <>
  void script::non_member_group::test<1>() /* unary function */
  {
    script::registrar::add(script::func(&detail::unary_1, "unary_1"));
    script::registrar::add(script::func(&detail::unary_2, "unary_2"));

    clear();
    detail::each_system([&](auto tag)
    {
      using T = decltype(tag);

      script::system<T>::eval(script::call_unary_1<T>());
      expect_equal(out.str(), "42");
      this->clear();

      script::system<T>::eval(script::call_unary_2<T>());
      expect_equal(out.str(), "string");
      this->clear();
    });
  }

  template <> template <>
  void script::non_member_group::test<2>() /* return value */
  {
    script::registrar::add(script::func(&detail::get_name, "get_name"));
    script::registrar::add(script::func(&detail::get_age, "get_age"));

    detail::each_system([&](auto tag)
    {
      using T = decltype(tag);

      auto const name(script::system<T>::template eval<std::string>(script::call_get_name<T>()));
      expect_equal(name, "Thomas");

      auto const age(script::system<T>::template eval<size_t>(script::call_get_age<T>()));
      expect_equal(age, 22ul);
    });
  }
}
