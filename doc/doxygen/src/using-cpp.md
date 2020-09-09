Using C++ in RIOT                                                  {#using-cpp}
=================

[TOC]

Levels of Support                                          {#levels-of-support}
=================

A CPU in RIOT can have three levels of support for C++ code:

1. No support for C++ at all
2. C++ is supported, but no libstdc++ implementation is available
3. C++ is supported and a libstdc++ implementation is available

The reason for missing or only partial C++ support can be one (or more) of
the following:

- No libstdc++ implementation for the target platform is available to RIOT, or
  the official RIOT docker image is missing it
- Missing toolchain support for a given target platform
- The C++ toolchain requires library code (such as constructor guards for the
  thread safe initialization of statically allocated instances) or hooks in
  the startup process to perform initialization

Using C++
=========

In order for C++ code to compile with RIOT, the following needs to be done:

- All C++ files must have the file extension `.cpp`, all C++ headers `.hpp`
    - For external code, overwriting the make variable `SRCXXEXT` e.g. to
      `cxx` can be used to compile C++ files with other extensions, e.g. `.cxx`
- `FEATURES_REQUIRED += cpp` must be added to the applications `Makefile`
    - If additionally the libstdc++ is used, `FEATURES_REQUIRED += libstdcpp`
      must be used additionally

RIOT Modules in C++                                              {#cpp-in-riot}
===================

RIOT modules should be written in C, so that boards/platforms without or partial
C++ support can still use these modules. However, external modules, packages,
and modules that require C++ support anyway (e.g. the Arduino compatibility
features) can be written in C++. These modules/packages have to depend on the
`cpp` feature (`FEATURES_REQUIRED += cpp`) and possibly the `libstdcpp`
feature using their `Makefile.dep`.

See Also                                                            {#see-also}
========

@ref sys_c11_atomics_cpp_compat, @ref cpp11-compat
