#!/bin/sh
find ${1} -name "*.[ch]" | xargs sed -i 's/() {/(void) {/'
