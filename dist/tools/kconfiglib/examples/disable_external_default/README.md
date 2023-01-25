# Externally defaulted modules that can be disabled by other modules in Kconfig

This is a pattern for modules that may be conditionally selected by
default by a different module and disabled by another module.  Similar
to the `DEFAULT_MODULE` and `DISABLE_MODULE` functionality in the
current make build system.

### Alternatives

If the module is visible with an application or a board disabling
`default y` without any conditions then it may be better to use the
`*.config` file.

If the module only needs to be deselected by ONE external module one can
use an `imply * if` pattern.  This will not scale and will require
knowledge of all external modules very quickly though.

If the module is simple and will stay simple it may be better to just
use a config override. This is also position dependant meaning overrides
should follow the application, board, cpu priorities and not be done
outside of that. When overriding `config` symbols, all dependencies must
be copied as they will weaken. Changing kconfig to not weaken `config`
dependencies will result in nothing working if the full tree is loaded
as a `BOARD` configs will ultimately prevent any boards...

## Reasoning

Disabling defaulted modules cannot easily be done within Kconfig, only
via `menuconfig` or the `*.config` files. Using the `DISABLE` and
`DEFAULT` conventions allow the module to be isolated from any other
module knowledge.

### Use case

The use case is for something like having `auto_init_xtimer` being
selected by defauby other modulelt but needing to be disabled if
`ztimer_xtimer_compat` is used.

## Usage

To view how it operates we can directly invoke the `menuconfig` from
this directory.

This will show the basic operation without anything selected.

```
./../../riot_menuconfig.py
```

### Module enabling `FOO` by default

```
KCONFIG_CONFIG=app.default.config ./../../riot_menuconfig.py
```


### Module disabling the default enabling of `FOO`

```
KCONFIG_CONFIG=app.disable_default.config ./../../riot_menuconfig.py
```

## Other tooling

Macro and templates are used to enforce the `DEFAULT_*` and `DISABLE_*`
convention.
