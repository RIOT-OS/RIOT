include ../Makefile.tests_common
DISABLE_MODULE += auto_init_sx126x
# other values supported are sx1262, sx1268 and llcc68
LORA_DRIVER ?= sx126x_stm32wl
USEMODULE += $(LORA_DRIVER)
USEMODULE += od
USEMODULE += od_string
USEMODULE += shell
USEMODULE += shell_cmds_default

include $(RIOTBASE)/Makefile.include
