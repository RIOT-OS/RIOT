# Extract SAM0 infos from CPU_MODEL
# Example for saml21e15b
# - SAM0_FAMILY:  l
# - SAM0_SERIES:  21
# - SAM0_PINCNT:  e  (32 Pins)
# - SAM0_DENSITY: 15 (32k Flash)
# - SAM0_VARIANT: b  (release stepping)

SAM0_INFO    := $(shell echo $(CPU_MODEL) | sed -E -e 's/^sam(d|e|l|r)([0-9][0-9])([a-z])([0-9][0-9])(.)?/\1 \2 \3 \4 \5/')
SAM0_FAMILY  := $(word 1, $(SAM0_INFO))
SAM0_SERIES  := $(word 2, $(SAM0_INFO))
SAM0_PINCNT  := $(word 3, $(SAM0_INFO))
SAM0_DENSITY := $(word 4, $(SAM0_INFO))
SAM0_VARIANT := $(word 5, $(SAM0_INFO))
