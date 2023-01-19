# Select a specific choice implementation from a module

This is a design pattern when one must select a specific implementation
from a module. This is specifically for modules that are not defined in
 boards, cpus, or applications as choice overrides would be the
preferred method.

## Reasoning

We need a choice to ensure mutual exclusive options. The issue is that we need
to detect if multiple modules `REQUIRES_*` different choices. If that happens
then it will select the first fitting choice in the list but we can use the
`ERROR_*` convention to enforce the correct one is being used. The example also
allows for auto-selection based on the `HAS_*` convention.

Note that selecting specific backends is not recommended or desired.
Modules should select only the generic module and using the `HAS_*`
instead. This is only to cover the corner cases which a generic module
and preference is not possible.

### Use case

The use case is for something like having `MODULE_STDIO` selecting a specific
`USB` implementation because it `REQUIRES_CDC_ACM` feature that may not exist
in all `USB` implementations.

## Usage

To view how it operates we can directly invoke the `menuconfig` from this
directory.

This will show the basic operation without anything selected.

```
./../../riot_menuconfig.py
```

### Pre-selected automatic selection based on `HAS_*`

```
KCONFIG_CONFIG=app.auto.config ./../../riot_menuconfig.py
```

### Pre-selected the `HAS_*` being overridden by the `MODULE_*` requirements

```
KCONFIG_CONFIG=app.has_override.config ./../../riot_menuconfig.py
```

### Pre-selected an error condition having a module select impossible requirements

```
KCONFIG_CONFIG=error.config ./../../riot_menuconfig.py
```

## Other tooling

The `Kconfig.generated` shows the actual design pattern and is generated with
the `generate_example.py` script.

For more details, try:
```
generate_example.py -h
```

## FAQs

### Why can't I just select the choice option from Kconfig?

This is a "feature" of Kconfig, problems would then occur when multiple
choice options are selected since a choice can only select one.
Selecting a `choice` option from Kconfig will generate an error. It is
possible to select one `choice` setting from the `app.config` settings
though. Selecting multiple `choice` settings from the `app.config` will
raise a Kconfig error.just use a choice override?

Choice overrides depend on inclusion order, which is is not guaranteed.
A module in `sys` or `pkg` or `drivers` may not be included in the order where
the overriding needs to be. To enforce that would constrain the system too much.
See the kconfig documentation for more details.
