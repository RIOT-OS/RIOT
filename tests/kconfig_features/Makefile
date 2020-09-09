include ../Makefile.tests_common

all: checks

include $(RIOTBASE)/Makefile.include

# List of variables to compare between Makefile and Kconfig
_VARS_TO_CHECK = BOARD CPU CPU_MODEL CPU_FAM CPU_CORE CPU_ARCH

# Commands to diff in 'check-values'
_CMDS = $(foreach v,$(_VARS_TO_CHECK), $(if $($(v)),info-debug-variable-$(v)))
_CMDS_CONFIG = $(foreach v,$(_VARS_TO_CHECK), $(if $($(v)),info-debug-variable-CONFIG_$(v)))

# Only compare variables that have a value in Makefile
checks: kconfig-features check-values

kconfig-features: $(KCONFIG_OUT_CONFIG)
	@bash -c 'diff <($(MAKE) info-features-provided) \
	    <($(MAKE) dependency-debug-features-provided-kconfig) || \
	    (echo "ERROR: Kconfig features mismatch" && exit 1)'

check-values: $(KCONFIG_OUT_CONFIG)
	@bash -c '(diff <($(MAKE) $(_CMDS) | sort) \
	    <($(MAKE) $(_CMDS_CONFIG) | sort) && echo "SUCCESS: $(_CMDS:info-debug-variable-%=%) values match") || \
	    (echo "ERROR: The value for $(_CMDS:info-debug-variable-%=%) in Kconfig does not match the one in the Makefile" && exit 1)'
