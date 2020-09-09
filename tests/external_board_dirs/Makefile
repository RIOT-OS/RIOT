include ../Makefile.tests_common

# Require arch_native feature so this is not compiled for other boards in
# $(RIOTBOARD)/
FEATURES_REQUIRED += arch_native

# Set without '?=' to also verify the docker integration when set with =
EXTERNAL_BOARD_DIRS = $(CURDIR)/external_board_dir_1
EXTERNAL_BOARD_DIRS += $(CURDIR)/external_board_dir_2

include $(RIOTBASE)/Makefile.include
