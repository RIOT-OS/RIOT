include ../Makefile.tests_common

FEATURES_REQUIRED = periph_timer

USEMODULE += core_thread_flags

# optionally configure timer under test. Defaults to xtimer config.
# TEST_TIMER_DEV takes a 0-based index to the periph_timer instance defined in
# the board's periph_conf.h. TEST_TIMER_FREQ as an integer number which will
# be used as "freq" parameter in the timer_init() call.
# TEST_TIMER_WIDTH defines the timer width in number of bits.
# Note: not all implementations support arbitrary frequencies.
#CFLAGS += -DTEST_TIMER_DEV=foo -DTEST_TIMER_FREQ=bar -DTEST_TIMER_WIDTH=n

# this test currently fails all CI boards and native
TEST_ON_CI_BLACKLIST += all

include $(RIOTBASE)/Makefile.include
