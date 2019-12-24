# Set the common memory addresses for stm32 MCU family
ROM_START_ADDR ?= 0x08000000
RAM_START_ADDR ?= 0x20000000

# The next block takes care of setting the rigth lengths of RAM and ROM
# for the stm32 family. Most of the CPUs should have been taken into
# account here, so no need to assign the lengths per model.
STM32_INFO := $(shell printf '%s' '$(CPU_MODEL)' | tr 'a-z' 'A-Z' | sed -E -e 's/^STM32(F|L)(0|1|2|3|4|7)([A-Z0-9])([0-9])(.)(.)(_A)?/\1 \2 \2\3\4 \3 \4 \5 \6 \7/')
STM32_TYPE     := $(word 1, $(STM32_INFO))
STM32_FAMILY   := $(word 2, $(STM32_INFO))
STM32_MODEL    := $(word 3, $(STM32_INFO))
STM32_MODEL2   := $(word 4, $(STM32_INFO))
STM32_MODEL3   := $(word 5, $(STM32_INFO))
STM32_PINCOUNT := $(word 6, $(STM32_INFO))
STM32_ROMSIZE  := $(word 7, $(STM32_INFO))
STM32_RAMMOD   := $(word 8, $(STM32_INFO))

ifeq ($(STM32_TYPE), F)
  ifeq ($(STM32_FAMILY), 0)
    ifeq ($(STM32_MODEL2), 3)
      ifeq ($(STM32_MODEL3), 0)
        ifneq (, $(filter $(STM32_ROMSIZE), 4 6))
          RAM_LEN = 4K
        else ifeq ($(STM32_ROMSIZE), 8)
          RAM_LEN = 8K
        else ifeq ($(STM32_ROMSIZE), C)
          RAM_LEN = 32K
        endif
      else ifneq (, $(filter $(STM32_MODEL3), 1 8))
        RAM_LEN = 4K
      endif
    else ifeq ($(STM32_MODEL2), 4)
      RAM_LEN = 6K
    else ifeq ($(STM32_MODEL2), 5)
      RAM_LEN = 8K
    else ifeq ($(STM32_MODEL2), 7)
      ifeq ($(STM32_MODEL3), 0)
        ifeq ($(STM32_ROMSIZE), 6)
          RAM_LEN = 6K
        else ifeq ($(STM32_ROMSIZE), B)
          RAM_LEN = 16K
        endif
      else ifneq (, $(filter $(STM32_MODEL3), 1 2 8))
        RAM_LEN = 16K
      endif
    else ifeq ($(STM32_MODEL2), 9)
      RAM_LEN = 32K
    endif
  else ifeq ($(STM32_FAMILY), 1)
    ifeq ($(STM32_MODEL2), 0)
      ifeq ($(STM32_MODEL3), 0)
        ifneq (, $(filter $(STM32_ROMSIZE), 4 6))
          RAM_LEN = 4K
        else ifneq (, $(filter $(STM32_ROMSIZE), 8 B))
          RAM_LEN = 8K
        else ifeq ($(STM32_ROMSIZE), C)
          RAM_LEN = 24K
        else ifneq (, $(filter $(STM32_ROMSIZE), E D))
          RAM_LEN = 32K
        endif
      else ifneq (, $(filter $(STM32_MODEL3), 1 2))
        ifeq ($(STM32_ROMSIZE), 4)
          RAM_LEN = 4K
        else ifeq ($(STM32_ROMSIZE), 6)
          RAM_LEN = 6K
        else ifeq ($(STM32_ROMSIZE), 8)
          RAM_LEN = 10K
        else ifeq ($(STM32_ROMSIZE), B)
          RAM_LEN = 16K
        else ifeq ($(STM32_ROMSIZE), C)
          RAM_LEN = 32K
        else ifneq (, $(filter $(STM32_ROMSIZE), D E))
          RAM_LEN = 48K
        else ifneq (, $(filter $(STM32_ROMSIZE), F G))
          RAM_LEN = 80K
        endif
      else ifeq ($(STM32_MODEL3), 3)
        ifeq ($(STM32_ROMSIZE), 4)
          RAM_LEN = 6K
        else ifeq ($(STM32_ROMSIZE), 6)
          RAM_LEN = 10K
        else ifneq (, $(filter $(STM32_ROMSIZE), 8 B))
          RAM_LEN = 20K
        else ifneq (, $(filter $(STM32_ROMSIZE), C))
          RAM_LEN = 48K
        else ifneq (, $(filter $(STM32_ROMSIZE), D E))
          RAM_LEN = 64K
        else ifneq (, $(filter $(STM32_ROMSIZE), F G))
          RAM_LEN = 96K
        endif
      else ifneq (, $(filter $(STM32_MODEL), 105 107))
        RAM_LEN = 64K
      endif
    endif
  else ifeq ($(STM32_FAMILY), 2)
    ifeq ($(STM32_MODEL3), 5)
      ifeq ($(STM32_ROMSIZE), B)
        RAM_LEN = 64K
      else ifeq ($(STM32_ROMSIZE), C)
        RAM_LEN = 96K
      else ifeq (, $(filter $(STM32_ROMSIZE), E F G))
        RAM_LEN = 128K
      endif
    else ifeq ($(STM32_MODEL3), 7)
      RAM_LEN = 128K
    endif
  else ifeq ($(STM32_FAMILY), 3)
    ifeq ($(STM32_MODEL), 301)
      RAM_LEN = 16K
    else ifeq ($(STM32_MODEL), 302)
      ifeq ($(STM32_ROMSIZE), 6)
        RAM_LEN = 32K
      else ifeq ($(STM32_ROMSIZE), 8)
        RAM_LEN = 64K
      else ifeq ($(STM32_ROMSIZE), B)
        RAM_LEN = 128K
      else ifeq ($(STM32_ROMSIZE), C)
        RAM_LEN = 256K
      else ifeq ($(STM32_ROMSIZE), D)
        RAM_LEN = 384K
      else ifeq ($(STM32_ROMSIZE), E)
        RAM_LEN = 512K
      endif
    else ifeq ($(STM32_MODEL), 303)
      ifneq (, $(filter $(STM32_ROMSIZE), 6 8))
        RAM_LEN = 12K
        CCMRAM_LEN = 4K
      else ifeq ($(STM32_ROMSIZE), B)
        RAM_LEN = 40K
        CCMRAM_LEN = 8K
      else ifeq ($(STM32_ROMSIZE), C)
        RAM_LEN = 40K
        CCMRAM_LEN = 8K
      else ifneq (, $(filter $(STM32_ROMSIZE), D E))
        RAM_LEN = 64K
        CCMRAM_LEN = 16K
      endif
    else ifeq ($(STM32_MODEL3), 4)
      RAM_LEN = 12K
      CCMRAM_LEN = 4K
    else ifeq ($(STM32_MODEL), 373)
      RAM_LEN = 32K
    else ifeq ($(STM32_MODEL3), 8)
      ifneq (, $(filter $(STM32_MODEL2), 1 2))
        RAM_LEN = 16K
        ifeq ($(STM32_MODEL2), 1)
          CCMRAM_LEN = 4K
        endif
      else ifeq ($(STM32_MODEL2), 5)
        RAM_LEN = 48K
        CCMRAM_LEN = 8K
      else ifeq ($(STM32_MODEL2), 7)
        RAM_LEN = 32K
      else ifeq ($(STM32_MODEL2), 9)
        RAM_LEN = 80K
        CCMRAM_LEN = 16K
      endif
  endif
  else ifeq ($(STM32_FAMILY), 4)
    ifeq ($(STM32_MODEL), 401)
      ifneq (, $(filter $(STM32_ROMSIZE), B C))
        RAM_LEN = 64K
      else ifneq (, $(filter $(STM32_ROMSIZE), D E))
        RAM_LEN = 96K
      endif
    else ifneq (, $(filter $(STM32_MODEL), 405 407))
      RAM_LEN = 192K
    else ifeq ($(STM32_MODEL), 410)
      RAM_LEN = 32K
    else ifneq (, $(filter $(STM32_MODEL), 411 446))
      RAM_LEN = 128K
    else ifneq (, $(filter $(STM32_MODEL), 412 427 429 437 439))
      RAM_LEN = 256K
    else ifneq (, $(filter $(STM32_MODEL), 413 423))
      RAM_LEN = 320K
    else ifneq (, $(filter $(STM32_MODEL), 415 417))
      RAM_LEN = 192K
    else ifneq (, $(filter $(STM32_MODEL), 469 479))
      RAM_LEN = 384K
    endif
    ifneq (, $(filter $(STM32_MODEL3), 5 7 9))
      CCMRAM_LEN = 64K
    endif
  else ifeq ($(STM32_FAMILY),7)
    ifneq (, $(filter $(STM32_MODEL2), 2 3))
      RAM_LEN = 256K
    else ifneq (, $(filter $(STM32_MODEL2), 4 5))
      RAM_LEN = 320K
    else ifneq (, $(filter $(STM32_MODEL2), 6 7))
      RAM_LEN = 512K
    endif
  endif
else ifeq ($(STM32_TYPE), L)
  ifeq ($(STM32_FAMILY), 0)
    ifeq ($(STM32_MODEL2), 1)
      RAM_LEN = 2K
      ifeq ($(STM32_MODEL3), 0)
        ifeq ($(STM32_ROMSIZE), 6)
          RAM_LEN = 8K
        endif
      endif
    else ifneq (, $(filter $(STM32_MODEL2), 2))
      RAM_LEN = 2K
    else ifneq (, $(filter $(STM32_MODEL2), 3 4 5 6))
      RAM_LEN = 8K
    else ifneq (, $(filter $(STM32_MODEL2), 7 8))
      RAM_LEN = 20K
    endif
  else ifeq ($(STM32_FAMILY), 1)
    ifeq ($(STM32_MODEL), 100)
      ifeq ($(STM32_ROMSIZE), 6)
        RAM_LEN = 4K
      else ifeq ($(STM32_ROMSIZE), 8)
        RAM_LEN = 8K
      else ifeq ($(STM32_ROMSIZE)$(STM32_RAMMOD), B)
        RAM_LEN = 10K
      else ifneq (, $(filter $(STM32_ROMSIZE)$(STM32_RAMMOD), B_A C))
        RAM_LEN = 16K
      endif
    else ifneq (, $(filter $(STM32_MODEL), 151 152))
      ifneq (, $(filter $(STM32_PINCOUNT), C Q U V Z))
        ifneq (, $(filter $(STM32_ROMSIZE)$(STM32_RAMMOD), 6 8))
          RAM_LEN = 10K
        else ifneq (, $(filter $(STM32_ROMSIZE)$(STM32_RAMMOD), 6_A B))
          RAM_LEN = 16K
        else ifneq (, $(filter $(STM32_ROMSIZE)$(STM32_RAMMOD), 8_A B_A C C_A))
          RAM_LEN = 32K
        else ifeq ($(STM32_ROMSIZE), D)
          RAM_LEN = 48K
        else ifeq ($(STM32_ROMSIZE), E)
          RAM_LEN = 80K
        endif
      else ifeq ($(STM32_PINCOUNT), R)
        ifeq ($(STM32_ROMSIZE)$(STM32_RAMMOD), 6)
          RAM_LEN = 10K
        else ifneq (, $(filter $(STM32_ROMSIZE)$(STM32_RAMMOD), 8_A B_A C C_A))
          RAM_LEN = 32K
        else ifneq (, $(filter $(STM32_ROMSIZE)$(STM32_RAMMOD), B 6_A))
          RAM_LEN = 16K
        else ifneq (, $(filter $(STM32_ROMSIZE)$(STM32_RAMMOD), D))
          RAM_LEN = 48K
        else ifneq (, $(filter $(STM32_ROMSIZE)$(STM32_RAMMOD), D_X E))
          RAM_LEN = 80K
        endif
      endif
    endif
  else ifeq ($(STM32_FAMILY), 4)
    ifeq ($(STM32_MODEL2), 3)
      RAM_LEN = 64K
    else ifeq ($(STM32_MODEL2), 7)
      RAM_LEN = 96K
      RAM2_LEN = 32K
    else ifeq ($(STM32_MODEL2), 5)
      RAM_LEN = 160K
    else ifeq ($(STM32_MODEL2), 9)
      RAM_LEN = 320K
    else ifeq ($(STM32_MODEL2), R)
      RAM_LEN = 640K
    endif
  endif
endif

ifeq ($(RAM_LEN), )
  $(warning Unsupported cpu model $(CPU_MODEL) automatically)
endif

ifeq ($(STM32_ROMSIZE), 4)
  ROM_LEN = 16K
else ifeq ($(STM32_ROMSIZE), 6)
  ROM_LEN = 32K
else ifeq ($(STM32_ROMSIZE), 8)
  ROM_LEN = 64K
else ifeq ($(STM32_ROMSIZE), B)
  ROM_LEN = 128K
else ifeq ($(STM32_ROMSIZE), Z)
  ROM_LEN = 192K
else ifeq ($(STM32_ROMSIZE), C)
  ROM_LEN = 256K
else ifeq ($(STM32_ROMSIZE), D)
  ROM_LEN = 384K
else ifeq ($(STM32_ROMSIZE), E)
  ROM_LEN = 512K
else ifeq ($(STM32_ROMSIZE), F)
  ROM_LEN = 768K
else ifeq ($(STM32_ROMSIZE), G)
  ROM_LEN = 1024K
else ifeq ($(STM32_ROMSIZE), H)
  ROM_LEN = 1536K
else ifeq ($(STM32_ROMSIZE), I)
  ROM_LEN = 2048K
endif

ifeq ($(STM32_PINCOUNT), A)
  STM32_PINCOUNT = 169
else ifeq ($(STM32_PINCOUNT), B)
  STM32_PINCOUNT = 208
else ifeq ($(STM32_PINCOUNT), C)
  STM32_PINCOUNT = 48
else ifeq ($(STM32_PINCOUNT), F)
  STM32_PINCOUNT = 20
else ifeq ($(STM32_PINCOUNT), G)
  STM32_PINCOUNT = 28
else ifeq ($(STM32_PINCOUNT), H)
  STM32_PINCOUNT = 40
else ifeq ($(STM32_PINCOUNT), I)
  STM32_PINCOUNT = 176
else ifeq ($(STM32_PINCOUNT), J)
  STM32_PINCOUNT = 72
else ifeq ($(STM32_PINCOUNT), K)
  STM32_PINCOUNT = 32
else ifeq ($(STM32_PINCOUNT), M)
  STM32_PINCOUNT = 81
else ifeq ($(STM32_PINCOUNT), N)
  STM32_PINCOUNT = 216
else ifeq ($(STM32_PINCOUNT), Q)
  STM32_PINCOUNT = 132
else ifeq ($(STM32_PINCOUNT), R)
  STM32_PINCOUNT = 64
else ifeq ($(STM32_PINCOUNT), T)
  STM32_PINCOUNT = 36
else ifeq ($(STM32_PINCOUNT), U)
  STM32_PINCOUNT = 63
else ifeq ($(STM32_PINCOUNT), V)
  STM32_PINCOUNT = 100
else ifeq ($(STM32_PINCOUNT), Z)
  STM32_PINCOUNT = 144
endif
