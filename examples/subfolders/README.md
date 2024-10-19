# Application Example with Subfolders

This example demonstrates the usage of subfolders in a RIOT application
show-casing two possible approaches: RIOT modules and simple subfolders.

## Details

Consider the following folder structure of this example.
The source files in `module` are incorporated as a RIOT module,
while the source files in `folder` are considered part of the application itself.

```
.
├── folder
│   ├── a.c
│   └── subfolder
│       └── b.c
├── main.c
├── Makefile
├── module
│   ├── a.c
│   ├── b.c
│   └── Makefile
└── README.md
```

### RIOT modules

At a minimum, each module in RIOT requires a `Makefile` with the following content:

```Makefile
MODULE := my_module

include $(RIOTBASE)/Makefile.base
```

If `MODULE` is not specified, the name of the module's directory is automatically used,
leaving only the last line as minimal content.
It is important to note that module names have to be unique both among _all_ RIOT modules,
i.e., including the modules that are part of RIOT itself.

If not manually specified via `SRC`, all source files which reside
directly in the module's directory are considered part of the module.
RIOT modules are also described in greater detail [in the documentation](https://doc.riot-os.org/creating-modules.html).

Two lines need to be added to the application's Makefile in order to compile and use the module:

```Makefile
DIRS += module
USEMODULE += my_module
```

The string added to `DIRS` has to match the directory name,
while the string added to `USEMODULE` has to match the module's name as defined above.


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
