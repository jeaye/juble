#pragma once

namespace script
{
  namespace detail
  {
    template <typename T>
    std::string& type_name()
    {
      static std::string name;
      return name;
    }
    template <typename T>
    std::string& type_name(std::string const &str)
    { return type_name<T>() = str; }
  }
}
