MODULE = $(APPLICATION_MODULE)

DIRS += $(RIOTCPU)/$(CPU) $(RIOTBOARD)/$(BOARD)
DIRS += $(RIOTBASE)/core $(RIOTBASE)/drivers $(RIOTBASE)/sys

# For regular modules, adding files to BLOBS to their Makefile is sufficient to
# create the corresponding headers.
#
# Application modules are different, as they use this makefile to build, thus
# application level variables are not available unless exported.
#
# But exporting e.g., BLOBS, would pre-set the variable for all
# submakefiles.
#
# As workaround, $(RIOTBASE)/Makefile.include passes BLOBS to this
# Makefile as APPLICATION_BLOBS.
BLOBS = $(APPLICATION_BLOBS)

include $(RIOTBASE)/Makefile.base
