#!/bin/bash

if [ -z "${CHECKOUT_FOLDER}" ]; then
    echo 'export CHECKOUT_FOLDER before running this script' >&2
    exit 2
fi

: ${CHECKOUT_LOCKFILE:=${CHECKOUT_FOLDER}/.riot-svn-checkout-lockfile}
# Default timeout, in seconds
: ${LOCK_TIMEOUT:=120}

# We need to protect SVN checkout with a lock to avoid parallel make runs from
# disrupting each other
if type flock >/dev/null 2>&1; then
    # Try flock(1) command first, usually found on Linux
    (
        flock -w "${LOCK_TIMEOUT}" 200 || (
            echo "Timed out waiting to acquire lock ${CHECKOUT_LOCKFILE}" >&2
            exit 1
        )
        $(dirname "$0")/perform-svn-checkout.sh
        exit $?
    ) 200>"${CHECKOUT_LOCKFILE}"
elif type lockf >/dev/null 2>&1; then
    # lockf is usually installed on FreeBSD and OSX
    lockf -t ${LOCK_TIMEOUT} ${CHECKOUT_LOCKFILE} $(dirname "$0")/perform-svn-checkout.sh
    exit $?
else
    # flock(1) and lockf(1) shell commands are missing from the system
    echo 'Missing both flock(1) and lockf(1),'
    echo 'flock is part of util-linux on most Linux systems, lockf is usually installed on FreeBSD.'
    echo 'see https://github.com/discoteq/flock for a portable alternative.'
    echo 'Proceeding with svn checkout without locking...'
    $(dirname "$0")/perform-svn-checkout.sh
    exit $?
fi
