#include <iostream>
#include <string>
#include <functional>
#include <memory>
#include <cstdint>

#include "types.hpp"
#include "system.hpp"
#include "registrar.hpp"

void say(std::string const &msg)
{ std::cout << msg << std::endl; }

class person
{
  public:
    person(std::string const &name, int16_t const age)
      : m_name(name), m_age(age)
    { }

    std::string const& get_name() const
    { return m_name; }
    void set_name(std::string const &name)
    { m_name = name; }
    int16_t get_age() const
    { return m_age; }

    bool alive{ true };

  private:
    std::string m_name;
    int16_t m_age{};
    static script::registration<person> const m_registration;
};

script::registration<person> const person::m_registration
{
  script::type<person>("person", "Example type representing a simple humanoid"),
  script::ctor<person (std::string const&, int16_t const)>("person", "Constructor taking a name and age"),
  script::ctor<person (person const&)>("person", "Copy constructor"),
  script::mem_func(&person::get_name, "get_name", "Accessor for name"),
  script::mem_func(&person::set_name, "set_name", "Mutator for name"),
  script::mem_func(&person::get_age, "get_age", "Acessor for age"),
  script::mem_var(&person::alive, "is_alive", "Whether or not the person is alive")
};

struct car
{
  enum class make
  { ford, honda };

  car() = default;
  car(car const&) = default;

  void drive()
  { std::cout << "vroom" << std::endl; }

  make const m_make{ make::ford };
  static script::registration<car> const m_registration;
};

script::registration<car> const car::m_registration
{
  script::type<car>("car",
      "An automotive vehicle"),
  script::ctor<car ()>("car",
      "Default constructor"),
  script::ctor<car (car const&)>("car", "Copy constructor"),
  script::mem_func(&car::drive, "drive", "Drives the car"),
  script::mem_var(&car::m_make, "make", "The make of the car")
};

int main()
{
  try
  {
    script::registrar::add(script::func(&say, "say"));
    script::registrar::describe();

    //script::system<script::chai>::use("src/scripts/chai/test.chai");
    //script::system<script::chai>::eval("say_hi();");

    std::cout << "----------------------------------" << std::endl;
    //script::system<script::documentation>::dump_data();
  }
  catch(std::exception const &e)
  { std::cerr << "exception: " << e.what() << std::endl; }
  catch(...)
  { std::cerr << "unknown exception" << std::endl; }
}
