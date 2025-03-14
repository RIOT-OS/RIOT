SCANBUILD_ENV_VARS := \
  APPDIR \
  AR \
  AS \
  ASFLAGS \
  BINDIR \
  BINDIRBASE \
  BOARD \
  BOARDDIR \
  BOARDSDIR \
  BUILD_DIR \
  BUILDRELPATH \
  CC \
  CFLAGS \
  COMMON_BOARD_DIRS \
  CPPMIX \
  CXX \
  CXXEXFLAGS \
  CXXUWFLAGS \
  ELFFILE \
  HEXFILE \
  FLASHFILE \
  HOME \
  LINK \
  LINKFLAGPREFIX \
  LINKFLAGS \
  OBJCOPY \
  OFLAGS \
  PATH \
  PREFIX \
  QUIET \
  WERROR \
  RIOT_VERSION \
  RIOT_CI_BUILD \
  SIZE \
  TOOLCHAIN \
  UNDEF \
  USER \
  #

# Produce errors by default at the end of compilation when warning are found
SCANBUILD_ERROR ?= $(if $(filter 1,$(WERROR)),--status-bugs)

SCANBUILD_ARGS ?= \
  -analyze-headers \
  --use-cc=$(CC) \
  --use-c++=$(CXX) \
  -analyzer-config stable-report-filename=true \
  $(SCANBUILD_ERROR) \
  #

SCANBUILD_OUTPUTDIR = scan-build/$(BOARD)

# Find all variables given on the command line and recreate the command.
CMDVARS := $(strip $(foreach varname, $(SCANBUILD_ENV_VARS), \
  $(if $(filter command, $(origin $(varname))), \
  '$(varname)=$(subst ','\'',$($(varname)))', \
  )))
ENVVARS := $(strip $(foreach varname, $(SCANBUILD_ENV_VARS), \
  $(if $(filter environment, $(origin $(varname))), \
  '$(varname)=$(subst ','\'',$($(varname)))', \
  )))

.PHONY: scan-build scan-build-analyze
.PHONY: ..scan-build-view ..scan-build-analyze

ifeq (1,$(INSIDE_DOCKER))
# In the container just do the analysis, 'view' will be done by the host
scan-build: ..scan-build-analyze
else # INSIDE_DOCKER
scan-build: ..scan-build-view
endif # INSIDE_DOCKER

# Prevent 'analyze' from producing an error when doing 'scan-build' and
# still show the webpage for interractive checking.
scan-build: SCANBUILD_ERROR=

ifeq ($(BUILD_IN_DOCKER),1)
# It will trigger executing 'scan-build' or 'scan-build-analyze' with docker
scan-build-analyze: ..in-docker-container
else # BUILD_IN_DOCKER
scan-build-analyze: ..scan-build-analyze
endif # BUILD_IN_DOCKER


..scan-build-analyze: clean
	@$(COLOR_ECHO) '$(COLOR_GREEN)Performing Clang static code analysis using toolchain "$(TOOLCHAIN)".$(COLOR_RESET)'
# ccc-analyzer needs to be told the proper -target setting for best results,
# otherwise false error reports about unknown register names etc will be produced.
# These kinds of errors can be safely ignored as long as they only come from LLVM
	@if [ "$${TOOLCHAIN}" != "llvm" -a "$${BOARD}" != "native" ]; then \
	  $(COLOR_ECHO) '$(COLOR_YELLOW)Recommend using TOOLCHAIN=llvm for best results.$(COLOR_RESET)'; \
	  $(COLOR_ECHO) '$(COLOR_YELLOW)Ignore any "error: unknown register name '\''rX'\'' in asm" messages.$(COLOR_RESET)'; \
	fi
	$(Q)mkdir -p '$(SCANBUILD_OUTPUTDIR)'
	$(Q)env -i $(ENVVARS) \
	    scan-build -o '$(SCANBUILD_OUTPUTDIR)' $(SCANBUILD_ARGS) \
	      $(MAKE) -C $(CURDIR) all $(strip $(CMDVARS)) FORCE_ASSERTS=1

..scan-build-view: scan-build-analyze
	@echo "Showing most recent report in your web browser..."
	@REPORT_FILE="$$(find '$(SCANBUILD_OUTPUTDIR)' -maxdepth 2 -mindepth 2 \
	            -type f -name 'index.html' 2>/dev/null | sort | tail -n 1)"; \
	  if [ -n "$${REPORT_FILE}" ]; then \
	    echo "$(OPEN) $${REPORT_FILE}"; \
	    $(OPEN) "$${REPORT_FILE}"; \
	  else \
	    echo "No report found"; \
	  fi
