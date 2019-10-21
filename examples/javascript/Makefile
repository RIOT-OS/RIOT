# application name
APPLICATION = riot_javascript

# default BOARD environment
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo arduino-mega2560 \
                   arduino-nano arduino-uno atmega328p chronos \
                   msb-430 msb-430h telosb waspmote-pro wsn430-v1_3b \
                   wsn430-v1_4 z1 pic32-wifire pic32-clicker \
                   mega-xplained microduino-corerf

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

JS_PATH = $(BINDIR)/js/$(MODULE)

# add directory of generated *.js.h files to include path
CFLAGS += -I$(JS_PATH)

# generate .js.h header files of .js files
JS = $(wildcard *.js)
JS_H = $(JS:%.js=$(JS_PATH)/%.js.h)

BUILDDEPS += $(JS_H) $(JS_PATH)/

include $(RIOTBASE)/Makefile.include

$(JS_PATH)/:
	$(Q)mkdir -p $@

$(JS_H): $(JS_PATH)/%.js.h: %.js | $(JS_PATH)/
	$(Q)xxd -i $< | sed 's/^unsigned/const unsigned/g' > $@
