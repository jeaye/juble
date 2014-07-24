#pragma once

#include <juble/detail/gen_config.hpp>
#include <juble/system.hpp>

#if JUBLE_CHAI
  #include <juble/systems/chai/system.hpp>
#endif
#if JUBLE_RUBY
  #include <juble/systems/ruby/system.hpp>
#endif
#if JUBLE_DOCUMENTATION
  #include <juble/systems/documentation/system.hpp>
#endif

/* Verify we have at least one system. */
#if !JUBLE_CHAI && !JUBLE_RUBY && !JUBLE_DOCUMENTATION
  #error "no valid systems enabled"
#endif
