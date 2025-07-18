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

## Speed-up builds with ccache

[`ccache`](https://ccache.samba.org/) is a compiler cache. It speeds up recompilation by caching previous compilations and detecting when the same compilation is being done again.

Usually, the initial build takes a little (5% - 20%) longer, but repeated builds are up to ten times faster.
Using `ccache` is safe, as `ccache` tries very hard to not mess up things and falls back to a normal compile if it cannot ensure correct output.

There's one drawback: without further tweaking, `gcc` stops emitting colored output.

### Setup

- Install using the package manager of your distribution, e.g., on Ubuntu or Debian:

```
sudo apt-get install ccache
```

- Set `CCACHE` variable to `ccache`:

```
export CCACHE=ccache
```

- (Optionally) add the above export line to your `~/.profile`

### Result

Build without `ccache`:

```sh
[kaspar@booze default (master)]$ time BOARD=samr21-xpro make clean all
Building application "default" for "samr21-xpro" with MCU "samd21".

[...]

    text    data     bss     dec     hex filename
  37016     180    6008   43204    a8c4 /home/kaspar/src/riot/examples/basic/default/bin/samr21-xpro/default.elf

real    0m12.321s
user    0m10.317s
sys     0m1.170s
[kaspar@booze default (master)]$
```

First build with `ccache` enabled:

```sh
[kaspar@booze default (master)]$ time BOARD=samr21-xpro make clean all
Building application "default" for "samr21-xpro" with MCU "samd21".

[...]

text    data     bss     dec     hex filename
37016     180    6008   43204    a8c4 /home/kaspar/src/riot/examples/basic/default/bin/samr21-xpro/default.elf

real    0m15.462s
user    0m12.410s
sys     0m1.597s
[kaspar@booze default (master)]$
```

Subsequent build with `ccache` enabled:

```sh
[kaspar@booze default (master)]$ time BOARD=samr21-xpro make clean all
Building application "default" for "samr21-xpro" with MCU "samd21".

[...]

    text    data     bss     dec     hex filename
  37016     180    6008   43204    a8c4 /home/kaspar/src/riot/examples/basic/default/bin/samr21-xpro/default.elf

real    0m2.157s
user    0m1.213s
sys     0m0.327s
[kaspar@booze default (master)]$
```

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

## Comparing Build Sizes

There is a make target for build size comparison. It will automatically check
all the boards compiled in the `NEWBIN` and `OLDBIN` and compare them.
For boards that do not have a complementary partner, a warning is generated.
You can use it like that:

```sh
$ cd RIOT/test/test_something

$ git checkout master
$ BINDIRBASE=master-bin BOARD=native64 make all

$ git checkout my-branch
$ BINDIRBASE=my-branch-bin BOARD=native64 make all

$ OLDBIN=master-bin NEWBIN=my-branch-bin make info-buildsizes-diff
text    data    bss     dec     BOARD/BINDIRBASE

0       0       0       0       native64    **← this line contains the diff**
57356   1532    96769   155657  master-bin
57356   1532    96769   155657  my-branch-bin
...
```

Check it out, the output contains colors. ;)

## RIOT-aware Completion in zsh

For zsh users a RIOT-aware completion is provided in `dist/tools/zsh-completion`. Refer to the `README.md` in there for more details and installation instructions.
