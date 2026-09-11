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

# Everything behind the 'STM32' prefix, as a list of single characters, e.g.
# 'STM32L476RG' becomes 'L 4 7 6 R G'.
_STM32_CHARS   := $(call split_chars,$(CPU_MODEL_UPPERCASE))
_STM32_SUFFIX  := $(wordlist 6,$(words $(_STM32_CHARS)),$(_STM32_CHARS))

# Every model is 'STM32' followed by at least six characters that encode type,
# family, model, pin count and ROM size, e.g. 'STM32L476RG'.
ifneq (STM32,$(subst $(space),,$(wordlist 1,5,$(_STM32_CHARS))))
  $(error Not a valid STM32 CPU_MODEL: '$(CPU_MODEL)')
endif

ifeq (,$(word 6,$(_STM32_SUFFIX)))
  $(error Too short to be a valid STM32 CPU_MODEL: '$(CPU_MODEL)')
endif

# 'MP' is the only type that is spelled with two characters
_STM32_TYPE    := $(if $(filter MP,$(subst $(space),,$(wordlist 1,2,$(_STM32_SUFFIX)))),MP,$(firstword $(_STM32_SUFFIX)))

# The characters that follow the type, which encode family, model, pin count
# and ROM size. Anything left over ('_A' on some models) is kept as is.
_STM32_REST    := $(wordlist $(words x $(call split_chars,$(_STM32_TYPE))),$(words $(_STM32_SUFFIX)),$(_STM32_SUFFIX))

STM32_INFO     := $(_STM32_TYPE) $(word 1,$(_STM32_REST)) \
                  $(word 1,$(_STM32_REST))$(word 2,$(_STM32_REST))$(word 3,$(_STM32_REST)) \
                  $(word 2,$(_STM32_REST)) $(word 3,$(_STM32_REST)) \
                  $(word 4,$(_STM32_REST)) $(word 5,$(_STM32_REST)) \
                  $(subst $(space),,$(wordlist 6,$(words $(_STM32_REST)),$(_STM32_REST)))
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
else ifneq (,$(filter $(CPU_FAM),f7 h7))
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
else ifeq (u3,$(CPU_FAM))
  CPU_CORE = cortex-m33
  SVD_MODEL := STM32U$(STM32_MODEL)
else ifeq (u5,$(CPU_FAM))
  CPU_CORE = cortex-m33
  SVD_MODEL := STM32U$(STM32_MODEL)
else
  $(error Not supported CPU family: '$(CPU_FAM)')
endif
