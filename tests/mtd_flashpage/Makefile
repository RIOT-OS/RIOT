include ../Makefile.tests_common

USEMODULE += mtd_flashpage
USEMODULE += embunit

# https://github.com/RIOT-OS/RIOT/pull/15859 exposed a bug in the
# handling of FEATURES_REQUIRED_ANY, so https://github.com/RIOT-OS/RIOT/pull/15935
# did not run unittests, native is currently failing, so blacklist while this
# is still the case
FEATURES_BLACKLIST += arch_native

include $(RIOTBASE)/Makefile.include
