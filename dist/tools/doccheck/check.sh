#!/bin/sh

TMP1=$(mktemp)
TMP2=$(mktemp)

# If no branch but an option is given, unset BRANCH.
# Otherwise, consume this parameter.
BRANCH="${1}"
if echo "${BRANCH}" | grep -q '^-'; then
    BRANCH=""
else
    if [ -n "${BRANCH}" ]; then
        shift 1
    fi
fi

# If the --diff-filter option is given, consume this parameter.
# Set the default DIFFFILTER option otherwise.
DIFFFILTER="${1}"
if echo "${DIFFFILTER}" | grep -q '^--diff-filter='; then
    shift 1
else
    DIFFFILTER="--diff-filter=ACMR"
fi

# select files to check
if [ -z "${BRANCH}" ]; then
    git ls-tree -r --full-tree --name-only HEAD | grep -E '\.([sSch]|cpp)$' | sort | uniq > ${TMP1}
else
    git diff ${DIFFFILTER} --name-only ${BRANCH} | grep -E '\.([sSchp]|cpp)$' | sort | uniq > ${TMP1}
fi

make doc 2>&1 | grep warning | grep -E '^\/' | sed "s#${PWD}\/\([^:]*\).*#\1#" | sort | uniq > ${TMP2}

WARNINGS=$(comm -1 -2 ${TMP1} ${TMP2})

rm ${TMP_ERR} ${TMP_WARN} ${TMP_DOC}
    
if [ -n "${WARNINGS}" ]
then
echo "WARNING: The following modified files generate doxygen warnings:"
echo "${WARNINGS}"
fi

if [ -n "${WARNINGS}" ]
then
    echo "The following files generate doxygen warnings: ${WARNINGS}"
    exit 2
else
    exit 0
fi
