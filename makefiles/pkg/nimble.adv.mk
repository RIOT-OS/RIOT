# Adds an external Advertisement Instance
# Parameter 1: The value for the advertisement instance without the CONFIG_ prefix
# Result:
#   - Increases BLE_MULTI_ADV_INSTANCES count by 1, note that the actual amount
#     of advertisement instances is MYNEWT_VAL_BLE_MULTI_ADV_INSTANCES + 1, which
#     is why BLE_MULTI_ADV_INSTANCES starts at -1
#   - Sets the adv instance for CONFIG_$1 index to $(BLE_MULTI_ADV_INSTANCES)
BLE_MULTI_ADV_INSTANCES ?= -1
define _add_ext_adv_instance
  # Increase the count in one
  BLE_MULTI_ADV_INSTANCES := $$(shell echo $$$$(($(BLE_MULTI_ADV_INSTANCES) + 1)))
  # Export the definition in CFLAGS
  $(1) := $$(BLE_MULTI_ADV_INSTANCES)
  CFLAGS += -DCONFIG_$(1)=$$($(1))
endef
