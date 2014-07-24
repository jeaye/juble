#pragma once

#include <string>
#include <stdexcept>

#include <juble/detail/config.hpp>
#include <juble/detail/gen_config.hpp>
#include <juble/detail/assert.hpp>

namespace script
{
  /* Templated on each of the enabled systems. */
  template <typename... Systems>
  class registrar_impl final
  {
    public:
      static registrar_impl& get()
      {
        static registrar_impl r;
        return r;
      }

      static void show_enabled()
      {
        int const _[]
        {(
            std::cout << "enabled system: "
                      << system_traits<Systems>::name
                      << std::endl
            , 0)...
        };
        (void)_;
      }

      /* Recurse through each of the provided args. */
      static void add() {}
      template <typename T, typename... Args>
      static void add(T const &entry, Args &&... args)
      {
        dispatch<T, Systems...>(entry);
        add(std::forward<Args>(args)...);
      }
      template <typename T, typename... Args>
      static void add(global_var_impl<T> const &entry, Args &&... args)
      {
        dispatch_global<T, Systems...>(entry);
        add(std::forward<Args>(args)...);
      }

    private:
      /* Recurse through each of the enabled systems. */
      template <typename T>
      static void dispatch(T const &) {}
      template <typename T, typename System, typename... Ss>
      static void dispatch(T const &entry)
      {
        system<System>::get().add(entry);
        dispatch<T, Ss...>(entry);
      }
      template <typename T>
      static void dispatch_global(T const &) {}
      template <typename T, typename System, typename... Ss>
      static void dispatch_global(T const &entry)
      {
        system<System>::get().add_global(entry);
        dispatch_global<T, Ss...>(entry);
      }
  };
  using registrar = registrar_impl<JUBLE_SYSTEMS>;

  /* Use to register classes to the script systems.
   * [Hard] Limit only one per class. */
  template <typename T>
  struct registration final
  {
    template <typename... Args>
    registration(Args &&... args)
    {
      juble_assert(!created, 
        std::string{ "already registered script class: " } + typeid(T).name());
      created = true;

      registrar::add(std::forward<Args>(args)...);
    }

    registration(registration const &) = delete;
    registration(registration &&) = delete;
    registration& operator =(registration const&) = delete;

    static bool created;
  };
  template <typename T>
  bool registration<T>::created{ false };
}
