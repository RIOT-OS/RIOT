You should check out the [C Conventions](CODING_CONVENTIONS.md) as some section
still apply (Documentation, Git, Travis).

When contributing source code, please adhere to the following coding style,
which is loosely based on the [Google C++ Style Guide] and the coding
 conventions used by the C++ Standard Library. Based on the [CAF coding style].

[Google C++ Style Guide]: (https://google.github.io/styleguide/cppguide.html)
[CAF coding style]: (https://github.com/actor-framework/actor-framework/blob/master/CONTRIBUTING.md)

## Example for the Impatient

```cpp
// module/riot/example/my_class.hpp

#ifndef RIOT_EXAMPLE_MY_CLASS_HPP
#define RIOT_EXAMPLE_MY_CLASS_HPP

#include <string>

// use "//" for regular comments and "///" for doxygen

namespace riot {
namespace example {

/// This class is only being used as style guide example.
class my_class {
public:
    /// Brief description. More description. Note that RIOT uses the
    /// "JavaDoc-style" autobrief option, i.e., everything up until the
    /// first dot is the brief description.
    my_class();

    /// Destructs `my_class`. Please use Markdown in comments.
    ~my_class();

    // suppress redundant @return if you start the brief description with "Returns"
    /// Returns the name of this instance.
    inline const std::string& name() const {
        return name_;
    }

    /// Sets the name of this instance.
    inline void name(const std::string& new_name) {
        name_ = new_name;
    }

    /// Prints the name to `STDIN`.
    void print_name() const;

    /// Does something (maybe).
    void do_something();

    /// Does something else.
    void do_something_else();

private:
    std::string name_;
};

} // namespace example
} // namespace riot

#endif // RIOT_EXAMPLE_MY_CLASS_HPP
```

```cpp
// my_module/my_class.cpp

#include "riot/example/my_class.hpp"

#include <iostream>

namespace riot {
namespace example {

namespace {

constexpr const char default_name[] = "my object";

} // namespace <anonymous>

my_class::my_class() : name_(default_name) {
    // nop
}

my_class::~my_class() {
    // nop
}

void my_class::print_name() const {
    std::cout << name() << std::endl;
}

void my_class::do_something() {
    if (name() == default_name) {
        std::cout << "You didn't gave me a proper name, so I "
                  << "refuse to do something."
                  << std::endl;
    } else {
        std::cout << "You gave me the name " << name()
                  << "... Do you really think I'm willing to do something "
                     "for you after insulting me like that?"
                  << std::endl;
    }
}

void my_class::do_something_else() {
    switch (default_name[0]) {
        case 'a':
            // handle a
            break;
        case 'b':
            // handle b
            break;
        default:
          handle_default();
    }
}

} // namespace example
} // namespace riot

```

## General

- Use 4 spaces per indentation level.

- The maximum number of characters per line is 80.

- No tabs, ever.

- Never use C-style casts.

- Vertical whitespaces separate functions and are not used inside functions:
  use comments to document logical blocks.

- Header filenames end in `.hpp`, implementation files end in `.cpp`.

- Never declare more than one variable per line.

- `*` and `&` bind to the *type*, e.g., `const std::string& arg`.

- Namespaces and access modifiers (e.g., `public`) do not increase the
  indentation level.

- In a class, use the order `public`, `protected`, and then `private`.

- Always use `auto` to declare a variable unless you cannot initialize it
  immediately or if you actually want a type conversion. In the latter case,
  provide a comment why this conversion is necessary.

- Never use unwrapped, manual resource management such as `new` and `delete`.

- Never use `typedef`; always write `using T = X` in favor of `typedef X T`.

- Keywords are always followed by a whitespace: `if (...)`, `template <...>`,
  `while (...)`, etc.

- Leave a whitespace after `!` to make negations easily recognizable:

  ```cpp
  if (! sunny())
      stay_home();
  else
      go_outside();
  ```

- Opening braces belong to the same line:

  ```cpp
  void foo() {
      // ...
  }
  ```

- Use standard order for readability: C standard libraries, C++ standard
  libraries, other libraries, (your) RIOT headers:

  ```cpp
  // some .hpp file

  #include <sys/types.h>

  #include <vector>

  #include "3rd/party.h"

  #include "riot/fwd.hpp"
  ```

  RIOT includes should always be in doublequotes, whereas system-wide
  includes in angle brackets. In a `.cpp` file, the implemented header always
  comes first and the header `riot/config.hpp` can be included second if you
  need platform-dependent headers.

- When declaring a function, the order of parameters is: outputs, then inputs.
  This follows the parameter order from the STL.

- Protect single-argument constructors with `explicit` to avoid implicit
  conversions.

## Naming

- All names except macros and template parameters should be
  lower case and delimited by underscores.

- Template parameter names should be written in CamelCase.

- Types and variables should be nouns, while functions performing an action
  should be "command" verbs. Classes used to implement metaprogramming
  functions also should use verbs, e.g., `remove_const`.

- Private and protected member variables use the suffix `_` while getter *and*
  setter functions use the name without suffix:

  ```cpp
  class person {
  public:
      const std::string& name() const {
          return name_
      }

      void name(const std::string& new_name) {
          name_ = new_name;
      }

  private:
      std::string name_;
  };
  ```

- Use `T` for generic, unconstrained template parameters and `x`
  for generic function arguments. Suffix both with `s` for
  template parameter packs:

  ```cpp
  template <class... Ts>
  void print(const Ts&... xs) {
      // ...
  }
  ```


## Headers

- Each `.cpp` file has an associated `.hpp` file.
  Exceptions to this rule are unit tests and `main.cpp` files.

- Each class has its own pair of header and implementation
  files and the relative path for the header file is derived from its full name.
  For example, the header file for `riot::example::my_class` of module
  `my_module` is located at `path/to/my_module/riot/example/my_class.hpp`
  and the source file at `path/to/my_module/my_class.cpp`.

- All header files should use `#define` guards to prevent multiple inclusion.
  The symbol name is `<RELATIVE>_<PATH>_<TO>_<FILE>_HPP`.

- Do not `#include` when a forward declaration suffices.

- Each library component must provide a `fwd.hpp` header providing forward
  declarations for all types used in the user API.

- Each library component must provide an `all.hpp` header that contains the
  main page for the documentation and includes all headers for the user API.

- Use `inline` for small functions (rule of thumb: 10 lines or less).


## Breaking Statements

- Break constructor initializers after the comma, use four spaces for
  indentation, and place each initializer on its own line (unless you don't
  need to break at all):

  ```cpp
  my_class::my_class()
      : my_base_class(some_function()),
        greeting_("Hello there! This is my_class!"),
        some_bool_flag_(false) {
      // ok
  }
  other_class::other_class() : name_("tommy"), buddy_("michael") {
      // ok
  }
  ```

- Break function arguments after the comma for both declaration and invocation:

  ```cpp
  intptr_t channel::compare(const abstract_channel* lhs,
                            const abstract_channel* rhs) {
      // ...
  }
  ```

- Break before tenary operators and before binary operators:

  ```cpp
  if (today_is_a_sunny_day()
      && it_is_not_too_hot_to_go_swimming()) {
      // ...
  }
  ```


## Template Metaprogramming

Despite its power, template metaprogramming came to the language pretty
much by accident. Templates were never meant to be used for compile-time
algorithms and type transformations. This is why C++ punishes metaprogramming
with an insane amount of syntax noise. In RIOT, we make excessive use of
templates. To keep the code readable despite all the syntax noise, we have some
extra rules for formatting metaprogramming code.

- Brake `using name = ...` statements always directly after `=` if it
  does not fit in one line.

- Consider the *semantics* of a metaprogramming function. For example,
  `std::conditional` is an if-then-else construct. Hence, place the if-clause
  on its own line and do the same for the two cases.

- Use one level of indentation per "open" template and place the closing `>`,
  `>::type` or `>::value` on its own line. For example:

  ```cpp
  using optional_result_type =
      typename std::conditional<
          std::is_same<result_type, void>::value,
          bool,
          optional<result_type>
      >::type;
  // think of it as the following (not valid C++):
  auto optional_result_type =
      conditional {
          if   result_type == void
          then bool
          else optional<result_type>
      };
  ```

- Note that this is not necessary when simply defining a type alias.
  When dealing with "ordinary" templates, indenting based on the position of
  the opening `<` is ok, e.g.:

  ```cpp
  using response_handle_type = response_handle<Subtype, message,
                                               ResponseHandleTag>;
  ```


## Preprocessor Macros

- Use macros if and only if you can't get the same result by using inline
  functions or proper constants.

- Macro names use the form `RIOT_<COMPONENT>_<NAME>`.


## Comments

- Doxygen comments start with `///`.

- Use Markdown instead of Doxygen formatters.

- Use `@cmd` rather than `\cmd`.

- Use `//` to define basic comments that should not be
  swallowed by Doxygen.
