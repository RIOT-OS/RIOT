# name of your application
APPLICATION = skald_bthome

# If no BOARD is found in the environment, use this default:
BOARD ?= nrf52dk

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../../../

# include Skald using SAUL
USEMODULE += saul_default
USEMODULE += skald_bthome_saul
FEATURES_REQUIRED += picolibc
USEMODULE += picolibc

DEVELHELP ?= 1

ifeq ($(DEVELHELP),0)
  USEMODULE += stdio_null
endif

BTHOME_NAME = "RIOT"
BTHOME_ADV_INTERVAL ?= 120000
ENCRYPTION_KEY ?=

ifneq ($(ENCRYPTION_KEY),)
  USEMODULE += skald_bthome_encrypt
  CFLAGS += -DENCRYPTION_KEY="\"$(ENCRYPTION_KEY)\""
endif

CFLAGS += -DISR_STACKSIZE=1024

CFLAGS += -DBTHOME_NAME="\"$(BTHOME_NAME)\""
CFLAGS += -DBTHOME_ADV_INTERVAL=$(BTHOME_ADV_INTERVAL)

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
