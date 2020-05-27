ifneq (,$(filter $(CPU_FAM),f2 f4 f7))
  # STM32F2/4/7 uses sectors instead of pages, where the minimum sector length is 16KB
  # or 32KB (the first sector), depending on the CPU_MODEL. Therefore RIOTBOOT_LEN must
  # be 16KB or 32kB to cover a whole sector.
  ifneq (,$(filter $(CPU_FAM),f2 f4))
    RIOTBOOT_LEN ?= 0x4000
  else ifneq (,$(filter stm32f722ze,$(CPU_MODEL)))
    RIOTBOOT_LEN ?= 0x4000
  else  # others stm32f7
    RIOTBOOT_LEN ?= 0x8000
  endif

  # CPU_IRQ_NUMOF for STM32F2 boards is < 81+16 so (97*4 bytes = 388 bytes ~= 0x200)
  # CPU_IRQ_NUMOF for STM32F4 boards is < 102+16 so (118*4 bytes = 472 bytes ~= 0x200)
  # CPU_IRQ_NUMOF for STM32F7 boards is < 110+16 so (126*4 bytes = 504 bytes ~= 0x200)
  # RIOTBOOT_HDR_LEN can be set to 0x200.
  # Details on alignment requirements for M3 in `cpu/cortexm_common/Makefile.include`.
  RIOTBOOT_HDR_LEN ?= 0x200

  # Sectors don't have the same length. Per bank there can be up to 12 sectors. The
  # first 4 sectors are 16kB long, the 5th is 64kB and the remaining 7 are 128kB.
  # Since flash can only be erased by sector, slots can't overlap over sectors.
  # The most efficient layout comes from removing RIOTBOOT_LEN twice, once at the
  # start of the flash for the bootloader, and a second time at the end of the
  # flash, to get evenly sized and distributed slots.
  SLOT0_LEN ?= $(shell printf "0x%x" $$((($(ROM_LEN:%K=%*1024)-2*$(RIOTBOOT_LEN)) / $(NUM_SLOTS))))
  SLOT1_LEN ?= $(SLOT0_LEN)
else ifeq (stm32l4,$(CPU_FAM))
  # "The Vector table must be naturally aligned to a power of two whose alignment
  # value is greater than or equal to number of Exceptions supported x 4"
  # CPU_IRQ_NUMOFF for stm32l4 boards is < 91+16 so (107*4 bytes = 428 bytes ~= 0x200)
  # RIOTBOOT_HDR_LEN can be set to 0x200
  RIOTBOOT_HDR_LEN ?= 0x200
else ifeq (stm32wb,$(CPU_FAM))
  # "The Vector table must be naturally aligned to a power of two whose alignment
  # value is greater than or equal to number of Exceptions supported x 4"
  # CPU_IRQ_NUMOFF for stm32l4 boards is < 91+16 so (107*4 bytes = 428 bytes ~= 0x200)
  # RIOTBOOT_HDR_LEN can be set to 0x200
  RIOTBOOT_HDR_LEN ?= 0x200

  # Slot size is determined by "((total_flash_size - RIOTBOOT_LEN) / 2)".
  # If RIOTBOOT_LEN uses an odd number of flashpages, the remainder of the
  # flash cannot be divided by two slots while staying FLASHPAGE_SIZE aligned.
  RIOTBOOT_LEN ?= 0x2000
endif
