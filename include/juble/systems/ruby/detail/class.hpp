#pragma once

namespace script
{
  namespace ruby
  {
    template <typename T>
    value_type value_class(std::string const &name)
    {
      static value_type const value{ rb_define_class(name.c_str(), rb_cObject) };

      rb_undef_method(value, "initialize");
      auto const init(static_cast<ruby::unary_func_t>(
      [](ruby::value_type const self)
      { return self; }));
      rb_define_method(value, "initialize",
              reinterpret_cast<ruby::any_func_t>(init), 0);
      return value;
    }
  }
}
