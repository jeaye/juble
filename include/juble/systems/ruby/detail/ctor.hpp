#pragma once

namespace script
{
  namespace ruby
  {
    template <typename F>
    class ctor_wrapper;
    template <typename Class, typename... Args>
    class ctor_wrapper<Class (Args...)> final
    {
      public:
        ctor_wrapper() = delete;
        ctor_wrapper(std::function<Class* (Args...)> const &func)
        { func_ = func; }

        static value_type call(int const argc, value_type * const argv, value_type const self)
        {
          using uptr_t = std::unique_ptr<detail::bare_t<Class>>;

          juble_assert(func_, "invalid ctor call");
          juble_assert(sizeof...(Args) == argc, "invalid argument count");

          auto * const data(new uptr_t
          { call_impl(std::index_sequence_for<Args...>{}, argv) });

          auto const deleter([](void * const data)
          { delete static_cast<uptr_t*>(data); });

          value_type const obj{ Data_Wrap_Struct(self, nullptr, deleter, data) };
          return obj;
        }

      private:
        template <size_t... Ns>
        static Class* call_impl(std::index_sequence<Ns...> const,
                                value_type * const argv)
        {
          (void)argv; /* XXX: When Ns is 0, argv is not used. */
          return func_(from_ruby<Args>(argv[Ns])...);
        }

        static std::function<Class* (Args...)> func_;
    };
    template <typename Class, typename... Args>
    std::function<Class* (Args...)> ctor_wrapper<Class (Args...)>::func_;
  }
}
