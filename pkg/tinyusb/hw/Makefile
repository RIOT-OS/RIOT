MODULE = tinyusb_hw

ifneq (,$(filter saml21 samd5x samd21,$(CPU)))
  SRC = hw_sam0.c
else
  SRC = hw_$(CPU).c
endif

include $(RIOTBASE)/Makefile.base
