# name of your application
APPLICATION = wolfcrypttest

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP
CFLAGS += -DWOLFSSL_RIOT_OS

#ignore warnings on empty translation units due to wolfSSL configurability
WERROR = 0

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# use BINARY_DIRS instead of DIRS
#BINARY_DIRS += wolfssl/src
INCLUDES += -I$(RIOTBASE)/tests/security/wolfssl

# list of files to include in binary distribution
# "bin/$(BOARD)/$(APPLICATION).elf" will automatically be added
DIST_FILES += Makefile
DIST_FILES += wolf-build-and-run-test.sh

include $(RIOTBASE)/Makefile.include
