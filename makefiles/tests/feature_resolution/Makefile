.PHONY: all info-boards-supported

TESTS := $(patsubst test-%.mk,run-%,$(wildcard test-*.mk))

all: $(TESTS)

info-boards-supported:
	@echo none

run-%: test-%.mk
	@echo "Running test $(patsubst run-%,%,$@)"
	@$(MAKE) --silent -f $<
