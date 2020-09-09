# Determine the list of modules that provide auto_init from the list of all
# auto_init_xx.c files available in the current directory.
# The rule is to have the file name matching the pattern: auto_init_<module name>.c
AUTO_INIT_PREFIX = auto_init_
AUTO_INIT_SRCS = $(wildcard $(AUTO_INIT_PREFIX)*.c)
AUTO_INIT_MODULES_AVAILABLE = $(subst $(AUTO_INIT_PREFIX),,$(subst .c,,$(AUTO_INIT_SRCS)))

# Compare this list against the modules used in the current build to determine
# which auto_init_xx.c file should be added to the build
AUTO_INIT_MODULES_USED = $(filter $(AUTO_INIT_MODULES_AVAILABLE),$(USEMODULE))
AUTO_INIT_MODULES_USED_SRCS = $(addprefix $(AUTO_INIT_PREFIX),$(addsuffix .c,$(AUTO_INIT_MODULES_USED)))

# Set the list of files to build in the auto_init module
SRC = init.c $(AUTO_INIT_MODULES_USED_SRCS)

include $(RIOTBASE)/Makefile.base
