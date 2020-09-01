#!/bin/bash

DATE=`date`
echo "build script called: $DATE" >> "mutate/logs/test.log"

#DIFF=$(git diff --color sys)
#echo "diff:" >> "mutate/logs/test.log"
#echo "$DIFF" >> "mutate/logs/test.log"

BLACKLIST="\
        xtimer_drift \
        xtimer_msg \
        xtimer_longterm \
        xtimer_msg_receive_timeout \
        xtimer_overhead \
        "

EXTRA="bench_xtimer"

XTIMER_TESTS=$(ls tests/ | grep ^xtimer)
XTIMER_TESTS+=" \
            $EXTRA \
            "

#XTIMER_TESTS="xtimer_remove xtimer_reset xtimer_hang"

# remove blacklisted test from test lists
for TEST in $BLACKLIST
do
    XTIMER_TESTS=$(sed "s/$TEST$//g" <<< $XTIMER_TESTS)
done

set -e
for TEST in $XTIMER_TESTS
do
    make TOOLCHAIN=clang -C "tests/$TEST" clean
    make -j4 TOOLCHAIN=clang -C "tests/$TEST" all
done

