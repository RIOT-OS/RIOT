# Control whether to use PyCortexMDebug
USE_PYCORTEXMDEBUG ?= 0

ifeq (1,$(USE_PYCORTEXMDEBUG))
  DEBUGDEPS += $(RIOTTOOLS)/PyCortexMDebug/checkout
  DBG_EXTRA_FLAGS += --init-command='$(RIOTTOOLS)/PyCortexMDebug/checkout/scripts/gdb.py'

  ifneq (,$(SVD_MODEL))
    DBG_EXTRA_FLAGS += --eval-command='svd_load $(SVD_VENDOR) $(SVD_MODEL)'
  endif
endif
