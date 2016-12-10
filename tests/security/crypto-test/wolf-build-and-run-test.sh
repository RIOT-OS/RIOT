#!/bin/bash

#---------------------------------------#
# function to confirm file system setup #
#---------------------------------------#
check_for_wolfssl() {
    if [ -d "../wolfssl" ]; then
        echo "wolfssl found, proceed..."
    else
        echo "failed to find wolfssl"
        echo "Please clone wolfssl in the RIOT/tests/security directory using the"
        echo "following commands:"
        echo "------------------------------------------------"
        echo "cd ../"
        echo "git clone https://github.com/wolfssl/wolfssl.git"
        echo "cd crypto-test"
        echo "------------------------------------------------"
        echo "then try running this script again, thanks!"
        exit 5
    fi
}

#---------------------------------------#
# function for setting up the example   #
#---------------------------------------#
wolf_riot_setup () {
    #copy the test sources here
    cp ../wolfssl/wolfcrypt/test/*.c ./

    cp ../wolfssl/wolfcrypt/src/*.c ./

    cp ../wolfssl/src/*.c ./

    #this file is included unles defining NO_INLINE
    rm misc.c
}

#---------------------------------------#
# function to cleanup                   #
#---------------------------------------#
wolf_riot_cleanup () {
    if [ $1 -eq 1 ]; then
        echo "cleaning up"
        rm ./*.c
        #leave this line in for testing. Comment it out when you want to build
        # a .elf for flashing to a device
        make clean &> /dev/null
        echo "done cleaning up"
    else
        echo "no cleanup specified, leaving files"
    fi
}
trap wolf_riot_cleanup INT TERM


#---------------------------------------#
# Script BEGIN:                         #
#---------------------------------------#
echo "Usage:"
echo "run with no arguments to build and run the test"
echo "file system will be cleaned up on completion of test"
echo "./wolf-build-and-run-test.sh"
echo ""
echo "run with --no-cleanup to prevent cleanup of the file system"
echo "./wolf-build-and-run-test.sh --no-cleanup"
echo ""

if [ "$1" == "--no-cleanup" ]; then
CLEANUP=0
else
CLEANUP=1
fi

# make sure the file system is properly setup
check_for_wolfssl

# copy the necessary files to this directory
wolf_riot_setup

# build the test
# change next line to just "make" to see verbose output
# NOTE: will throw a warning on every file that is empty if that feature
#       is not enabled in wolfssl.
make &> /dev/null
RESULT=$?
 [ $RESULT != 0 ] && echo "Make FAILED: running verbose make" &&
                     make

if [ $RESULT != 0 ];
then
    wolf_riot_cleanup $CLEANUP && exit 2
fi

# run the test
RESULT=`./bin/native/wolfcrypttest.elf`

# confirm success or failure
errstring="error"
if test "${RESULT#*$errstring}" != "$RESULT"
    then
        echo "$RESULT"
        echo "TEST FAILED" && wolf_riot_cleanup $CLEANUP && exit 2
    else
        echo "$RESULT"
    fi

echo "ALL TEST PASSED!"

# cleanup. All changes made should be to the files in:
# <wolfssl-root>/src
# <wolfssl-root>/wolfcrypt/src
# or other. Never make changes to the files copied here as they are only
# temporary. Once changes are made, to test them just run this script again.
wolf_riot_cleanup $CLEANUP

exit 0


