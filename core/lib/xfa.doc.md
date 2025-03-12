@defgroup  core_util_xfa Cross-File Arrays (XFA)
@ingroup   core_util
@brief     Define static array across compilation units
@author    Kaspar Schleiser <kaspar@schleiser.de>
@author    carl-tud

Cross-File array allow you to define arrays spread over multiple C compilation/translation
units (i.e., files).

@experimental This API is considered experimental and will probably change without notice!

## Using Cross-File Arrays
First, you need to decide whether you want your cross-file array to be read-only. If this is your choice, use
replace the following macros with @ref XFA_INIT_CONST, @ref XFA_USE_CONST, and @ref XFA_CONST.

```c
// main.c
#include "xfa.h"
XFA_INIT(int, favorite_numbers);
```
In the `main.c` translation unit above, we initialize a new cross-file array called `favorite_numbers` with an
element type of `int` using @ref XFA_INIT.
To acess array elements, retrieve the array's count and access elements by subscript:

```c
#include "xfa.h"
int main(void) {
    size_t count = XFA_LEN(int, favorite_numbers);
    for (size_t i = 0; i < count; i += 1) {
        printf("%i belongs to my favorite numbers\n", favorite_numbers[i]);
    }
}
```

To access elements in a file other then the file you initialized the array in, you use @ref XFA_USE. Think of this macro
as the `extern` version of @ref XFA_INIT.
```c
// someplace.c
#include "xfa.h"
XFA_USE(int, favorite_numbers);
```
Now you yould access elements like in the `main` function above.

To add element, use @ref XFA. To add numbers, @ref XFA_INIT and @ref XFA_USE do not need to be present in the same
file.

```c
// best_number.c
#include "xfa.h"
XFA(int, favorite_numbers, 1, best) = 73;
```

### Compatibility Note

@remark Previously, the standard way to add an element was `XFA(int, favorite_numbers, 1) best = ...`.
You can still define array elements this way. However, **if you do want to support macOS, you must include the element
name in the XFA invocation**: `XFA(int, favorite_numbers, 1, best) = ...`. **We encourage you to use the
new technique for new applications**.


If you try to add an element on macOS and do not include the element name in the invocation, i.e., use the old technique,
the following message will be emitted when trying to build.

    warning: Element of cross-file array 'favorite_numbers' must
             have an identifier (4th argument) on macOS. [-W#pragma-messages]

## Under the Hood
XFAs are implemented differently, depending on whether you want to produce an ELF binary (on GNU/Linux) or Mach-O file
(on macOS). Both implementations share the same fundemental design: Elements of a cross-file array are placed in a specific
section of the resulting binary. Elements are placed contiguously, with the array pointer referencing the start of the first
element. Under the hood, the array element count is calculated using an end pointer to the last element. The array's
element count can then be calculated using the formula `(end - start) / sizeof(element)`.

Whenever you initialize a new cross-file array, the **start and end marker** are also declared.
The **start marker** is the same as the cross-file array pointer.

For example,

```c
XFA_INIT(int, favorite_numbers)
```

declares `int* favorite_numbers` (start marker = array pointer)
and `int* favorite_numbers_end` (end marker). @ref XFA_LEN takes these and
computes the element count using the formula introduced above.

This is what a cross-file array with 6 elements looks like in memory:

           +----+----+----+----+----+----+ - - - +
           | #0 | #1 | #2 | #3 | #4 | #5 |       |
           +----+----+----+----+----+----+ - - - +
           ^                             ^ 
    favorite_numbers            favorite_numbers_end
          ==

### ELF
On GNU/Linux, `gcc` or `clang` produce an ELF (_Executable and Linkable Format_) binary.
When you define XFA elements, these get inserted into a temporary ELF section with its name starting with either `.xfa.`
or `.roxfa`, depending on whether you defined a read-only array. Each element gets its own temporary section.
The start and end of the array are marked by a zero-width array.

Let's look at the fictional `favorite_numbers` XFA definition from above.

```c
XFA_INIT(int, favorite_numbers)
```
The initializer creates two zero-width symbols placed in these sections

    .xfa.favorite_numbers.0   <- favorite_numbers     (zero-width start marker)
    .xfa.favorite_numbers.9   <- favorite_numbers_end (zero-width end marker)

You may have already noticed the numbers in the section name. These _priorities_ are used to make sure
- (a) elements are placed between the start and end marker and
- (b) to allow developers to sort elements.

For example, consider your favorite numbers are 42, 73, and `0xff` and you define them in different files.
```c
// In sheldon.c, we define best = 73
XFA(int, favorite_numbers, 1, best) = 73;

// In universe.c, we define answer = 42
XFA(int, favorite_numbers, 0, answer) = 42;

// In memory.c, we define byte = 0xff
XFA(int, favorite_numbers, 3, byte) = 0xff;
```
Notice the priorities ensuring your favorite numbers are inserted in ascending order. 
These are the ELF sections generated using `__attribute__((section(".xfa...."))`:

    .xfa.favorite_numbers.0_   <- favorite_numbers     (zero-width start marker)
    .xfa.favorite_numbers.5_1  -- best   (73)
    .xfa.favorite_numbers.5_0  -- answer (42)
    .xfa.favorite_numbers.5_3  -- byte   (0xff)
    .xfa.favorite_numbers.9_   <- favorite_numbers_end (zero-width end marker)

The linker script (`xfa.ld`) will sort these sections and integrate them into the `.data` section. 
If we would have used
@ref XFA_INIT_CONST and @ref XFA_CONST, these would have been inserted in the `.rodata` section.

Sorting works since element priorities are prepended with `5_`, guaranteeing they don't interfere 
with the start/stop marker.

### Mach-O
On macOS, you create a Mach-O binary using `clang` (the name stems from the _Mach_ operating system).
The XFA implementation on macOS is more complex due to the macOS linker lacking support for linker scripts and 
the `section("...")` attribute imposing a limit on the section's name length. Furthermore,
it is not possible to create a zero-width symbol.

On macOS, we assign each XFA element an a symbol alias using the `redefine_extname` pragma. 
For example, `XFA(int, favorite_numbers, 1, best)` becomes `__xfa__$favorite_numbers$5_1$best`. 
Symbol aliases generally follow the following naming scheme:
   
    [__xfa__|__roxfa__]$<xfa_name>$<prio>$<element_name>

Let's look at the fictional `favorite_numbers` XFA definition from above.

```c
XFA_INIT(int, favorite_numbers)
```
The initializer assigns the start marker/array pointer (`favorite_numbers`) and 
end marker (`favorite_numbers_end`) the following aliases:

    __xfa__$favorite_numbers$0$__start__    ==  favorite_numbers     (start marker)
    __xfa__$favorite_numbers$9$__end__      ==  favorite_numbers_end (end marker)

Given the example from above, 
```c
XFA_INIT(int, favorite_numbers)

XFA(int, favorite_numbers, 1, best) = 73;

XFA(int, favorite_numbers, 0, answer) = 42;

XFA(int, favorite_numbers, 3, byte) = 0xff;
```

the following symbol aliases would be generated for the lements:

    __xfa__$favorite_numbers$5_1$best    ==  best
    __xfa__$favorite_numbers$5_0$answer  ==  answer
    __xfa__$favorite_numbers$5_3$byte    ==  byte

After building all objects, i.e., before linking, we extract all of these XFA symbol aliases using
`nm -g` and sort that list. This ensures the priorities are respected.
Before invoking the linker, we also generate generate the start marker by defining another alias
to the first array element in the sorted list. This is necessary as there are no zero-width
symbols. The first array element's' address must be the same as the array itself. 
In the example from above, `favorite_numbers` would point to `answer`, the first element.
This works as follow.

After retrieving the first array element from the list of XFA symbols, we generate
an assembly file that defines an alias for the array itself (the start marker). In the fictional
case above this would cause the following assembly to be generated:

    .globl __xfa__$favorite_numbers$0$__start__
    __xfa__$favorite_numbers$0$__start__ = __xfa__$__xfa__$favorite_numbers$5_0$answer

(Note that it is not possible to use the `redefine_extname` pragma to define the start marker
as that would require knowing which element is the first beforehand. We only know that once
all objects have been created.)

In summary, this is what the list of symbols looks like:

    __xfa__$favorite_numbers$0$__start__
    __xfa__$favorite_numbers$5_0$answer
    __xfa__$favorite_numbers$5_1$best
    __xfa__$favorite_numbers$5_2$byte
    __xfa__$favorite_numbers$9$__end__

To summarize, these steps are necessary to support XFA on macOS:

1. When invoking @ref XFA_INIT, @ref XFA_USE, and @ref XFA, define aliases 
   - for the start marker/array pointer: `__xfa...$__start__ = favorite_numbers`
   - for the end marker: `__xfa...$__end__ = favorite_numbers_end`
   - for each element: `__xfa...$element = element`
2. After objects have been build, extract all symbol starting with `__xfa`
3. Sort this list
4. Take all start markers and make them point to the first element in the sorted list:
    `__xfa...$__start__ = favorite_numbers = __xfa...$first_element_name`.
    This is done in assembly.
5. Pass the order file to the linker and link.

@note This logic is implemented in `makefiles/arch/native.inc.mk` in the `xfa-darwin` recipe.
