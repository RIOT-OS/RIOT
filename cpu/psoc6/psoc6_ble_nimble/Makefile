MODULE = psoc6_ble_nimble

NIMBLE_H4_INC_SRC := $(RIOTBASE)/pkg/nimble/nimble/transport/common/hci_h4/include

NIMBLE_H4_INC_BIN := $(BINDIR)/pkg/nimble/nimble/transport/common/hci_h4/include

NIMBLE_H4_INC_GLOBAL := $(RIOTBASE)/build/pkg/nimble/nimble/transport/common/hci_h4/include

INCLUDES += $(if $(wildcard $(NIMBLE_H4_INC_SRC)),-I$(NIMBLE_H4_INC_SRC),)
INCLUDES += $(if $(wildcard $(NIMBLE_H4_INC_BIN)),-I$(NIMBLE_H4_INC_BIN),)
INCLUDES += $(if $(wildcard $(NIMBLE_H4_INC_GLOBAL)),-I$(NIMBLE_H4_INC_GLOBAL),)

include $(RIOTBASE)/Makefile.base
