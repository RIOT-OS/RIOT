RIOTBASE  ?= $(shell git rev-parse --show-toplevel)
RIOTMAKE  ?= $(RIOTBASE)/makefiles
RIOTTOOLS ?= $(RIOTBASE)/dist/tools

.PHONY: install
install:
	@echo "Installing starlight..."
	@npm install --prefix $(CURDIR) --prefer-offline
	@echo "Starlight installed successfully."

.PHONY: build
build: install
	@npm run build --prefix $(CURDIR)
	@echo "RIOT documentation successfully generated at file://$(RIOTBASE)/doc/starlight/dist/index.html"

.PHONY: dev
dev: install
	@echo "Starting starlight live server..."
	@npm run dev --prefix $(CURDIR)

clean:
	-@rm -rf node_modules .astro dist
