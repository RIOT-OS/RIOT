#!/bin/bash

DATE=`date`
echo "test script called: $DATE" >> "mutate/logs/test.log"

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

FAILED=0
for TEST in $XTIMER_TESTS
do
    make -C "tests/$TEST" test
    status=$?
    echo "$TEST: $status" >> "mutate/logs/test.log"
    if [ $status != 0 ]; then
	FAILED=1
    fi
done

if [ $FAILED != 0 ]; then
    exit 1
fi
