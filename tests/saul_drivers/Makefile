APPLICATION = saul_drivers
include ../Makefile.tests_common

SAUL_DRIVERS = adc081c \
               adc101c \
               adc121c \
               adxl345 \
               bmp180 \
               bmp280 \
               bme280 \
               dht \
               hdc1000 \
               io1_xplained \
               isl29020 \
               jc42 \
               l3g4200d \
               lis3dh \
               lps331ap \
               lsm6dsl \
               lsm303dlhc \
               mag3110 \
               mma8x5x \
               si70xx \
               tcs37727 \
               tmp006 \
               tsl2561 \
               veml6070

USEMODULE += saul_default
USEMODULE += auto_init_saul
USEMODULE += $(SAUL_DRIVERS)

include $(RIOTBASE)/Makefile.include
