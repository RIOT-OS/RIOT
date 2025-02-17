# name of your application
APPLICATION = subfolders

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../..

# Add subfolders as modules
DIRS += module
USEMODULE += my_module # name as defined in module/Makefile

# Add source files in subfolders manually
SRC += main.c
SRC += folder/a.c folder/subfolder/b.c folder/subfolder/c.c

# Alternative method to add files in subfolders using wildcards
# SRC += $(wildcard *.c folder/*.c folder/**/*.c)

# Adding subfolders both via SRC and DIRS will generate a warning
# and likely fail during linking
# DIRS += folder

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
