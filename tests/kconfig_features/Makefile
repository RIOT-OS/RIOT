include ../Makefile.tests_common

BOARD_WHITELIST += arduino-duemilanove \
                   arduino-leonardo \
                   arduino-mega2560 \
                   arduino-nano \
                   arduino-uno \
                   atmega1284p \
                   atmega256rfr2-xpro \
                   atmega328p \
                   avr-rss2 \
                   cc1312-launchpad \
                   cc1352-launchpad \
                   cc1352p-launchpad \
                   cc2538dk \
                   cc2650-launchpad \
                   cc2650stk \
                   chronos \
                   derfmega128 \
                   derfmega256 \
                   firefly \
                   frdm-k22f \
                   frdm-k64f \
                   hifive1 \
                   hifive1b \
                   ikea-tradfri \
                   mega-xplained \
                   microduino-corerf \
                   msb-430 \
                   msb-430h \
                   mulle \
                   openlabs-kw41z-mini \
                   openlabs-kw41z-mini-256kib \
                   openmote-b \
                   openmote-cc2538 \
                   pba-d-01-kw2x \
                   phynode-kw41z \
                   remote-pa \
                   remote-reva \
                   remote-revb \
                   samr21-xpro \
                   slstk3401a \
                   slstk3402a \
                   sltb001a \
                   slwstk6000b-slwrb4150a \
                   slwstk6000b-slwrb4162a \
                   slwstk6220a \
                   stk3600 \
                   stk3700 \
                   teensy31 \
                   telosb \
                   usb-kw41z \
                   waspmote-pro \
                   wsn430-v1_3b \
                   wsn430-v1_4 \
                   z1
                   #

all: checks

include $(RIOTBASE)/Makefile.include

# List of variables to compare between Makefile and Kconfig
_VARS_TO_CHECK = BOARD CPU CPU_MODEL CPU_FAM CPU_CORE CPU_ARCH

# Commands to diff in 'check-values'
_CMDS = $(foreach v,$(_VARS_TO_CHECK), $(if $($(v)),info-debug-variable-$(v)))
_CMDS_CONFIG = $(foreach v,$(_VARS_TO_CHECK), $(if $($(v)),info-debug-variable-CONFIG_$(v)))

# Only compare variables that have a value in Makefile
checks: kconfig-features check-values

kconfig-features: $(KCONFIG_OUT_CONFIG)
	@bash -c 'diff <($(MAKE) info-features-provided) \
	    <($(MAKE) dependency-debug-features-provided-kconfig) || \
	    (echo "ERROR: Kconfig features mismatch" && exit 1)'

check-values: $(KCONFIG_OUT_CONFIG)
	@bash -c '(diff <($(MAKE) $(_CMDS) | sort) \
	    <($(MAKE) $(_CMDS_CONFIG) | sort) && echo "SUCCESS: $(_CMDS:info-debug-variable-%=%) values match") || \
	    (echo "ERROR: The value for $(_CMDS:info-debug-variable-%=%) in Kconfig does not match the one in the Makefile" && exit 1)'
