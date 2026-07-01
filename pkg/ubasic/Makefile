PKG_NAME=ubasic
PKG_URL=https://github.com/adamdunkels/ubasic
PKG_VERSION=cc07193c231e21ecb418335aba5b199a08d4685c
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

UBASIC_MODULES   = ubasic_tests
UBASIC_USEMODULE = $(filter $(UBASIC_MODULES),$(USEMODULE))

.PHONY: ubasic ubasic%

all: ubasic

make_module = +$(QQ)"$(MAKE)" -f $(RIOTPKG)/ubasic/$(1).mk -C $(2)

ubasic: $(UBASIC_USEMODULE)
	$(call make_module,$@,$(PKG_SOURCE_DIR))

ubasic%:
	$(call make_module,$@,$(PKG_SOURCE_DIR))
