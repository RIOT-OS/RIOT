APPLICATION = lua_repl

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Uncomment the following lines to enable debugging features.
#CFLAGS_OPT = -Og
#CFLAGS += -DCONFIG_ASSERT_VERBOSE -DLUA_DEBUG

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# This value is in excess because we are not sure of the exact requirements of
# lua (see the package's docs). It must be fixed in the future by taking
# appropriate measurements.
CFLAGS += -DTHREAD_STACKSIZE_MAIN='(THREAD_STACKSIZE_DEFAULT+7000)'

USEPKG += lua

# generate .lua.h header files of .lua files
BLOBS += $(wildcard *.lua)

include $(RIOTBASE)/Makefile.include
