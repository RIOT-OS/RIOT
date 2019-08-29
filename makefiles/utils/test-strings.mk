# Test the string handling functions

include strings.mk
include checks.mk

# String equality. This has to be tested BEFORE ensure_equal
# (ensure_equal is defined in term of this).
# The order is: test assert, assert_not, streq (this), assert_eq,
# and then the rest of the tests in this file

test-streq:
	$(call assert,$(call streq,hello,hello),"Strings are equal, you should not be seeing this")
	$(call assert_not,$(call streq,hello,goodbye),"Strings are different (OK)")
	$(call assert_not,$(call streq,,goodbye),"First string empty (OK)")
	$(call assert_not,$(call streq,hello,),"Second string empty (OK)")
	$(call assert_not,$(call streq,,),"Both strings empty (OK)")
	@true

test-rest:
	$(call assert_eq,$(call rest,ab xy),xy)
	$(call assert_eq,$(call rest,$(call rest,one two three)),three)
	$(call assert_not,$(call rest,first))
	@true

test-strnot:
	$(call assert,$(call strnot,),"strnot(empty) should be true")
	$(call assert_not,$(call strnot,something),"strnot(not empty) should be false")
	@true

test-concat:
	$(call assert_eq,$(call concat,ab xy z),abxyz)
	@true

test-intercal:
	@# The multiple spaces between xy and z are on purpose
	$(call assert_eq,$(call intercal,-,ab xy   z),ab-xy-z)
	$(call assert_eq,$(call intercal,-,ab),ab)
	@true

STRING_LOWER = abcdefghijklmnopqrstuvwxyz-123456789
STRING_UPPER = ABCDEFGHIJKLMNOPQRSTUVWXYZ-123456789
STRING_MACRO = ABCDEFGHIJKLMNOPQRSTUVWXYZ_123456789

test-lowercase:
	$(call assert_eq,$(call lowercase,$(STRING_UPPER)),$(STRING_LOWER))
	@true

test-uppercase:
	$(call assert_eq,$(call uppercase,$(STRING_LOWER)),$(STRING_UPPER))
	@true

test-uppercase_and_underscore:
	$(call assert_eq,$(call uppercase_and_underscore,$(STRING_LOWER)),$(STRING_MACRO))
	@true
