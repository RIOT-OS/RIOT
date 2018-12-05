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
