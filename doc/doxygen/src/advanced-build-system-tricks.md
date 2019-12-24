Advanced build system tricks                    {#advanced-build-system-tricks}
============================

[TOC]

Introduction                                                    {#introduction}
============

This page describes some build systems tricks that can help developers but are
not part of the standard workflow.

They are low level commands that should not be taken as part of a stable API
but better have a documentation than only having a description in the build
system code.


Customize the build system                            {#customize-build-system}
==========================

+ `RIOT_MAKEFILES_GLOBAL_PRE`: files parsed before the body of
  `$RIOTBASE/Makefile.include`
+ `RIOT_MAKEFILES_GLOBAL_POST`: files parsed after the body of
  `$RIOTBASE/Makefile.include`

The variables are a list of files that will be included by
`$RIOTBASE/Makefile.include`.
They will be handled as relative to the application directory if the path is
relative.


Usage
-----

You can configure your own files that will be parsed by the build system main
`Makefile.include` file before or after its main body, examples usages can be:

* Globally overwrite a variable, like `TERMPROG`
* Specify a hard written `PORT` / `DEBUG_ADAPTER_ID` for some BOARD values
* Define your custom targets
* Override default targets


Analyze dependency resolution                   {#analyze-depedency-resolution}
=============================

When refactoring dependency handling or modifying variables used for dependency
resolution, one may want to evaluate the impact on the existing applications.
This describe some debug targets to dump variables used during dependency
resolution.

To analyze one board and application run the following commands in an
application directory.

Generate the variables dump with the normal dependency resolution to a
`dependencies_info_board_name` file:

~~~~~~~~~~~~~~~~~~~
BOARD=board_name make dependency-debug
~~~~~~~~~~~~~~~~~~~

Or with the "quick" version used by murdock to know supported boards
(this is an incomplete resolution, details in `makefiles/dependencies_debug.inc.mk`)
to a `dependencies_info-boards-supported_board_name` file:

~~~~~~~~~~~~~~~~~~~
BOARDS=board_name DEPENDENCY_DEBUG=1 make info-boards-supported
~~~~~~~~~~~~~~~~~~~

For more configuration and usage details, see in the file defining the targets
`makefiles/dependencies_debug.inc.mk`

To do a repository wide analysis, you can use the script
`dist/tools/buildsystem_sanity_check/save_all_dependencies_resolution_variables.sh`
that will generate the output for all boards and applications.
It currently take around 2 hours on an 8 cores machine with ssd.
