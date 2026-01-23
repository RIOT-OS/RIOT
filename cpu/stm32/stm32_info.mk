# Extract STM32 infos from CPU_MODEL
# Example for STM32L476RG:
#  - STM32_TYPE: L
#  - STM32_FAMILY: 4
#  - STM32_MODEL: 476
#  - STM32_MODEL2: 7
#  - STM32_MODEL3: 6
#  - STM32_PINCOUNT: R (64)
#  - STM32_ROMSIZE: G (1024K)
CPU_MODEL_UPPERCASE = $(call uppercase,$(CPU_MODEL))
STM32_INFO     := $(shell echo $(CPU_MODEL_UPPERCASE) | sed -E -e 's/^STM32(F|L|W|G|MP|U|C)([0-7]|B|L)([A-Z0-9])([0-9])(.)(.)?(_A)?/\1 \2 \2\3\4 \3 \4 \5 \6 \7/')
STM32_TYPE     = $(word 1, $(STM32_INFO))
STM32_FAMILY   = $(word 2, $(STM32_INFO))
STM32_MODEL    = $(word 3, $(STM32_INFO))
ifneq (,$(filter W,$(STM32_TYPE)))
  STM32_MODEL2   = $(word 4, $(STM32_INFO))
  STM32_MODEL3   = $(word 5, $(STM32_INFO))
  STM32_PINCOUNT = $(word 6, $(STM32_INFO))
  STM32_ROMSIZE  = $(word 7, $(STM32_INFO))
else ifneq (,$(filter MP,$(STM32_TYPE)))
  STM32_MODEL2   = $(word 6, $(STM32_INFO))
  STM32_PINCOUNT = $(word 7, $(STM32_INFO))$(word 8, $(STM32_INFO))
else
  STM32_MODEL2   = $(word 4, $(STM32_INFO))
  STM32_MODEL3   = $(word 5, $(STM32_INFO))
  STM32_PINCOUNT = $(word 6, $(STM32_INFO))
  STM32_ROMSIZE  = $(word 7, $(STM32_INFO))
  STM32_RAMMOD   = $(word 8, $(STM32_INFO))
endif

CPU_FAM = $(call lowercase,$(STM32_TYPE)$(STM32_FAMILY))
SVD_VENDOR := STMicro

ifeq (f0,$(CPU_FAM))
  CPU_CORE = cortex-m0
  ifneq (,$(filter $(STM32_MODEL),031 042 072 091))
    SVD_MODEL := STM32F$(STM32_MODEL)x
  endif
  ifeq (030,$(STM32_MODEL))
    SVD_MODEL := STM32F$(STM32_MODEL)
  endif
else ifeq (f1,$(CPU_FAM))
  CPU_CORE = cortex-m3
  SVD_MODEL := STM32F$(STM32_MODEL)xx
else ifeq (f2,$(CPU_FAM))
  CPU_CORE = cortex-m3
  SVD_MODEL := STM32F2$(STM32_MODEL2)x
else ifeq (l1,$(CPU_FAM))
  CPU_CORE = cortex-m3
  # TODO: Memory map description is split over multiple SVD files, but this
  # is not yet supported by the build system. We load the common STM32L1xx
  # SVD file only for now
	SVD_MODEL := STM32L1xx
else ifeq (f3,$(CPU_FAM))
  CPU_CORE = cortex-m4f
  ifneq (,$(filter $(STM32_MODEL3),4 8))
    SVD_MODEL := STM32F3x$(STM32_MODEL3)
  else
    SVD_MODEL := STM32F$(STM32_MODEL)
  endif
else ifeq (f4,$(CPU_FAM))
  CPU_CORE = cortex-m4f
  SVD_MODEL := STM32F$(STM32_MODEL)
else ifeq (l4,$(CPU_FAM))
  CPU_CORE = cortex-m4f
  SVD_MODEL := STM32L4x$(STM32_MODEL3)
else ifeq (mp1,$(CPU_FAM))
  CPU_CORE = cortex-m4f
else ifeq (g4,$(CPU_FAM))
  CPU_CORE = cortex-m4
  SVD_MODEL := STM32G$(STM32_MODEL)xx
else ifneq (,$(filter $(CPU_FAM),wb wl))
  CPU_CORE = cortex-m4
else ifeq (f7,$(CPU_FAM))
  CPU_CORE = cortex-m7
	SVD_MODEL := STM32F7x$(STM32_MODEL3)
else ifeq (g0,$(CPU_FAM))
  CPU_CORE = cortex-m0plus
  SVD_MODEL := STM32G$(STM32_MODEL)
else ifeq (l0,$(CPU_FAM))
  CPU_CORE = cortex-m0plus
  SVD_MODEL := STM32L0x$(STM32_MODEL3)
else ifeq (c0,$(CPU_FAM))
  CPU_CORE = cortex-m0plus
else ifeq (l5,$(CPU_FAM))
  CPU_CORE = cortex-m33
  SVD_MODEL := STM32L$(STM32_MODEL)
else ifeq (u5,$(CPU_FAM))
  CPU_CORE = cortex-m33
  SVD_MODEL := STM32U$(STM32_MODEL)
else
  $(error Not supported CPU family: '$(CPU_FAM)')
endif
