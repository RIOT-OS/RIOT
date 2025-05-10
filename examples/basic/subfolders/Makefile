# name of your application
APPLICATION = subfolders

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../..

# Add subfolders as modules
EXTERNAL_MODULE_DIRS += $(CURDIR)/external_modules/
USEMODULE += module # module name must match folder name

# This is used for the test integration: The app will wait for "s" being sent
# before starting with the output, so that the test does not fail because it
# already finished printing when the terminal attaches.
USEMODULE += test_utils_interactive_sync

# Add source files in subfolders manually
SRC += main.c
SRC += folder/a.c folder/subfolder/b.c folder/subfolder/c.c

# Alternative method to add files in subfolders using wildcards
# SRC += $(wildcard *.c folder/*.c folder/**/*.c)

# Adding files in subfolders both via SRC and EXTERNAL_MODULE_DIRS
# will generate a warning and likely fail during linking
# SRC += external_modules/module/a.c

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
