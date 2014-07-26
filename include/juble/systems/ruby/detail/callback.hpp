#pragma once

namespace script
{
  namespace ruby_detail
  {
    /* value_type func(value_type const self, value_type const arg...) */
    inline int callback_variadic(int const n)
    { return n; }
    /* value_type func(int const argc, value_type * const argv, value_type const self) */
    int constexpr const callback_argc{ -1 };
    /* value_type func(value_type const self, value_type const args_arr) */
    int constexpr const callback_array{ -2 };

    inline char const * current_method()
    {
      auto const callee(rb_funcall(rb_mKernel, rb_intern("__callee__"), 0));
      return RSTRING_PTR(rb_funcall(callee, rb_intern("to_s"), 0));
    }
  }
}
