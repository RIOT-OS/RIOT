# fallback so empty RIOTBASE won't lead to "/examples/"
RIOTBASE ?= .

# 1. use wildcard to find Makefiles
# 2. use patsubst to drop trailing "/"
# 3. use patsubst to drop possible leading "./"
# 4. sort
APPLICATION_DIRS := $(sort $(patsubst ./%,%,$(patsubst %/,%,$(dir $(wildcard \
	$(RIOTBASE)/examples/*/Makefile \
	$(RIOTBASE)/tests/*/Makefile    \
	)))))

info-applications:
	@for dir in $(APPLICATION_DIRS); do echo $$dir; done
