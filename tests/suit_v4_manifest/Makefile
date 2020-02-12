include ../Makefile.tests_common

USEMODULE += suit_v4
USEMODULE += riotboot_hdr
USEMODULE += embunit

# Lots of structs on the stack and crypto verification
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(8*THREAD_STACKSIZE_DEFAULT\)

BLOBS += $(wildcard bin/manifests/manifest*.bin)

PSEUDOMODULES += suit_v4_test
USEMODULE += suit_v4_test

ifeq (native, $(BOARD))
  # Fake some flash support
  CFLAGS += -DFLASHPAGE_SIZE=256 -DFLASHPAGE_NUMOF=16
  FEATURES_PROVIDED += periph_flashpage
endif

FEATURES_REQUIRED += periph_flashpage

BUILDDEPS += create_test_data

include $(RIOTBASE)/Makefile.include
include $(RIOTMAKE)/suit.v4.inc.mk

# Export to use with the test data script
export SUIT_SEC
export SUIT_PUB

create_test_data: $(SUIT_SEC) $(SUIT_PUB_HDR)
	@mkdir -p $(BINDIRBASE)/manifests
	sh create_test_data.sh
