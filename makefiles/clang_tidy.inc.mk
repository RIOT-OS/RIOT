.PHONY: clang-tidy

ifneq (, $(filter clang-tidy,$(MAKECMDGOALS)))
ifneq (llvm,$(TOOLCHAIN))
  $(error clang-tidy must be ran with TOOLCHAIN=llvm)
endif
endif

FILE_REFEX = '.([CcHh]|[ch]pp)$$'
DIFF_FILTER ?= 'ACMR'
BASE_BRANCH ?= master

CLANG_TIDY_CHECKS ?= *
CLANG_TIDY_SRCS ?= $(shell git diff --diff-filter=$(DIFF_FILTER) --name-only $(BASE_BRANCH) | grep -E $(FILE_REFEX))

clang-tidy:
	$(Q)cd $(RIOTBASE); \
	clang-tidy -checks=$(CLANG_TIDY_CHECKS) $(CLANG_TIDY_SRCS) -- $(CFLAGS) $(INCLUDES)
