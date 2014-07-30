#include <iostream>
#include <string>
#include <functional>
#include <memory>
#include <cstdint>

#include <juble/juble.hpp>
#include <jest/jest.hpp>

#include "generic/ctor.hpp"
#include "generic/non_member.hpp"

int main()
{
  script::registrar::show_enabled();
  std::cerr << std::endl;

  jest::worker const j{};
  return j();
}
