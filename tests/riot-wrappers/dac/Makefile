# name of your application
APPLICATION = riot-wrappers-test-dac
APPLICATION_RUST_MODULE = riot_wrappers_test_dac
FEATURES_REQUIRED += periph_dac

# Of these boards it is known that DAC0 may be driven arbitrarily because it's
# just a pin on the extension header. (It's probably true for most boards, but
# so far nobody guarantees that. Just add your board here if your DAC0 pin is
# good to use).
BOARD_WHITELIST = stk3700

include $(RIOTBASE)/Makefile.include
