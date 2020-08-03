# Kconfig in RIOT                                           {#kconfig-in-riot}
[TOC]

# Glossary                                                   {#kconfig-glossary}
- **Feature**: indicator that a certain characteristic or functionality is
present.
- **Module**: unit which provides functionalities and may provide features.
May or may not have associated code ([see](./creating-modules.html)).
- **Menuconfig**: in a Kconfig file begins the definition of a symbol hinting front-ends that
a submenu, containing the following dependant symbols, should be created.

# Sections

- [User's guide to configure with Kconfig](./kconfig-user-guide.html)
- [Developer's guide and tips to write Kconfig files](./kconfig-developer-guide.html)
- [Integration of Kconfig into the build system](./kconfig-build-system.html)

# Overview                                                  {#kconfig-overview}
The objective of using Kconfig in RIOT is to select and configure software
modules at compile-time. This means having a standard way of:
- Selecting modules
- Exposing configurable parameters
- Assigning application and user-specific configurations
- Verifying these parameters
    - Check possible values
    - Check valid configuration considering inter-dependencies
- Applying the selected configuration

## Module selection
The user selects which functionalities to include in the application by adding
[modules](./creating-modules.html). Modules may have dependencies, so Kconfig
only presents the user with the modules it can select depending on the target
hardware and the modules that are already activated (e.g. by the application
default configuration).

## Exposure
Modules in RIOT expose their configurable parameters via
Kconfig files (for more information on Kconfig syntax check
[the specification](https://www.kernel.org/doc/html/latest/kbuild/kconfig-language.html)).
In these files documentation, restrictions, default values and dependencies can
be expressed.

Kconfig files are structured through the file system mirroring the current
module distribution. In time, all modules will have Kconfig files to make
themselves configurable through this system.

## Assignment
The user can assign values to the exposed parameters, either by manually writing
'.config' files or using an interface such as Menuconfig. Parameters with no
assigned values will take the default ones. For a detailed distinction between
Kconfig and '.config' files see the
[Appendix A](#kconfig-appendix-different-files).

## Verification and application
Using '.config' and Kconfig files the build system takes care of doing the
necessary checks on the values according to the parameter definition. After
that, the `autoconf.h` header file is generated, it contains all the
configurations in the form of (`CONFIG_` prefixed) macros.

---
# Appendixes                                              {#kconfig-appendixes}

## Appendix A: Difference between 'Kconfig' and '.config' files {#kconfig-appendix-different-files}
Kconfig files describe a configuration database, which is a collection of
configuration options organized in a tree structure. Configuration options may
have dependencies (among other attributes), which are used to determine their
visibility.

Kconfig files are written in
[Kconfig language](https://www.kernel.org/doc/Documentation/kbuild/kconfig-language.txt)
defined in the Linux kernel. Configuration options have attributes such as
types, prompts and default values.

#### Kconfig file

```Kconfig
menu "Buffer Sizes"

config GCOAP_PDU_BUF_SIZE
    int "Request or response buffer size"
    default 128

endmenu
```

On the other hand configuration files contain assignment of values to
configuration options and use Makefile syntax. They can also be used to save a
set of configuration values as backup.

#### '.config' file

```makefile
# enable Kconfig configuration for gcoap
CONFIG_KCONFIG_MODULE_GCOAP=y
# set the value
CONFIG_GCOAP_PDU_BUF_SIZE=12345
```

In other words: Kconfig files describe configuration options and '.config' files
assign their values.

## Appendix B: Pitfall when using different configuration interfaces {#kconfig-appendix-pitfall-interfaces}
In the current configuration flow the user can choose to configure RIOT using
the menuconfig graphical interface or writing '.config' files by hand.

As explained in the
['Configuration sources merging step'](#kconfig-steps-merge-configs)
of the configuration process, configuration from multiple sources are loaded to
create a single `merged.config` file, and the order of merging matters: last
file has priority.

While editing values directly via '.config' files `merged.config` will be
re-built. Once the user decides to edit `merged.config` directly using
menuconfig, the file will not be re-built anymore, and any changes by manually
editing the source files will have no effect. To go back to manual edition
a `make clean` has to be issued in the application directory.

## Appendix D: A few key aspects while exposing a macro to Kconfig {#kconfig-appendix-d}
A macro that holds a 0 or 1 is modelled in Kconfig as a `bool` symbol. References to this macro
can then make use of IS_ACTIVE macro from kernel_defines.h with C conditionals
for conditional compilation.
[FXOS8700 driver exposure to Kconfig](https://github.com/RIOT-OS/RIOT/pull/13914)
can be considered as an example. If the macro is defined as `TRUE` by default,
a new symbol gets introduced to invert the semantics. The recommended
practice is to add a new symbol and expose it to Kconfig while the old one is
tagged to be deprecated. The process is documented in this
[commit](https://github.com/RIOT-OS/RIOT/pull/13129/commits/c7b6dc587cf20f3177abe0417a408b6ab90d0ff8)

There may be cases where a macro is expected to hold only specific values, e.g.
'GNRC_IPV6_MSG_QUEUE_SIZE' expressed as the power of two. These may be modelled
in such a way that a new macro is introduced to hold the restricted figures
while operators are added to arrive at the desired value. The process is
documented in this [pull request.](https://github.com/RIOT-OS/RIOT/pull/14086)

# Useful references                                {#kconfig-useful-references}
- [Kconfig language specification](https://www.kernel.org/doc/html/latest/kbuild/kconfig-language.html)
- [Kconfig macro language specification](https://www.kernel.org/doc/html/latest/kbuild/kconfig-macro-language.html)
- [Kconfig - Tips and Best Practices](https://docs.zephyrproject.org/latest/guides/kconfig/tips.html)
