# exclude submodule sources from *.c wildcard source selection

IGNORE_SRC = \
    dw1000_pkg.c \
    dw1000_cli.c \
    dw1000_cli_priv.c \
    dw1000_sysfs.c \
    dw1000_debugfs.c \
    #

SRC := $(filter-out $(IGNORE_SRC),$(wildcard *.c))

include $(RIOTBASE)/Makefile.base
