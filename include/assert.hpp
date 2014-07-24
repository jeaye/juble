#pragma once

#include <cassert>
#include <stdexcept>

#define juble_assert_failure(msg) \
  std::cerr << "assertion failure: " << (msg) << std::endl; \
  assert(false)
//#define juble_assert_failure(msg) throw std::runtime_error{ msg }
#define juble_assert(test, msg) \
  if(!(test))\
  { juble_assert_failure(msg); }
