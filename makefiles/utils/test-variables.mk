include variables.mk

# Timestamp in nanoseconds (to be an integer)
# OSx 'date' does not support 'date +%s%N' so rely on python instead
# It could be OSx specific but we do not have 'OS' defined here to differentiate
date_nanoseconds = $(shell python -c 'import time; print(int(time.time() * 1000000000))')

EXPORTED_VARIABLES = MY_VARIABLE CURRENT_TIME
MY_VARIABLE = my_variable
# Defered evaluation to the test
CURRENT_TIME = $(call date_nanoseconds)

$(call target-export-variables,test-exported-variables,$(EXPORTED_VARIABLES))
test-exported-variables:
	$(Q)bash -c 'test "$(MY_VARIABLE)" = "$${MY_VARIABLE}" || { echo ERROR: "$(MY_VARIABLE)" != "$${MY_VARIABLE}"; exit 1; }'
	$(Q)bash -c 'test $(PARSE_TIME) -lt $${CURRENT_TIME} || { echo ERROR: $(PARSE_TIME) \>= $${CURRENT_TIME} >&2; exit 1; }'

# Immediate evaluation for comparing
PARSE_TIME := $(call date_nanoseconds)
