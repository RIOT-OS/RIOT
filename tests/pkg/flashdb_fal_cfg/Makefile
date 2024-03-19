include ../Makefile.pkg_common

# select the MTD backend
USEMODULE += flashdb_mtd
# enable key-value database
USEMODULE += flashdb_kvdb
# enable time series database
USEMODULE += flashdb_tsdb
# rtc_localtime()
USEMODULE += rtc_utils

# prefer periph_rtc over periph_rtt
FEATURES_OPTIONAL += periph_rtc
FEATURES_REQUIRED_ANY += periph_rtc|periph_rtt

CFLAGS += -DFAL_PART1_LABEL=\"part1\"
CFLAGS += -DFAL_PART1_LENGTH=FAL_PART0_LENGTH
CFLAGS += -DFAL_PART2_LABEL=\"part2\"
CFLAGS += -DFAL_PART2_LENGTH=FAL_PART0_LENGTH
CFLAGS += -DFAL_PART3_LABEL=\"part3\"
CFLAGS += -DFAL_PART3_LENGTH=FAL_PART0_LENGTH

include $(RIOTBASE)/Makefile.include

# handle RTC backend after inclusion of $(RIOTBASE)/Makefile.include
ifeq (,$(filter periph_rtc,$(FEATURES_USED)))
  USEMODULE += rtt_rtc
  USEMODULE += ztimer_no_periph_rtt
endif
