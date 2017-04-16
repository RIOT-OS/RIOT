all:
	pwd=$$(pwd); \
	for testdir in test_*; do \
		cd $${testdir}; \
		scriptdir=tests; \
		if [ -d $${scriptdir} ]; then \
			$(MAKE) clean all || exit 1; \
			for script in $${scriptdir}/*; do \
				$${script} || exit 1; \
			done; \
		fi; \
		cd $${pwd}; \
	done;
	$(MAKE) -C unittests clean all term
