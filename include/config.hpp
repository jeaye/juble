#pragma once

#include "gen_config.hpp"
#include "system.hpp"

#if JUBLE_CHAI
  #include "systems/chai/system.hpp"
#endif
#if JUBLE_RUBY
  #include "systems/ruby/system.hpp"
#endif
#if JUBLE_DOCUMENTATION
  #include "systems/documentation/system.hpp"
#endif

/* Verify we have at least one system. */
#if !JUBLE_CHAI && !JUBLE_RUBY && !JUBLE_DOCUMENTATION
  #error "no valid systems enabled"
#endif
