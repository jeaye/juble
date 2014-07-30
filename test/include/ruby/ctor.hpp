#pragma once

namespace script
{
  template <>
  char constexpr const * call_person_ctor<ruby_tag>()
  { return "p = Person.new"; }
  template <>
  char constexpr const * call_person_ctor2<ruby_tag>()
  { return "p = Person.new\np2 = Person.new_copy(p);"; }
  template <>
  char constexpr const * call_person_ctor_name<ruby_tag>()
  { return "pjerry = Person.new_name(\"jerry\");"; }
}
