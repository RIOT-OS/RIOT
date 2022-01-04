PERIPH_COMMON_MODULES_SRCS_ALL = $(filter-out init.c,$(wildcard *.c))
PERIPH_COMMON_MODULES_NAMES = $(subst .c,,$(PERIPH_COMMON_MODULES_SRCS_ALL))
PERIPH_COMMON_MODULES = $(addprefix periph_, $(PERIPH_COMMON_MODULES_NAMES))
PERIPH_COMMON_MODULES_USED = $(filter $(PERIPH_COMMON_MODULES),$(USEMODULE))
SRC = init.c $(subst periph_,,$(addsuffix .c,$(PERIPH_COMMON_MODULES_USED)))

include $(RIOTBASE)/Makefile.base
