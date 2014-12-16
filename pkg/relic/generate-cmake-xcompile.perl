#!/usr/bin/env perl
print "INCLUDE(CMakeForceCompiler)\n";
print "\n";
print "\n";
print "SET(CMAKE_SYSTEM_NAME Generic)\n";
print "SET(CMAKE_SYSTEM_VERSION 1)\n";
print "\n";
print "SET(CMAKE_C_COMPILER \"$ENV{CC}\" CACHE STRING \"\")\n";
print "SET(CMAKE_CXX_COMPILER \"$ENV{CXX}\" CACHE STRING \"\")\n";
print "SET(CMAKE_RANLIB \"echo\" CACHE STRING \"\")\n";

print "\n";
print "# specify the cross compiler\n";
print "CMAKE_FORCE_C_COMPILER(\${CMAKE_C_COMPILER} GNU)\n";
print "CMAKE_FORCE_CXX_COMPILER(\${CMAKE_CXX_COMPILER} GNU)\n";
print "SET(CMAKE_LINKER \"$ENV{LINK}\" CACHE STRING \"\")\n";
print "\n";
my $esc_c_flags = "$ENV{CFLAGS}";
$esc_c_flags =~ s/"/\\"/g;
print "SET(CMAKE_C_FLAGS \"$esc_c_flags\" CACHE STRING \"\")\n";
print "\n";
print "SET(CMAKE_EXE_LINKER_FLAGS \"$ENV{LINKFLAGS}\" CACHE STRING \"\")\n";

print "\n";
print "# search for programs in the build host directories\n";
print "SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)\n";
print "# for libraries and headers in the target directories\n";
print "SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)\n";
print "SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)\n";
