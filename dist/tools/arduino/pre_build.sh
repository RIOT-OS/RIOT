#!/bin/sh

# check if at least the application dir and one sketch is given
if [ $# -lt 2 ]
then
    echo "[Arduino pre-build] Error: not enough arguments given"
    exit 1
fi

# 'consume' the application and arduino directories (first argument)
SRCDIR=$1
shift
APPDIR=$1
shift

# create temporary file and put in the file header
cat ${SRCDIR}/pre.snip > ${APPDIR}/_sketches.cpp
# loop through the given sketches and include them into the temp file
for sketch in $@
do
    cat ${sketch} >> ${APPDIR}/_sketches.cpp
done
# and prepend the file with the arduino bootstrapping code
cat ${SRCDIR}/post.snip >> ${APPDIR}/_sketches.cpp
