#!/usr/bin/env sh

# When setting variables, use the 'bracket argument' format to allow having \"
# inside the string. Which is not supported by quoted arguments
# https://cmake.org/cmake/help/latest/manual/cmake-language.7.html#bracket-argument
#
#     bracket_argument ::=  bracket_open bracket_content bracket_close
#     bracket_open     ::=  '[' '='* '['
#     bracket_content  ::=  <any text not containing a bracket_close with
#                            the same number of '=' as the bracket_open>
#                            bracket_close    ::=  ']' '='* ']'
#
# I will use [==[value]==] in this file, to not maybe match a case with one '='
# I used it for all variables even ones that currently do not need it.

printf 'SET(CMAKE_SYSTEM_NAME Generic)\n'
printf 'SET(CMAKE_SYSTEM_VERSION 1)\n'
# specify the cross compiler"
printf 'SET(CMAKE_C_COMPILER [==[%s]==] CACHE STRING "")\n' "${CC}"
printf 'SET(CMAKE_CXX_COMPILER [==[%s]==] CACHE STRING "")\n' "${CXX}"
printf 'SET(CMAKE_LINKER [==[%s]==] CACHE STRING "")\n' "${LINK}"
printf 'SET(CMAKE_RANLIB [==[%s]==] CACHE STRING "")\n' "${RANLIB}"
# disable linker test
printf 'SET(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)\n'
printf 'SET(CMAKE_C_FLAGS [==[%s]==] CACHE STRING "")\n' "${CFLAGS}"
printf 'SET(CMAKE_CXX_FLAGS [==[%s]==] CACHE STRING "")\n' "${CXXFLAGS}"
printf 'SET(CMAKE_EXE_LINKER_FLAGS [==[%s]==] CACHE STRING "")\n' "${LFLAGS}"
# search for programs in the build host directories
printf 'SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)\n'
# for libraries and headers in the target directories
printf 'SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)\n'
printf 'SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)\n'
