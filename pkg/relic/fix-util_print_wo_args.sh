#!/bin/sh
find ${1} -name "*.[ch]" | xargs sed -i 's/util_print("\(.*\)")/util_print("\1", NULL)/g'
