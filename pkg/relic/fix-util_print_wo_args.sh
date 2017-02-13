#!/bin/sh
. ${RIOTBASE}/pkg/relic/os_util.sh
find ${1} -type f -name "*.[ch]" -exec ${SEDBIN} 's/util_print("\(.*\)")/util_print("\1", NULL)/g' {} +
