include $(RIOTBOARD)/groups.inc.mk

MCUGROUP.8bit := $(MCUGROUP.avr8)
MCUGROUP.16bit := $(MCUGROUP.msp430)
MCUGROUP.32bit := $(sort $(MCUGROUP.arm7) $(MCUGROUP.cortexm) $(MCUGROUP.mips) $(MCUGROUP.native))

MCUGROUPS.arch := \
  arm7 \
  avr8 \
  cortexm \
  native \
  mips \
  msp430 \
  #

ALL_BOARDS := $(foreach group,$(MCUGROUPS.arch),$(MCUGROUP.$(group)))
