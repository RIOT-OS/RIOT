# Build System Basics                                       {#build-system-basics}

# Variables declaration guidelines              {#variable-declaration-guidelines}

This page contains basic guidelines about `make` variable declaration, it
summarizes some of the pros and cons as well as specifies good and bad patterns
in our build system. You might want to refer to `gnu make` documentation
regarding these subjects.

## Avoid unnecessary export

```
export OUTPUT = $(shell some-command)
```

Exporting a variable means it will be evaluated on every `target` call, which
slows down the build system. Always avoid exporting a variable if unneeded.

If an export is actually needed by a `sub-make` then export the variable only for
the needed targets using `target-export-variables` (more in
`makefiles/utils/variables.mk`).

Exported variables ("global variable") are hard to remove, specially when badly
documented. If no one knows why it's there and no one knows where it can be used
then no one knows if it's safe to remove since it's present for every target.
This is why global variables need clear documentation.

[gnumake doc](https://www.gnu.org/software/make/manual/html_node/Variables_002fRecursion.html)

## Use memoized for variables referencing a function or command

### recursively expanded variable:

~~~~~~~~~~~~~~~~
OUTPUT = $(shell some-command $(ANOTHER_VARIABLE))
~~~~~~~~~~~~~~~~

- When using `=` the value of the variable is only declared, but not set,
  therefore the variable will only be evaluated when expanded (used) somewhere
  in the makefile. If `$(OUTPUT)` is never expanded, `some-command`
  is never executed and `ANOTHER_VARIABLE` not expanded.

- All variables or functions referenced by the declared variable will will be
  evaluated every time the variable is expanded.
  In the example `some-command` is executed every time `OUTPUT` is expanded, same for
  `ANOTHER_VARIABLE`. If `some-command` is slow this introduced unneeded overhead.

- If the variable expansion doesn't involve evaluating a function the overhead
  is none.

### simply expanded variable:

~~~~~~~~~~~~~~~~
OUTPUT := $(shell some-command $(ANOTHER_VARIABLE))
~~~~~~~~~~~~~~~~

- When using `:=` the value is only expanded once, expanding any reference to
  other variables or functions. If `OUTPUT` is always used at least once and
  evaluates a costly function (`some command`) then use `:=`.

- When using `:=` the variable will be evaluated even if not needed, which
  introduces unnecessary delay, in particular `some command` or functions
  evaluated by `ANOTHER_VARIABLE` are slow.
  It can also cause a failure in a worst-case scenario (think what happens if a
  tool is defined with `:=` but you don't have the tool and you don't need it either).

- The values of variables declared with `:=` depend on the order of definition.

### memoized:

~~~~~~~~~~~~~~~~
OUTPUT = $(call memoized,OUTPUT,$(shell some-command))
~~~~~~~~~~~~~~~~

- `memoized` is a RIOT defined function that combines characteristics from
  both `=` and `:=`.
  The variable expansion will be deferred until its first usage, but further
  usage will consider it as a simply expanded variable, so it will use the already
  evaluated value. In the example `some-command` would be executed once or not
  at all (more in `makefiles/utils/variables.mk`).

[gnumake doc](https://www.gnu.org/software/make/manual/html_node/Flavors.html)

## Additional documentation

- Deferred vs. simple expansion: http://make.mad-scientist.net/deferred-simple-variable-expansion/
- Tracking issue: [#10850](https://github.com/RIOT-OS/RIOT/issues/10850)
