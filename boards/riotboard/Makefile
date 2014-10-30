MODULE = $(BOARD)_base

DIRS = muxer_config

.PHONY: muxer_config riotboard_default
riotboard_default: muxer_config all

include $(RIOTBASE)/Makefile.base

muxer_config:
	rm -f muxer_config
	ln -s $(RIOTBASE)/pkg/riotboard_SDK/muxer_config

clean::
	rm -f muxer_config
