USEMODULE += shell_cmd_suit
USEMODULE += suit_transport_mock

SUIT_SEC = mock_suit_sec  # prevent SUIT key generation

include $(CURDIR)/../Makefile.riotctrl_shell_common

CFLAGS += -D"TESTSUITE_SUIT_CMD_TEST"

mock_suit_sec:
	$(Q)true
