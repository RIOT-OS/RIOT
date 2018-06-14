include ../Makefile.tests_common

# othread_t modifying modules:
#
# disabled by default:
# USEMODULE += core_thread_flags
#
# enabled by default:
# DISABLE_MODULE += core_msg

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
