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
`USB` implementation because it `REQUIRES_USB_CDC_ACM` feature that may not exist
in all `USB` implementations.

#### Pattern: Choice Error

This pattern uses a choice fall-through to select the `ERROR_` condition.
The tradeoff for this is:
- :+1 simple, lowest lines of code
- :+1 visible error condition in `menuconfig`
- :+1 scalable, if more conditions are added there is still only 1 fall-through
- :-1 User can select the error condition as a choice
- :-1 No so obvious how to resolve the issue without looking around or being explict with the help

Let's step through resolving the error of the use case:

```
./../../riot_menuconfig.py Kconfig.use_case.choice_error
```

1. Press `/` then find `ERROR_USB_IMPL` is `y` and press Enter.
2. Press `?` to inspect `ERROR_USB_IMPL(=y)` and read the help message.
3. Back out and press `A` to see the hidden choices.
4. Press `?` to check for the missing dependencies.
5. `MODULE_MIN_USB` shows `!REQUIRES_USB_CDC_ACM(=y)  (=n)`, which means we need to not require `USB_CDC_ACM`.
6. `MODULE_RIOT_USB` and `PACKAGE_USB` shows `MISSING_DEP(=n)`
7. This means we either need to remove the requirement for `USB_CDC_ACM` or select the `MISSING_DEP`
8. Since we don't want to rewire to use the UART we can select the `MISSING_DEP`
10. Now both `MODULE_RIOT_USB` and `PACKAGE_USB` are selectable, defaulting to `MODULE_RIOT_USB`


Exiting at anytime with the `ERROR_` selected will result in a failure when using with make.

#### Pattern: Requires

This pattern uses a module independent `ERROR_` condition but must be aware of everything fitting the requirements.
The tradeoff for this is:
- :+1 simple if very few possible selectable modules (no more than 2)
- :+1 easiest to find the source of the error as there are few abstractions
- :-1 Not scalable, if there more modules or choices that fit the requirements, the conditional list for the error grows

Let's step through resolving the error of the use case:

```
./../../riot_menuconfig.py Kconfig.use_case.requires
```

1. Press `/` then find `ERROR_USB_IMPL` is `y` and press Enter.
2. Press `?` to inspect `ERROR_USB_IMPL(=y)`.
3. Evaluate `MODULE_PACKAGE_USB_CDC_ACM` or `!MODULE_RIOT_USB_CDC_ACM`.
4. Move up one level to evaluate `MODULE_RIOT_USB` or `PACKAGE_USB` and find `MISSING_DEP(=n)`.
5. Now both `MODULE_RIOT_USB` and `PACKAGE_USB` are selectable, defaulting to `MODULE_RIOT_USB`.


#### Pattern: Requires and Provides

This pattern uses a module independent `ERROR_` condition and uses a feature abstraction to scale.
This also allows external modules to provide a requirement without needing to adapt RIOT.

The tradeoff for this is:
- :+1 scalable
- :+1 better module isolation, allowing external modules to use the logic
- :-1 abstractions make it harder to find what is causing the error
- :-1 heavy on boilerplate code

Let's step through resolving the error of the use case:

```
./../../riot_menuconfig.py Kconfig.use_case.requires_provides
```

1. Press `/` then find `ERROR_USB_IMPL` is `y` and press Enter.
2. Press `?` to inspect `ERROR_USB_IMPL(=y)` to see `!PROVIDES_USB_CDC_ACM(=n)`.
3. Evaluate `PROVIDES_USB_CDC_ACM` to see that either `MODULE_RIOT_USB_CDC_ACM` or `MODULE_PACKAGE_USB_CDC_ACM` is needed.
4. Evaluate `MODULE_PACKAGE_USB_CDC_ACM` or `!MODULE_RIOT_USB_CDC_ACM`.
5. Move up one level to evaluate `MODULE_RIOT_USB` or `PACKAGE_USB` and find `MISSING_DEP(=n)`
6. Now both `MODULE_RIOT_USB` and `PACKAGE_USB` are selectable, defaulting to `MODULE_RIOT_USB`

## Generic Pattern

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

### Pre-selected an error condition having a PACKAGE_USB

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
ng specific backends is not recommended or desired.
Modules should select only the generic module and using the `HAS_*`
instead. This is only to cover the corner cases which a generic module
and preference is not possible.
