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
                   cc2650-launchpad \
                   cc2650stk \
                   chronos \
                   derfmega128 \
                   derfmega256 \
                   ikea-tradfri \
                   mega-xplained \
                   microduino-corerf \
                   msb-430 \
                   msb-430h \
                   samr21-xpro \
                   slstk3401a \
                   slstk3402a \
                   sltb001a \
                   slwstk6000b-slwrb4150a \
                   slwstk6000b-slwrb4162a \
                   stk3600 \
                   stk3700 \
                   telosb \
                   waspmote-pro \
                   wsn430-v1_3b \
                   wsn430-v1_4 \
                   z1
                   #

all: checks

include $(RIOTBASE)/Makefile.include

# List of variables to compare between Makefile and Kconfig
_VARS_TO_CHECK = BOARD CPU CPU_MODEL CPU_FAM CPU_CORE CPU_ARCH

# Only compare variables that have a value in Makefile
checks: kconfig-features $(foreach v,$(_VARS_TO_CHECK), $(if $($(v)),check-value-$(v)))

kconfig-features: $(KCONFIG_OUT_CONFIG)
	@bash -c 'diff <($(MAKE) info-features-provided) \
	    <($(MAKE) dependency-debug-features-provided-kconfig) || \
	    (echo "ERROR: Kconfig features mismatch" && exit 1)'

check-value-%: $(KCONFIG_OUT_CONFIG)
	@bash -c '(diff <($(MAKE) info-debug-variable-$*) \
	    <($(MAKE) info-debug-variable-CONFIG_$*) && echo "SUCCESS: $* values match") || \
	    (echo "ERROR: The value for $* in Kconfig does not match the one in the Makefile" && exit 1)'
