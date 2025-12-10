include variables.mk

# Timestamp in nanoseconds (to be an integer)
# OSx 'date' does not support 'date +%s%N' so rely on python instead
# It could be OSx specific but we do not have 'OS' defined here to differentiate
date_nanoseconds = $(shell python3 -c 'import time; print(int(time.time() * 1000000000))')

EXPORTED_VARIABLES = MY_VARIABLE CURRENT_TIME
MY_VARIABLE = my_variable
# Defered evaluation to the test
CURRENT_TIME = $(call date_nanoseconds)

$(call target-export-variables,test-exported-variables,$(EXPORTED_VARIABLES))
test-exported-variables:
	$(Q)test "$(MY_VARIABLE)" = "$${MY_VARIABLE}" || { echo ERROR: "$(MY_VARIABLE)" != "$${MY_VARIABLE}"; exit 1; }
	$(Q)test $(PARSE_TIME) -lt $${CURRENT_TIME} || { echo ERROR: $(PARSE_TIME) \>= $${CURRENT_TIME} >&2; exit 1; }


MEMOIZED_CURRENT_TIME = $(call memoized,MEMOIZED_CURRENT_TIME,$(call date_nanoseconds))
MEMOIZED_CURRENT_TIME_2 = $(call memoized,MEMOIZED_CURRENT_TIME_2,$(call date_nanoseconds))

PRE_MEMOIZED_TIME := $(call date_nanoseconds)
# Two separate evaluations
REF_CURRENT_TIME_1 := $(MEMOIZED_CURRENT_TIME)
# Strip to detect added whitespaces by function
REF_CURRENT_TIME_2 := $(strip $(MEMOIZED_CURRENT_TIME))

test-memoized-variables:
	@# The value was only evaluated on first use
	$(Q)test $(PRE_MEMOIZED_TIME) -lt $(REF_CURRENT_TIME_1) || { echo ERROR: $(PRE_MEMOIZED_TIME) \>= $(REF_CURRENT_TIME_1) >&2; exit 1; }
	@# Both evaluation return the same time and without added whitespace
	$(Q)test "$(REF_CURRENT_TIME_1)" = "$(REF_CURRENT_TIME_2)" || { echo ERROR: "$(REF_CURRENT_TIME_1)" != "$(REF_CURRENT_TIME_2)" >&2; exit 1; }
	@# The second memoized value was only evaluated when calling the target
	$(Q)test $(PARSE_TIME) -lt $(MEMOIZED_CURRENT_TIME_2) || { echo ERROR: $(PARSE_TIME) \>= $(MEMOIZED_CURRENT_TIME_2) >&2; exit 1; }


# Immediate evaluation for comparing
PARSE_TIME := $(call date_nanoseconds)
