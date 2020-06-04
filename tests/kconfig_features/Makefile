include ../Makefile.tests_common

BOARD_WHITELIST += samr21-xpro

all: kconfig-features

include $(RIOTBASE)/Makefile.include

kconfig-features: $(KCONFIG_OUT_CONFIG)
	@bash -c 'diff <($(MAKE) info-features-provided) \
	    <($(MAKE) dependency-debug-features-provided-kconfig) || \
	    (echo "ERROR: Kconfig features mismatch" && exit 1)'
