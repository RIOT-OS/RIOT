MODULE := vendor

# Enable submodules, although these are all compiled into the same "vendor.a"
# library since vendor_% is marked as a pseudomodule.
SUBMODULES := 1
SUBMODULES_NOFORCE := 1

CFLAGS  += -Wno-unused-parameter -Wno-sign-compare
INCLUDES += \
    -I$(RIOTCPU)/$(CPU)/include/vendor/ \
    -I$(RIOTCPU)/$(CPU)/include/vendor/drivers/ \
    #

include $(RIOTBASE)/Makefile.base
