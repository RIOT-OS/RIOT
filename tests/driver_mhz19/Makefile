include ../Makefile.tests_common

USEMODULE += ztimer
USEMODULE += ztimer_msec

# set default device parameters in case they are undefined
TEST_MODE ?= 1

TEST_UART ?= UART_DEV\(1\)
TEST_PWM ?= GPIO_PIN\(PC,8\)

ifeq ($(TEST_MODE),1)
  USEMODULE += mhz19_uart
endif

ifeq ($(TEST_MODE),2)
  USEMODULE += mhz19_pwm
endif

# export parameters
CFLAGS += -DTEST_MODE=$(TEST_MODE)
CFLAGS += -DTEST_UART=$(TEST_UART)
CFLAGS += -DTEST_PWM=$(TEST_PWM)

include $(RIOTBASE)/Makefile.include
