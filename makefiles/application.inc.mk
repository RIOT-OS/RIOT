MODULE = $(APPLICATION)

DIRS += $(RIOTCPU)/$(CPU) $(RIOTBOARD)
DIRS += $(RIOTBASE)/core $(RIOTBASE)/drivers $(RIOTBASE)/sys

include $(RIOTBASE)/Makefile.base
