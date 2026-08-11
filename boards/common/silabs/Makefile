MODULE = boards_common_silabs

MODULE_DEFS_NEEDED += \
  silabs_aem \
  silabs_bc \
  silabs_pic \
  #

ifneq (,$(filter silabs_aem,$(USEMODULE)))
  DIRS += drivers/aem
endif

ifneq (,$(filter silabs_bc,$(USEMODULE)))
  DIRS += drivers/bc
endif

ifneq (,$(filter silabs_pic,$(USEMODULE)))
  DIRS += drivers/pic
endif

include $(RIOTBASE)/Makefile.base
