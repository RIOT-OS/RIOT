# name of your application
APPLICATION = tetris

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

download:
	wget ftp://troglobit.com/tetris/tetris-1.2.1.tar.bz2 -O- | tar -xvj
	ln -s tetris-1.2.1/*.h tetris-1.2.1/*.c .
	touch download

include $(RIOTBASE)/Makefile.include
