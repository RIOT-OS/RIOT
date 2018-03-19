test-murdock: link
	cd $(RIOTBASE) && \
		./.murdock make_test_job \
		$$(realpath --relative-to $(RIOTBASE) $(APPDIR)) \
		$(BOARD) \
		$(HEXFILE)
