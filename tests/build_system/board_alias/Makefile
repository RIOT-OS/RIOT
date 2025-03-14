include ../Makefile.build_system_common

include $(RIOTBASE)/Makefile.include

# override info-boards-supported target with a list of existing board aliases
# to automatically test all of them in the CI
info-boards-supported:
	@echo $(foreach alias,$(BOARD_ALIAS), $(firstword $(subst :, ,$(alias))))
