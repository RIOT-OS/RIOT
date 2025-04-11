---
title: Advanced Build System Tricks
description: This page describes some build systems tricks that can help developers but are not part of the standard workflow.
---

This page describes some build systems tricks that can help developers but are not part of the standard workflow.

They are low level commands that should not be taken as part of a stable API but better have a documentation than only having a description in the build system code.

## Customize the build system

- `RIOT_MAKEFILES_GLOBAL_PRE`: files parsed before the body of `$RIOTBASE/Makefile.include`
- `RIOT_MAKEFILES_GLOBAL_POST`: files parsed after the body of `$RIOTBASE/Makefile.include`

The variables are a list of files that will be included by `$RIOTBASE/Makefile.include`. They will be handled as relative to the application directory if the path is relative.

## Usage

You can configure your own files that will be parsed by the build system main `Makefile.include` file before or after its main body, examples usages can be:

- Globally overwrite a variable, like `TERMPROG`
- Specify a hard written `PORT` / `DEBUG_ADAPTER_ID` for some BOARD values
- Define your custom targets
- Override default targets

## Analyze dependency resolution

When refactoring dependency handling or modifying variables used for dependency resolution, one may want to evaluate the impact on the existing applications. This describe some debug targets to dump variables used during dependency resolution.

To analyze one board and application run the following commands in an application directory.

Generate the variables dump with the normal dependency resolution to a `dependencies_info_board_name` file:

```sh
BOARD=board_name make dependency-debug
```

Or with the "quick" version used by murdock to know supported boards (this is an incomplete resolution, details in `makefiles/dependencies_debug.inc.mk`) to a `dependencies_info-boards-supported_board_name` file:

```sh
BOARDS=board_name DEPENDENCY_DEBUG=1 make info-boards-supported
```

For more configuration and usage details, see in the file defining the targets `makefiles/dependencies_debug.inc.mk`.

To do a repository wide analysis, you can use the script `dist/tools/buildsystem_sanity_check/save_all_dependencies_resolution_variables.sh` that will generate the output for all boards and applications. It currently take around 2 hours on an 8 cores machine with ssd.

## Generate Makefile.ci content

Most applications and tests include a `Makefile.ci` to indicate which boards cannot compile the application or test. The content for these files can be generated via the script in:

```sh
make -C $APPLICATION_DIRECTORY generate-Makefile.ci
```

This will compile and link the application for every board available and record the result in the Makefile.ci. This requires the toolchain for every target to be available. The target supports using docker via the `BUILD_IN_DOCKER=1` variable.

## Out of Tree Cache Directory

By exporting the `BUILD_DIR` environment variable, a custom build / clone cache directory can be created. This can be particularly useful when working with multiple git work trees or clones of the RIOT repository.

## RIOT-aware Completion in zsh

For zsh users a RIOT-aware completion is provided in `dist/tools/zsh-completion`. Refer to the `README.md` in there for more details and installation instructions.
