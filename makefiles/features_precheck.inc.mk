# Check if provided features are available

ifneq (,$(filter picolibc,$(FEATURES_PROVIDED)))
  # Test if picolibc.specs is available
  ifneq ($(shell $(LINK) -specs=picolibc.specs -E - 2>/dev/null >/dev/null </dev/null ; echo $$?),0)
    FEATURES_BLACKLIST += picolibc
 endif
endif

ifneq (,$(filter newlib,$(FEATURES_PROVIDED)))
  # Test if nosys.specs is available
  ifneq ($(shell $(LINK) -specs=nosys.specs -E - 2>/dev/null >/dev/null </dev/null ; echo $$?),0)
    FEATURES_BLACKLIST := newlib
  endif
endif
