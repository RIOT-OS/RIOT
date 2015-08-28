PKG_NAME=leshan
PKG_URL=https://github.com/eclipse/leshan.git
PKG_VERSION=3abb9c99b9a7da68a0e7c07c75ccdcca7a375232
PKG_DIR=$(CURDIR)/$(PKG_NAME)

.PHONY: all build clean run

all: run

run: build
	cd "$(PKG_DIR)/leshan-standalone"; \
	export COAPIFACE=2001:db8::1:4242 && java -jar target/leshan-standalone-*-SNAPSHOT-jar-with-dependencies.jar

build:
	test -d "$(PKG_DIR)" || git clone "$(PKG_URL)" "$(PKG_DIR)"; \
	cd "$(PKG_DIR)" && git checkout -f "$(PKG_VERSION)"; \
	mvn install; \
	cd leshan-standalone; \
	mvn assembly:assembly -DdescriptorId=jar-with-dependencies

clean::
	@echo "Cleaning up $(PKG_NAME) package..."
	@cd "$(PKG_DIR)" 2> /dev/null > /dev/null && \
		git clean -x -f && \
		git am --abort && \
		git reset --hard "$(PKG_VERSION)" && \
		$(MAKE) patch || true


distclean::
	rm -rf "$(PKG_DIR)"

Makefile.include:
	@true
