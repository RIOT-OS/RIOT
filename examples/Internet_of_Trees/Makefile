# name of your application
APPLICATION = nimble_heart_rate_sensor

# If no BOARD is found in the environment, use this default:
BOARD ?= pulga

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

DRIVER ?= bme280_i2c
USEMODULE += $(DRIVER)
FEATURES_REQUIRED += periph_i2c
USEMODULE += checksum
USEMODULE += xtimer

CFLAGS += -DBMX280_PARAM_I2C_DEV=I2C_DEV\(0\)
CFLAGS += -DBMX280_PARAM_I2C_ADDR=0x77


# Some RIOT modules needed for this example
USEMODULE += event_timeout_ztimer

# Include NimBLE
USEPKG += nimble
USEMODULE += nimble_svc_gap
USEMODULE += nimble_svc_gatt

# Use automated advertising
USEMODULE += nimble_autoadv
CFLAGS += -DNIMBLE_AUTOADV_DEVICE_NAME='"Internet of Trees"'
CFLAGS += -DNIMBLE_AUTOADV_START_MANUALLY=1

# pa boost by default
CFLAGS += -DSX127X_PARAM_PASELECT="SX127X_PA_BOOST"

DEVEUI ?= cafe000000000099
APPEUI ?= cafe000000000099

DEVADDR ?= cafe0099
NWKSKEY ?= cafe0000000000000000000000000099
APPSKEY ?= cafe0000000000000000000000000099

# Default radio driver is Semtech SX1276 (used by the B-L072Z-LRWAN1 board)
USEMODULE += sx1272

# Default region is Europe and default band is 868MHz
LORA_REGION ?= AU915

# Include the Semtech-loramac package
USEPKG += semtech-loramac

USEMODULE += fmt

# printar float
USEMODULE += printf_float

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

ifndef CONFIG_KCONFIG_USEMODULE_LORAWAN
  # OTAA compile time configuration keys
  CFLAGS += -DCONFIG_LORAMAC_APP_EUI_DEFAULT=\"$(APPEUI)\"
  CFLAGS += -DCONFIG_LORAMAC_DEV_EUI_DEFAULT=\"$(DEVEUI)\"

  # ABP compile time configuration keys
  CFLAGS += -DCONFIG_LORAMAC_DEV_ADDR_DEFAULT=\"$(DEVADDR)\"
  CFLAGS += -DCONFIG_LORAMAC_NWK_SKEY_DEFAULT=\"$(NWKSKEY)\"
  CFLAGS += -DCONFIG_LORAMAC_APP_SKEY_DEFAULT=\"$(APPSKEY)\"

  CFLAGS += -DCONFIG_LORAMAC_DEFAULT_TX_MODE_UNCNF=1
  #CFLAGS += -DCONFIG_LORAMAC_DEFAULT_SYSTEM_MAX_RX_ERROR=100
endif