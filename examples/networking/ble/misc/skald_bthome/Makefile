# name of your application
APPLICATION = skald_bthome

# If no BOARD is found in the environment, use this default:
BOARD ?= nrf52dk

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../../../

# include Skald using SAUL
USEMODULE += saul_default
USEMODULE += skald_bthome_saul

DEVELHELP ?= 1

BTHOME_NAME = "RIOT"
BTHOME_ADV_INTERVAL ?= 60000

CFLAGS += -DBTHOME_NAME="\"$(BTHOME_NAME)\""
CFLAGS += -DBTHOME_ADV_INTERVAL=$(BTHOME_ADV_INTERVAL)

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
