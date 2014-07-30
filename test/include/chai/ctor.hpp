#pragma once

namespace script
{
  template <>
  char constexpr const * call_person_ctor<chai_tag>()
  { return "var p = Person();"; }
  template <>
  char constexpr const * call_person_ctor_output<chai_tag>()
  { return "person()person(person const&)"; }
  template <>
  char constexpr const * call_person_ctor2<chai_tag>()
  { return "var p2 = Person(p);"; }
  template <>
  char constexpr const * call_person_ctor2_output<chai_tag>()
  { return "person(person const&)person(person const&)"; }
  template <>
  char constexpr const * call_person_ctor_name<chai_tag>()
  { return "var pjerry = Person(\"jerry\");"; }
  template <>
  char constexpr const * call_person_ctor_name_output<chai_tag>()
  { return "person(jerry)person(person const&)jerry"; }
}
