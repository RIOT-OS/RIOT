MODULE = $(APPLICATION)

DIRS += $(RIOTCPU) $(RIOTBOARD)
DIRS += $(RIOTBASE)/core $(RIOTBASE)/drivers $(RIOTBASE)/sys

include $(RIOTBASE)/Makefile.base
