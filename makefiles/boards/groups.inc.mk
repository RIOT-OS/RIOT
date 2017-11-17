include $(RIOTBOARD)/groups.inc.mk

BOARD_GROUP.8bit += $(BOARD_GROUP.avr8)
BOARD_GROUP.16bit += $(BOARD_GROUP.msp430)
BOARD_GROUP.32bit += $(sort $(BOARD_GROUP.arm7) $(BOARD_GROUP.cortexm) $(BOARD_GROUP.mips) $(BOARD_GROUP.native))

BOARD_GROUPS.arch += \
  arm7 \
  avr8 \
  cortexm \
  native \
  mips \
  msp430 \
  #

BOARD_GROUPS += $(BOARD_GROUPS.arch) 8bit 16bit 32bit

ALL_BOARDS := $(foreach group,$(BOARD_GROUPS.arch),$(BOARD_GROUP.$(group)))
