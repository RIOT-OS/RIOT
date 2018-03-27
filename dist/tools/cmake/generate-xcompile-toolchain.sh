#!/usr/bin/env sh
echo "SET(CMAKE_SYSTEM_NAME Generic)"
echo "SET(CMAKE_SYSTEM_VERSION 1)"
# specify the cross compiler"
echo "SET(CMAKE_C_COMPILER \"${CC}\" CACHE STRING \"\")"
echo "SET(CMAKE_CXX_COMPILER \"${CXX}\" CACHE STRING \"\")"
echo "SET(CMAKE_LINKER \"${LINK}\" CACHE STRING \"\")"
echo "SET(CMAKE_RANLIB \"${RANLIB}\" CACHE STRING \"\")"
# disable linker test
echo "SET(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)"
echo "SET(CMAKE_C_FLAGS \"${CFLAGS}\" CACHE STRING \"\")"
echo "SET(CMAKE_EXE_LINKER_FLAGS \"${LFLAGS}\" CACHE STRING \"\")"
# search for programs in the build host directories
echo "SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)"
# for libraries and headers in the target directories
echo "SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)"
echo "SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)"
