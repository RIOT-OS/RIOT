PKG_NAME=openthread
PKG_URL=git://github.com/openthread/openthread.git
PKG_VERSION=02712ffe109aaeb7b6323be16fbafa81c49eb08c
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: all

$(info $(CPP))
$(info $(CC))
$(info $(CXX))
$(info $(OBJC))
$(info $(OBJCXX))
$(info $(AR))
$(info $(RANLIB))
$(info $(NM))
$(info $(STRIP))
$(info $(CPPFLAGS))
$(info $(CFLAGS))
$(info $(CXXFLAGS))
$(info $(LDFLAGS))

all: git-download
	    cd $(PKG_BUILDDIR) && PREFIX="/" ./bootstrap && ./configure CPP="$(CPP)" CC="$(CC)" CXX="$(CXX)" OBJC="$(OBJC)" OBJCXX="$(OBJCXX)" AR="$(AR)" RANLIB="$(RANLIB)" NM="$(NM)" STRIP="$(STRIP)" CPPFLAGS="$(CPPFLAGS)" CFLAGS="$(CFLAGS)" CXXFLAGS="$(CXXFLAGS)" LDFLAGS="$(LDFLAGS)"
		$(make)

include $(RIOTBASE)/pkg/pkg.mk
