DIR_WITH_TESTS=$(shell test -f $(abspath $(1)/Makefile) && grep "^test:" $(abspath $(1)/Makefile) 1>&2 > /dev/null && echo "$(abspath $(1))")
TESTS=$(sort $(foreach DIR,$(shell find * -maxdepth 0 -type d),$(call DIR_WITH_TESTS, $(DIR))))

full-test:
	@$(foreach TEST,$(TESTS),$(MAKE) -C $(TEST) all flash test || exit 1;)

list-tests:
	@echo "$(TESTS)"

clean:
	@$(foreach TEST,$(TESTS),$(MAKE) -C $(TEST) clean || exit 1;)
