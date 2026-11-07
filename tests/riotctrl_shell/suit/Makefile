USEMODULE += shell_cmd_suit
USEMODULE += suit_transport_mock

SUIT_SEC = mock_suit_sec  # prevent SUIT key generation

include $(CURDIR)/../Makefile.riotctrl_shell_common

CFLAGS += -D"SUIT_TRANSPORT_MOCK_APP_MANIFEST"

mock_suit_sec:
	$(Q)true
