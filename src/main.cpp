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

class person final
{
  public:
    person() = delete;
    person(person const&) = default;
    person(person &p)
      : name_{ p.name_ + "2" }, age_{ p.age_ }
    { p.name_ += " (dead)"; p.age_ = 0; }
    explicit person(int16_t const age)
      : name_{ "Jeaye" }, age_{ age }
    { }
    explicit person(std::string const &name, int16_t const age)
      : name_{ name }, age_{ age }
    { }

    static void register_api()
    {
      script::registration<person> const registration
      {
        script::type<person>("Person"),
        script::ctor<person (int16_t const)>("new_age"),
        script::ctor<person (std::string const&, int16_t const)>("new_shit"),
        script::ctor<person (person &)>("new_copy"),
        script::mem_func(&person::get_name, "get_name"),
        script::mem_func(&person::set_name, "set_name"),
        script::mem_func(&person::get_age, "get_age"),
        script::mem_func(&person::talk, "talk"),
        script::mem_var(&person::alive, "is_alive")
      };
    }

    void talk() const
    { std::cout << name_ << ": " << age_ << std::endl; }

    std::string const& get_name() const
    { return name_; }
    void set_name(std::string const &name) /* TODO: function params */
    { name_ = name; }
    int16_t get_age() const /* TODO: function return types. */
    { return age_; }

    bool alive{ true }; /* TODO: member variables */

  private:
    std::string name_;
    int16_t age_{};
};

struct car final
{
  enum class make
  { ford, honda };

  car() = default;
  car(car const&) = default;

  static void register_api()
  {
    script::registration<car> const registration
    {
      script::type<car>("Car"),
      script::ctor<car ()>("new"),
      script::ctor<car (car const&)>("new_copy"),
      script::mem_func(&car::drive, "drive"),
      script::mem_var(&car::make_, "make")
    };
  }

  void drive()
  { std::cout << "vroom " << count_++ << std::endl; }

  int count_{};
  make const make_{ make::ford };
};

int main()
{
  try
  {
    script::registrar::show_enabled();

    person::register_api();
    car::register_api();

    script::registrar::add(script::func(&say, "say"));

    //script::system<script::chai>::use("src/scripts/chai/test.chai");
    //script::system<script::chai>::eval("say_hi();");

    script::ruby_system::eval("c = Car.new\nc.drive\nc.drive\nc.drive");
    script::ruby_system::eval("p = Person.new_age(22)\np.talk");
    script::ruby_system::eval("p = Person.new_shit('Bob', -13)\np.talk");
    script::ruby_system::eval("\np = Person.new_shit('Gary', 41)\np.talk\np2 = Person.new_copy(p)\np2.talk\np.talk");

    std::cout << "----------------------------------" << std::endl;
    //script::system<script::documentation>::dump_data();
  }
  catch(std::exception const &e)
  { std::cerr << "exception: " << e.what() << std::endl; }
  catch(...)
  { std::cerr << "unknown exception" << std::endl; }
}
