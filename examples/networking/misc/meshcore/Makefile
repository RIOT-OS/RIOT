# name of your application
APPLICATION = meshcore

# Use the ST B-L072Z-LRWAN1 board by default:
BOARD ?= b-l072z-lrwan1

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Default radio driver is Semtech SX1276 (used by the B-L072Z-LRWAN1 board),
# use sx1262 or sx126x_stm32wl for the SX126x transceivers
LORA_DRIVER ?= sx1276

USEPKG += meshcore
USEMODULE += meshcore_node
USEMODULE += $(LORA_DRIVER)

USEMODULE += shell
USEMODULE += shell_cmds_default

# The shell commands use the cryptography of MeshCore
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(4*THREAD_STACKSIZE_DEFAULT\)

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
