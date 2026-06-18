# SPDX-FileCopyrightText: 2026 HAW Hamburg
# SPDX-License-Identifier: LGPL-2.1-only

# Central memory layout for nRF9160 TrustZone configuration.
# This is the single source of truth for all flash/RAM partitioning.
# All other files (linker scripts, SPU config, RIOT app offsets) derive from these values.

# === nRF9160 chip constants ===
NRF_FLASH_BASE        := 0x0
NRF_FLASH_SIZE        := 0x100000
NRF_FLASH_REGION_SIZE := 0x8000
NRF_RAM_BASE          := 0x20000000
NRF_RAM_SIZE          := 0x40000
NRF_RAM_REGION_SIZE   := 0x2000

# === Layout inputs ===
BL_FLASH_SIZE         ?= 0x0
TEE_FLASH_SIZE        ?= 0x20000 # needs to be 32KB aligned
TEE_RAM_SIZE          ?= 0x16000 # needs to be 8KB aligned

# === Derived values ===
SECURE_FLASH_SIZE     := $(shell printf "0x%X" $$(($(BL_FLASH_SIZE) + $(TEE_FLASH_SIZE))))
TEE_FLASH_START       := $(BL_FLASH_SIZE)
NS_FLASH_START        := $(SECURE_FLASH_SIZE)
NS_FLASH_SIZE         := $(shell printf "0x%X" $$(($(NRF_FLASH_SIZE) - $(SECURE_FLASH_SIZE))))
NS_RAM_START          := $(shell printf "0x%X" $$(($(NRF_RAM_BASE) + $(TEE_RAM_SIZE))))
NS_RAM_SIZE           := $(shell printf "0x%X" $$(($(NRF_RAM_SIZE) - $(TEE_RAM_SIZE))))

# === Alignment checks ===
$(if $(shell [ $$(($(SECURE_FLASH_SIZE) % $(NRF_FLASH_REGION_SIZE))) -ne 0 ] && echo fail),\
  $(error SECURE_FLASH_SIZE ($(SECURE_FLASH_SIZE)) must be aligned to $(NRF_FLASH_REGION_SIZE)))
$(if $(shell [ $$(($(TEE_RAM_SIZE) % $(NRF_RAM_REGION_SIZE))) -ne 0 ] && echo fail),\
  $(error TEE_RAM_SIZE ($(TEE_RAM_SIZE)) must be aligned to $(NRF_RAM_REGION_SIZE)))

# SPU region indices
FIRST_NS_FLASH_REGION := $(shell echo $$(($(SECURE_FLASH_SIZE) / $(NRF_FLASH_REGION_SIZE))))
FIRST_NS_RAM_REGION   := $(shell echo $$(($(TEE_RAM_SIZE) / $(NRF_RAM_REGION_SIZE))))
NSC_FLASH_REGION      := $(shell echo $$(($(FIRST_NS_FLASH_REGION) - 1)))
