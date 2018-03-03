OSNAME=`uname -s`
SEDBIN="sed -i"
if [ "${OSNAME}" = "Darwin" ] ; then
    SEDBIN="sed -i ''"
    LANG=C
    LC_CTYPE=C
fi
