RIOTBASE  ?= $(shell git rev-parse --show-toplevel)
RIOTMAKE  ?= $(RIOTBASE)/makefiles
RIOTTOOLS ?= $(RIOTBASE)/dist/tools

.PHONY: install
install: integrate_outside_files
	@echo "Installing starlight..."
	@npm install --prefix $(CURDIR) --prefer-offline
	@echo "Starlight installed successfully."

.PHONY: build
build: install
	@echo "Copying RIOT documentation source files..."
	@echo "See: https://github.com/withastro/starlight/issues/3470"
	@rm -rf $(CURDIR)/src/content/docs
	@cp -r $(RIOTBASE)/doc/guides $(CURDIR)/src/content/docs
	@npm run build --prefix $(CURDIR)
	@echo "RIOT documentation successfully generated at file://$(RIOTBASE)/doc/starlight/dist/index.html"

.PHONY: preview
preview: build
	@npm run preview --prefix $(CURDIR)

.PHONY: dev
dev: install
	@echo "Linking RIOT documentation source files..."
	@rm -rf $(CURDIR)/src/content/docs
	@ln -s ../../../guides $(CURDIR)/src/content/docs
	@echo "Starting starlight live server..."
	@npm run dev --prefix $(CURDIR)

.PHONY: clean
clean:
	-@rm -rf node_modules .astro dist

.PHONY: integrate_outside_files
integrate_outside_files: code_of_conduct governance lostandfound coding_conventions coding_conventions_cpp

.PHONY: code_of_conduct
code_of_conduct: ../../CODE_OF_CONDUCT.md
	-@mkdir -p ../guides/generated
	-@echo "Generating Code of Conduct documentation..."
	-@sed '1,2d' $< | cat ./templates/CODE_OF_CONDUCT.template.md - > ../guides/generated/CODE_OF_CONDUCT.md

.PHONY: governance
governance: ../../GOVERNANCE.md
	-@echo "Generating Governance documentation..."
	-@sed '1,2d' $< | sed '/<!-- TOC start -->/,/<!-- TOC end -->/d' | cat ./templates/GOVERNANCE.template.md - > ../guides/generated/GOVERNANCE.md

.PHONY: lostandfound
lostandfound: ../../LOSTANDFOUND.md
	-@echo "Generating Lost and Found documentation..."
	-@sed '1,2d' $< | cat ./templates/LOSTANDFOUND.template.md - > ../guides/generated/LOSTANDFOUND.md

.PHONY: coding_conventions
coding_conventions: ../../CODING_CONVENTIONS.md
	-@echo "Generating Coding Conventions documentation..."
	-@sed '1,2d' $< | cat ./templates/CODING_CONVENTIONS.template.md - > ../guides/generated/CODING_CONVENTIONS.md

.PHONY: coding_conventions_cpp
coding_conventions_cpp: ../../CODING_CONVENTIONS_C++.md
	-@echo "Generating C++ Coding Conventions documentation..."
	-@sed '1,2d' $< | cat ./templates/CODING_CONVENTIONS_CPP.template.md - > ../guides/generated/CODING_CONVENTIONS_CPP.md
