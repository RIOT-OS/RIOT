# Make does not recognize escaped characters (e.g. \t, \033, etc).
# The following variables contain the characters themselves.
# This file is inherently fragile (i.e. your editor could destroy the tab
# without you realizing, to be careful when editing this.

# To generate this: $ printf "ANSI_ESC := \033# comment"
ANSI_ESC := # you may or may not be able to see that character in your editor

# The third parameter to the subst is a tab - be careful not to replace it with
# spaces!
TAB := $(subst ,,	)

define NEWLINE


endef
