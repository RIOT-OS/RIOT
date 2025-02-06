.all:

.PHONY: all doc doc-man doc-latex docclean print-versions welcome

all: welcome
	@echo ""
	@exit 1

doc:
	@./dist/tools/python_with_requirements/python_with_requirements \
	./dist/tools/features_yaml2mx/features_yaml2mx.py \
		features.yaml \
		--output-md doc/doxygen/src/feature_list.md \
		--output-ttl doc/rdf/features.ttl
	@# The BUILD_IN_DOCKER is a workaround for the RISC-V
	@# architecture tests otherwise triggering when building eg. in murdock
	@# ("No RISC-V toolchain detected. Make sure a RISC-V toolchain is
	@# installed."). If I had a penny for every time I refused to let
	@# BUILD_IN_DOCKER be the simple workaround, I wouldn't get a penny
	@# today (but I'd still be proud of my small collection so far).
	unset BOARDS; \
	BUILD_IN_DOCKER=1 \
	./dist/tools/python_with_requirements/python_with_requirements \
	./dist/tools/rdf/info_to_rdf.py \
		doc/rdf/info.ttl
	@./dist/tools/python_with_requirements/python_with_requirements \
	./dist/tools/rdf/doxygen_to_rdf.py \
		doc/rdf/doxygen.ttl
	@./dist/tools/python_with_requirements/python_with_requirements \
	./dist/tools/rdf/build_board_feature_table.py > doc/doxygen/src/feature_table.html
	"$(MAKE)" -BC doc/doxygen

doc-man:
	"$(MAKE)" -BC doc/doxygen man

doc-latex:
	"$(MAKE)" -BC doc/doxygen latex

docclean:
	"$(MAKE)" -BC doc/doxygen clean

clean:
	@echo "Cleaning all build products for the current board"
	@for dir in $(APPLICATION_DIRS); do "$(MAKE)" -C$$dir clean; done

pkg-clean:
	@echo "Cleaning all package sources"
	rm -rf build/pkg

distclean: docclean pkg-clean
	@echo "Cleaning all build products"
	@for dir in $(APPLICATION_DIRS); do "$(MAKE)" -C$$dir distclean; done

print-versions:
	@./dist/tools/ci/print_toolchain_versions.sh

generate-features:
	@./dist/tools/python_with_requirements/python_with_requirements \
	./dist/tools/features_yaml2mx/features_yaml2mx.py \
		features.yaml \
		--output-makefile makefiles/features_existing.inc.mk

include makefiles/boards.inc.mk
include makefiles/app_dirs.inc.mk

include makefiles/tools/riotgen.inc.mk
-include makefiles/tests.inc.mk

include makefiles/color.inc.mk

# Prints a welcome message
define welcome_message
	@echo "Welcome to RIOT - The friendly OS for IoT!"
	@echo ""
	@echo "You executed 'make' from the base directory."
	@echo "Usually, you should run 'make' in your application's directory instead."
	@echo ""
	@echo "Please see our Quick Start Guide at:"
	@echo "    https://doc.riot-os.org/getting-started.html"
	@echo "You can ask questions or discuss with other users on our forum:"
	@echo "    https://forum.riot-os.org"
	@echo ""
	@echo "Available targets for the RIOT base directory include:"
	@echo " generate-{board,driver,example,module,pkg,test,features}"
	@echo " info-{applications,boards,emulated-boards} info-applications-supported-boards"
	@echo " print-versions"
	@echo " clean distclean pkg-clean"
	@echo " doc doc-{man,latex}"
	@echo ""
	@echo "==> tl;dr Try running:"
	@echo "    cd examples/default"
	@echo "    make BOARD=<INSERT_BOARD_NAME>"
endef

welcome:
	$(call welcome_message)

.DEFAULT:
	@echo '*** ERROR: unrecognized target "$@"'
	@echo ""
	$(call welcome_message)
	@exit 1
