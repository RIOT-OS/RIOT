#!/bin/sh
# This sed script converts doxygen error messages to line number agnostic regex
# patterns as used by doccheck/check.sh
#
# Example:
#
#     dist/tools/doccheck/check.sh \
#         | grep boards/nucleo-g071rb/include/periph_conf.h \
#         | dist/tools/doccheck/create_pattern.sh >> dist/tools/doccheck/exclude_patterns
#
# This would add all remaining warnings of nucleo-g071rb's periph_conf.h to the
# exclude_patterns.
#

sed -E -e "s#^(.*:)([0-9]+): #\1DIGITS_MAGIC: #" \
        -e 's/[]\\.$*{}|+?()[^-]/\\&/g' \
        -e 's/:DIGITS_MAGIC:/:[0-9]+:/'
