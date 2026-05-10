PKG_NAME=radiolib
PKG_URL=https://github.com/jgromes/RadioLib
PKG_VERSION=f403b9c3d735bbd467ee92c1baf8ea216b608125
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

RADIOLIB_MODULES = \
                    radiolib_core \
                    radiolib_utils \
                    radiolib_physicallayer \
                    radiolib_lr11x0 \
					#

CXXUWFLAGS += -Wcast-align -Wunused-parameter -Wdeprecated-copy

DIR_radiolib_core          = src
DIR_radiolib_utils         = src/utils
DIR_radiolib_physicallayer = src/protocols/PhysicalLayer
DIR_radiolib_lr11x0        = src/modules/LR11x0

.PHONY: radiolib_%

all: $(RADIOLIB_MODULES)

radiolib_%:
	$(QQ)RADIOLIB_MODULE=$@ "$(MAKE)" -C $(PKG_SOURCE_DIR)/$(DIR_$@) -f $(CURDIR)/Makefile.radiolib_module
