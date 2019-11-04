# application name
APPLICATION = riot_javascript

# default BOARD environment
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

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
