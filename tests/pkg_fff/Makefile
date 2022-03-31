include ../Makefile.tests_common
USEPKG += fff
# If periph_i2c is pulled in the real implementation conflicts with the mock
FEATURES_BLACKLIST += periph_i2c

# only two threads used
CFLAGS += -DMAXTHREADS=2

include $(RIOTBASE)/Makefile.include
