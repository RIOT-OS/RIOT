#!/bin/sh -u

if [ -e ${1} ]; then
    echo "Directory \"${1}\" exists. Will not overwrite."
    exit 1
fi

echo "Creating project ${1}..."

cp -r skel ${1}
sed -i -e "s/skel/${1}/g" ${1}/main.c
sed -i -e "s/skel/${1}/g" ${1}/Jamfile

echo "Done."

