@defgroup   pkg_mpaland-printf   mpaland's printf
@ingroup    pkg
@brief      mpaland's printf implementation is a lean, thread-safe and
            feature-complete printf implementation

# License

Licensed under the MIT license.

# Usage

Add `USEPKG += mpaland-printf` to the application's `Makefile` or compile
using `USEPKG=mpaland-printf make BOARD=<BOARD> -C <APP_DIR>`.

# Features

The package implements all standard format specifiers. However, support
for floating point is disabled by default due to the immense ROM overhead
on MCUs without FPU.

@note   Support for floating point formatting can be enabled
        via the `printf_float` module, i.e., by adding
        `USEMODULE += printf_float` to the application's `Makefile`.
@note   Support for non-standard format specifiers such as needed for
        printing from flash on AVR MCUs are not implemented.

@see    https://github.com/mpaland/printf
