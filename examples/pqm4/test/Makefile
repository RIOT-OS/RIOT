# name of your application
APPLICATION = pqm4-test


# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../..

# Uncomment these lines if you want to use platform support from external
# repositories:
#RIOTCPU ?= $(CURDIR)/../../RIOT/thirdparty_cpu
#RIOTBOARD ?= $(CURDIR)/../../RIOT/thirdparty_boards

# Uncomment this to enable scheduler statistics for ps:
#USEMODULE += schedstatistics

# If you want to use native with valgrind, you should recompile native
# with the target all-valgrind instead of all:
# make -B clean all-valgrind

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 0

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += random
USEMODULE += xtimer

USEPKG += pqm4

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(20*THREAD_STACKSIZE_DEFAULT+THREAD_EXTRA_STACKSIZE_PRINTF\) # Same as for tweetnacl


# Choose a key encapsulation scheme
#
#PQ_KEM=frodo640-cshake
#PQ_KEM=kindi256342
#PQ_KEM=sntrup4591761
#PQ_KEM=kyber768
#PQ_KEM=newhope1024cca
#PQ_KEM=ntruhrss701
#PQ_KEM=saber
#PQ_KEM=sikep751

# Choose a signature scheme
#PQ_SIGN=dilithium
#PQ_SIGN=sphincs-shake256-128s

include $(RIOTBASE)/Makefile.include
