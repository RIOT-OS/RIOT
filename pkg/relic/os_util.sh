OSNAME=`uname -s`
SEDBIN="sed -i"
if [ "${OSNAME}" = "Darwin" ] ; then
    command -v gsed >/dev/null 2>&1 || \
        { echo "\n\033[31mPlease 'brew install gnu-sed'\033[0m\n"; exit 1; }
    SEDBIN="gsed -i"
    LANG=C
fi
