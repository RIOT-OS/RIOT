# Application Example with Subfolders

This example demonstrates the usage of subfolders in a RIOT application
(or in a RIOT module in general) show-casing two possible approaches:
External RIOT modules and simple subfolders.

## Details

Consider the following folder structure of this example.
The source files in `module` are incorporated as a RIOT module,
while the source files in `folder` are considered part of the application itself.

```
.
├── external_modules
│   └── module
│       ├── a.c
│       ├── b.c
│       └── Makefile
├── folder
│   ├── a.c
│   └── subfolder
│       ├── b.c
│       └── c.c
├── main.c
├── Makefile
└── README.md
```

### RIOT modules

At a minimum, each module in RIOT requires a `Makefile` with the following content:

```Makefile
include $(RIOTBASE)/Makefile.base
```

The name of the module's directory is automatically used as the module name.
It is important to note that module names have to be unique among _all_ RIOT modules,
i.e., including the modules that are part of RIOT itself.

If not manually specified via `SRC`, all source files which reside
directly in the module's directory are considered part of the module.
RIOT modules are also described in greater detail [in the documentation](https://doc.riot-os.org/creating-modules.html).

Two lines need to be added to the application's Makefile in order to compile and use the module:

```Makefile
EXTERNAL_MODULE_DIRS += $(CURDIR)/external_modules/
USEMODULE += module
```

The path added to `EXTERNAL_MODULE_DIRS` is the parent directory of the external module,
while the string added to `USEMODULE` has to match the module's directory name.
External modules are described in greater detail [in the documentation](https://doc.riot-os.org/creating-an-application.html#autotoc_md2308).


### Subfolders

Compared to the module approach, no additional Makefile is needed in the subfolder.
The application's Makefile needs to add _all_ source files explicitly,
including the ones residing directly in the application directory:

```Makefile
SRC += main.c
SRC += folder/a.c folder/subfolder/b.c
```

To avoid listing all source files individually, it is of course possible
to use normal GNU make functions such as `wildcard`:

```Makefile
SRC += $(wildcard *.c folder/*.c folder/**/*.c)
```


## Which approach should I use?

In general, modules in RIOT are well-defined units of code that provide a set of features to your application.
If this matches your use-case, i.e., you have all your application tests separated into a subfolder,
RIOT modules are probably the best approach.
It is good practice to prefix all your application modules to avoid name clashes.

If however you barely want to organize your files in a sensible folder structure,
but always require all source files to be part of your application,
the more straight-forward subfolder approach is probably the better pick.
