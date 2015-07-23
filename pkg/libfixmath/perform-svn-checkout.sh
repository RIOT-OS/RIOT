#!/bin/bash
if [ -z "${PKG_URL}" ]; then
    echo 'export PKG_URL before running this script' >&2
    exit 2
fi
if [ -z "${CHECKOUT_FOLDER}" ]; then
    echo 'export CHECKOUT_FOLDER before running this script' >&2
    exit 2
fi

: ${SVN_INFO_FILE:=${CHECKOUT_FOLDER}/svn_info.xml}

if [ -s "${SVN_INFO_FILE}" ]; then
    # svn checkout was completed by another thread while we waited for the lock
    if [ "${QUIET}" -ne 1 ]; then
        echo "File already exists: ${SVN_INFO_FILE}, skipping svn checkout"
    fi
    exit 0
fi
if [ -n "${PKG_VERSION}" ]; then
    svn checkout -q -r ${PKG_VERSION} ${PKG_URL} ${CHECKOUT_FOLDER}
else
    svn checkout -q ${PKG_URL} ${CHECKOUT_FOLDER}
fi
svn info --xml ${CHECKOUT_FOLDER} > ${SVN_INFO_FILE}
