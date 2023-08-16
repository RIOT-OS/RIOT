#!/bin/bash
#
# Generate Kconfig file for all parts of a sam0 family
# based on the available vendor files.

if [ -z "$1" ]
then
    echo "usage: $0 <family>"
    exit 1
fi

FAM=$1
FAM_UC=$(echo $FAM | tr a-z A-Z)

VENDOR_FILES=$(dirname $0)/../include/vendor/$FAM/*/$FAM?*.h

{
echo '## CPU Models'
for i in $VENDOR_FILES; do
    MODEL_LC=$(basename $i .h)
    MODEL_UC=$(echo $MODEL_LC | tr a-z A-Z)

    echo "config CPU_MODEL_$MODEL_UC"
    echo "    bool"
    echo "    select CPU_FAM_$FAM_UC"
    echo ""
done

echo 'config CPU_MODEL'
for i in $VENDOR_FILES; do
    MODEL_LC=$(basename $i .h)
    MODEL_UC=$(echo $MODEL_LC | tr a-z A-Z)
    printf '    default "%s" if CPU_MODEL_%s\n' $MODEL_LC $MODEL_UC
done
} > Kconfig.$FAM
