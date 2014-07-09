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
    explicit person(std::string const &name, int16_t const age)
      : name_{ name }, age_{ age }
    { }

    std::string const& get_name() const
    { return name_; }
    void set_name(std::string const &name)
    { name_ = name; }
    int16_t get_age() const
    { return age_; }

    bool alive{ true };

  private:
    std::string name_;
    int16_t age_{};
    static script::registration<person> const registration_;
};

script::registration<person> const person::registration_
{
  //script::type<person>("person"),
  //script::ctor<person (std::string const&, int16_t const)>("person"),
  //script::ctor<person (person const&)>("person"),
  //script::mem_func(&person::get_name, "get_name"),
  //script::mem_func(&person::set_name, "set_name"),
  //script::mem_func(&person::get_age, "get_age"),
  //script::mem_var(&person::alive, "is_alive")
};

struct car final
{
  enum class make
  { ford, honda };

  car() = default;
  car(car const&) = default;

  void drive()
  { std::cout << "vroom " << count_++ << std::endl; }

  int count_{};
  make const make_{ make::ford };
  static script::registration<car> const registration_;
};

script::registration<car> const car::registration_
{
  script::type<car>("Car"),
  //script::ctor<car ()>("car"),
  //script::ctor<car (car const&)>("car"),
  script::mem_func(&car::drive, "drive"),
  //script::mem_var(&car::make_, "make")
};

VALUE shout(VALUE)
{
  std::cout << "BARK" << std::endl;
  return Qnil;
}

int main()
{
  try
  {
    script::registrar::show_enabled();

    VALUE const c{ rb_define_class("Test", rb_cObject) };
    rb_define_method(c, "shout", reinterpret_cast<VALUE (*)(ANYARGS)>(&shout), 0);
    rb_eval_string("t = Test.new\nt.shout");

    script::registrar::add(script::func(&say, "say"));

    //script::system<script::chai>::use("src/scripts/chai/test.chai");
    //script::system<script::chai>::eval("say_hi();");

    script::ruby_system::eval("c = Car.new\nc.drive\nc.drive\nc.drive");

    std::cout << "----------------------------------" << std::endl;
    //script::system<script::documentation>::dump_data();
  }
  catch(std::exception const &e)
  { std::cerr << "exception: " << e.what() << std::endl; }
  catch(...)
  { std::cerr << "unknown exception" << std::endl; }
}
