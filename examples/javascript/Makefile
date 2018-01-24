# application name
APPLICATION = riot_javascript

# default BOARD environment
BOARD ?= native

BOARD_INSUFFICIENT_MEMORY := airfy-beacon b-l072z-lrwan1 bluepill calliope-mini \
                             cc2650-launchpad cc2650stk maple-mini \
                             microbit nrf51dongle nrf6310 nucleo-f030 nucleo-f070 \
                             nucleo-f072 nucleo-f103 nucleo-f302 nucleo-f334 nucleo-f410 \
                             nucleo-l053 nucleo-l073 nucleo32-f031 nucleo32-f042 \
                             nucleo32-f303 nucleo32-l031 opencm904 \
                             spark-core stm32f0discovery yunjia-nrf51822 \

BOARD_BLACKLIST := arduino-duemilanove arduino-mega2560 arduino-uno chronos \
                   msb-430 msb-430h telosb waspmote-pro wsn430-v1_3b \
                   wsn430-v1_4 z1 pic32-wifire pic32-clicker jiminy-mega256rfr2

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

include $(CURDIR)/../../Makefile.include

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
