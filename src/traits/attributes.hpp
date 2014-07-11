#pragma once

#include <type_traits>

namespace script
{
  namespace detail
  {
    template <typename T>
    struct bare
    { using type = std::remove_cv_t<std::remove_reference_t<std::remove_pointer_t<T>>>; };
    template <typename T>
    using bare_t = typename bare<T>::type;
  }
}
