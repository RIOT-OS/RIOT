include ../Makefile.tests_common

USEMODULE += bus_stats_spi
USEMODULE += bus_stats_i2c

# Many sensors and actuators are connected via I2C, some via SPI, so add them
# as potential devices to monitor.
USEMODULE += saul_default

# Add the shell cmd to print / clear bus stats (pulls in a shell as dep)
USEMODULE += shell_cmd_bus_stats
USEMODULE += shell_cmd_saul_reg

include $(RIOTBASE)/Makefile.include
