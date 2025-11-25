ifneq (, $(filter buildtest info-concurrency, $(MAKECMDGOALS)))
  ifeq (, $(strip $(NPROC)))
    # Linux (utility program)
    NPROC := $(shell nproc 2>/dev/null)

    ifeq (, $(strip $(NPROC)))
      # Linux (generic)
      NPROC := $(shell grep -c ^processor /proc/cpuinfo 2>/dev/null)
    endif
    ifeq (, $(strip $(NPROC)))
      # BSD (at least FreeBSD and Mac OSX)
      NPROC := $(shell sysctl -n hw.ncpu 2>/dev/null)
    endif
    ifeq (, $(strip $(NPROC)))
      # Fallback
      NPROC := 1
    endif

    NPROC := $(shell echo $$(($(NPROC) + 1)))

    ifneq (, $(NPROC_MAX))
      NPROC := $(shell if [ $(NPROC) -gt $(NPROC_MAX) ]; then echo $(NPROC_MAX); else echo $(NPROC); fi)
    endif
    ifneq (, $(NPROC_MIN))
      NPROC := $(shell if [ $(NPROC) -lt $(NPROC_MIN) ]; then echo $(NPROC_MIN); else echo $(NPROC); fi)
    endif
  endif
endif

.PHONY: info-concurrency

info-concurrency:
	@echo "$(NPROC)"
