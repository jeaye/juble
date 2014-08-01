juble (C++14 embedded scripting framework)
---

**NOTE**: juble is not production (or even side-project) ready.

juble is a header-only C++ embedded scripting framework that uses a templated approach to registering C++ APIs to scripting languages like Lua, Ruby, Chaiscript, Python, et cetera. Using one generic registration, any number of different scripting languages can learn everything about your C++ types, create objects with those types, call member functions, manipulate variables, et cetera.

There is no need to limit yourself to a single scripting language when you're, for example, providing a way for people to write mods or custom logic. Just expose your API to juble and your API will work for all enabled scripting systems.

#### Features:
  * Type-safe C++14 API
  * Header-only
  * Support for:
    * Chaiscript
    * Ruby
  * Extensive test suite (using [jest](https://github.com/jeaye/jest), a "sane and minimal C++14 unit test framework")

#### Sample:
```cpp
#include <juble/juble.hpp>

struct car final
{
  enum class make
  { ford, honda };

  car() = default;
  car(car const&) = default;

  /* Nomenclature is not required, just register after main() is entered. */
  static void register_api()
  {
    script::registration<car> const registration
    {
      script::type<car>("Car"),
      script::ctor<car ()>("new"), /* Some languages without ctors will use these names. */
      script::ctor<car (car const&)>("new_copy"),
      script::mem_func(&car::drive, "drive"),
      script::mem_var(&car::make_, "make")
    };
  }

  void drive()
  { std::cout << "vroom " << std::endl; }

  make const make_{ make::ford };
};

int main()
{
  car::register_api();
  script::chai_system::use("foo.chai");
  script::ruby_system::eval("c = Car.new \n c.drive");
}
```

### Configuration
For the most up-to-date information, see `./configure --help`. You can disable certain language systems directly, so their dependencies will not be resolved and support will not be provided. For example: `./configure --disable-chai`

### Installation
Since juble is a header-only library, simply copy over the contents of `include` to your project, or, better yet, add juble as a submodule and introduce `juble/include` to your header search paths.  

Full installation can also be achieved by using `./configure && make install`. See the `configure` script for prefix options.  
