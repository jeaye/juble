#pragma once

#include <juble/detail/config.hpp>
#include <juble/detail/gen_config.hpp>
#include <juble/detail/traits/attributes.hpp>

#include "util.hpp"

namespace script
{
  template <typename... Systems>
  struct ctor_test
  {
    void engage()
    { std::cout.rdbuf(out.rdbuf()); }

    void clear()
    { out.str(""); }

    std::stringstream out;
  };
  using ctor_group = jest::group<ctor_test<JUBLE_SYSTEMS>>;
  ctor_group const ctor_obj{ "ctor" };

  struct person
  {
    person()
    { std::cout << "person()"; }
    person(person const &p)
      : name_{ p.name_ }
    { std::cout << "person(person const&)" << name_; }
    person(std::string const &name)
      : name_{ name }
    { std::cout << "person(" << name << ")"; }

    static void register_api()
    {
      script::registration<person> const registration
      {
        script::type<person>("Person"),
        script::ctor<person ()>("new"),
        script::ctor<person (person const&)>("new_copy"),
        script::ctor<person (std::string const&)>("new_name")
      };
    }

    std::string name_;
  };

  template <typename T>
  char constexpr const * call_person_ctor()
  { return ""; }
  template <typename T>
  char constexpr const * call_person_ctor_output()
  { return "person()"; }

  template <typename T>
  char constexpr const * call_person_ctor2()
  { return ""; }
  template <typename T>
  char constexpr const * call_person_ctor2_output()
  { return "person()person(person const&)"; }

  template <typename T>
  char constexpr const * call_person_ctor_name()
  { return ""; }
  template <typename T>
  char constexpr const * call_person_ctor_name_output()
  { return "person(jerry)"; }
}

#if JUBLE_CHAI
  #include "../chai/ctor.hpp"
#endif
#if JUBLE_RUBY
  #include "../ruby/ctor.hpp"
#endif

namespace jest
{
  template <> template <>
  void script::ctor_group::test<0>() /* detault */
  {
    script::person::register_api();

    engage();
    clear();

    detail::each_system([&](auto tag)
    {
      using T = decltype(tag);

      script::system<T>::eval(script::call_person_ctor<T>());
      expect_equal(out.str(), script::call_person_ctor_output<T>());
      this->clear();
    });
  }

  template <> template <>
  void script::ctor_group::test<1>() /* copy */
  {
    engage();
    clear();

    detail::each_system([&](auto tag)
    {
      using T = decltype(tag);

      script::system<T>::eval(script::call_person_ctor2<T>());
      expect_equal(out.str(), script::call_person_ctor2_output<T>());
      this->clear();
    });
  }

  template <> template <>
  void script::ctor_group::test<2>() /* unary */
  {
    engage();
    clear();

    detail::each_system([&](auto tag)
    {
      using T = decltype(tag);

      script::system<T>::eval(script::call_person_ctor_name<T>());
      expect_equal(out.str(), script::call_person_ctor_name_output<T>());
      this->clear();
    });
  }
}
