include ../Makefile.drivers_common

USEMODULE += servo
USEMODULE += shell_cmds_default
USEMODULE += saul_default

include $(RIOTBASE)/Makefile.include

SERVO_PIN ?= GPIO_PIN(0, 0)

TIMER ?= TIMER_DEV(0)
TIMER_FREQ ?= CLOCK_CORECLOCK/8

ifneq (,$(filter atmega_common,$(USEMODULE)))
  # The ATmega PWM driver has no support for 16 bit timers (as of now). Hence,
  # limit the PWM resolution to what a 8 bit timer can handle.
  CFLAGS += -DSERVO_PWM_PARAM_RES=256
endif

CFLAGS += '-DSERVO_PARAM_PIN=$(SERVO_PIN)'
CFLAGS += '-DSERVO_TIMER_PARAM_DEV=$(TIMER)'
CFLAGS += '-DSERVO_TIMER_PARAM_FREQ=$(TIMER_FREQ)'
