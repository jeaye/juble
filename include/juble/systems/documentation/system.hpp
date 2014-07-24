#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <utility>

#include "geordi/type_strings.hpp"
#include <boost/algorithm/string.hpp>

#include <juble/system.hpp>
#include <juble/types.hpp>
#include <juble/detail/assert.hpp>

namespace script
{
  struct documentation final
  {};

  template <>
  class system<documentation> final
  {
    struct free_functions final
    { std::string const name; }; 

    public:
      static system& get()
      {
        static system s;
        static bool add_free_functions{ false };
        if(!add_free_functions)
        {
          add_free_functions = true;
          s.add(free_functions{ "free functions" });
        }

        return s;
      }

      template <template <typename...> class T, typename... Es>
      void add(T<Es...> const &entry)
      { object<T<Es...>>::get().add(entry); }
      template <typename S>
      void add(func_impl<S> const &entry)
      { object<free_functions>::get().add(entry); }
      template <typename S>
      void add(type<S> const &entry)
      { object<S>::get().add(entry); }
      template <typename S>
      void add(ctor<S> const &entry)
      { object<typename decltype(entry.func)::result_type>::get().add(entry); }
      template <typename T, typename... Ss>
      void add(mem_func_impl<T, Ss...> const &entry)
      { object<T>::get().add(entry); }
      template <typename T, typename S>
      void add(mem_var_impl<T, S> const &entry)
      { object<T>::get().add(entry); }
      template <typename S>
      void add(var_impl<S> const &)
      { }

      template <typename T>
      void add_global(T const &entry)
      { object<T>::get().add(entry); }

      static void dump_data()
      {
        for(auto const &it : get().objects_)
        { std::cout << it->to_string() << std::endl; }
      }

    private:
      void add(free_functions const &entry)
      { object<free_functions>::get().add(entry); }

      template <typename T>
      static std::string type_name()
      {
        std::stringstream ss;
        ss << TYPE<T>;

        std::string str{ ss.str() };
        boost::algorithm::replace_all(str,
        "std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >",
        "std::string");
        return str;
      }
      template <typename T>
      static std::string type_description()
      {
        std::stringstream ss;
        ss << TYPE_DESC<T>;
        return ss.str();
      }

      /* Use type erasure to hold onto all script objects. */
      struct concept
      {
        virtual ~concept() = default;
        virtual std::string to_string() const = 0;
      };
      template <typename T>
      class object final : public concept
      {
        public:
          static object& get()
          {
            static object o;
            return o;
          }

          template <template <typename...> class S, typename... Es>
          void add(S<Es...> const &)
          { juble_assert(false, "unknown documentation system entry"); }
          void add(free_functions const &entry)
          { 
            juble_assert(type_.first.empty(),
                         "type for free functions already specified");
            type_.first = entry.name;
          }
          template <typename S>
          void add(type<S> const &entry)
          {
            juble_assert(type_.first.empty(), "type already specified");
            type_.first = entry.name;
          }
          template <typename S>
          void add(ctor<S> const &entry)
          { ctors_.push_back({
              (entry.name + " [" +
              type_name<S>()
              + "]"),
              "" }); }
          template <typename S>
          void add(func_impl<S> const &)
          { }
          template <typename... Ss>
          void add(mem_func_impl<Ss...> const &entry)
          { mem_funcs_.push_back({
              (entry.name + " [" +
              type_name<typename std::remove_cv<decltype(entry.base_func)>::type>()
              + "]"),
              "" }); }
          template <typename... Ss>
          void add(mem_var_impl<Ss...> const &entry)
          { mem_vars_.push_back({
              (entry.name + " [" +
              type_name<typename std::remove_cv<decltype(entry.func)>::type>()
              + "]"),
              "" }); }
          template <typename S>
          void add(var_impl<S> const &)
          { }

          template <template <typename...> class S, typename E>
          void add_global(S<E> const &)
          { juble_assert(false, "unknown documentation global type"); }
          template <typename S>
          void add_global(global_var_impl<S> const &)
          { }

          std::string to_string() const override
          {
            std::stringstream ss;
            ss << type_.first << std::endl;

            if(ctors_.size())
            {
              ss << "\n  ctors " << "\n      ";
              for(auto const &it : ctors_)
              { ss << it.first << "\n      "; }
            }
            if(mem_funcs_.size())
            {
              ss << "\n  member functions\n      ";
              for(auto const &it : mem_funcs_)
              { ss << it.first << "\n      "; }
            }
            if(mem_vars_.size())
            {
              ss << "\n  member variables\n      ";
              for(auto const &it : mem_vars_)
              { ss << it.first << "\n      "; }
            }

            return ss.str();
          }

        private:
          object()
          { system<documentation>::get().objects_.push_back(this); }

          std::pair<std::string, std::string> type_;
          std::vector<std::pair<std::string, std::string>> ctors_;
          std::vector<std::pair<std::string, std::string>> mem_funcs_;
          std::vector<std::pair<std::string, std::string>> mem_vars_;
      };

      std::vector<concept*> objects_;
  };
  template <>
  struct system_traits<documentation> final
  {
    static char constexpr const * const name{ "documentation" };
  };
}
