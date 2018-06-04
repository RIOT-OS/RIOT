# application name
APPLICATION = riot_javascript

# default BOARD environment
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := airfy-beacon b-l072z-lrwan1 bluepill calliope-mini \
                             cc2650-launchpad cc2650stk hifive1 maple-mini \
                             microbit nrf51dongle nrf6310 nucleo-f030r8 nucleo-f070rb \
                             nucleo-f072rb nucleo-f103rb nucleo-f302r8 nucleo-f334r8 nucleo-f410rb \
                             nucleo-l053r8 nucleo-l073rz nucleo-f031k6 nucleo-f042k6 \
                             nucleo-f303k8 nucleo-l031k6 opencm904 \
                             spark-core stm32f0discovery stm32mindev \
                             yunjia-nrf51822

BOARD_BLACKLIST := arduino-duemilanove arduino-mega2560 arduino-uno chronos \
                   msb-430 msb-430h telosb waspmote-pro wsn430-v1_3b \
                   wsn430-v1_4 z1 pic32-wifire pic32-clicker jiminy-mega256rfr2 \
                   mega-xplained

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

ifneq ($(BOARD),native)
# Set stack size to something (conservatively) enormous
CFLAGS += -DTHREAD_STACKSIZE_MAIN=9092
endif

# Add the package for Jerryscript
USEPKG += jerryscript

include $(RIOTBASE)/Makefile.include

JS_PATH := $(BINDIR)/js/$(MODULE)
# add directory of generated *.js.h files to include path
CFLAGS += -I$(JS_PATH)

# generate .js.h header files of .js files
JS = $(wildcard *.js)
JS_H := $(JS:%.js=$(JS_PATH)/%.js.h)

$(JS_PATH)/:
	@mkdir -p $@

$(JS_H): | $(JS_PATH)/
$(JS_H): $(JS_PATH)/%.js.h: %.js
	xxd -i $< | sed 's/^unsigned/const unsigned/g' > $@

$(RIOTBUILD_CONFIG_HEADER_C): $(JS_H)
