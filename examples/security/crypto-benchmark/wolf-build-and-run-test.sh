#!/bin/bash

#---------------------------------------#
# function to confirm file system setup #
#---------------------------------------#
check_for_wolfssl() {
    if [ -d "../wolfssl" ]; then
        echo "wolfssl found, proceed..."
    else
        echo "failed to find wolfssl"
        echo "Please clone wolfssl in the RIOT/examples/security directory using the"
        echo "following commands:"
        echo "------------------------------------------------"
        echo "cd ../"
        echo "git clone https://github.com/wolfssl/wolfssl.git"
        echo "cd crypto-benchmark"
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
    cp Makefile.benchmark Makefile
    cp ../wolfssl/wolfcrypt/benchmark/benchmark.c ./

    cp ../wolfssl/wolfcrypt/src/*.c ./

    cp ../wolfssl/src/*.c ./
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
        rm Makefile
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
make &> /dev/null
RESULT=$?
 [ $RESULT != 0 ] && echo "Make FAILED: running verbose make" &&
                     make
if [ $RESULT != 0 ];
then
    wolf_riot_cleanup $CLEANUP && exit 2
fi

# run the test
./bin/native/wolfbenchmark.elf

# confirm success or failure
RESULT=$?
 [ $RESULT != 0 ] && echo "TEST FAILED"  && wolf_riot_cleanup $CLEANUP && exit 2

echo "TEST PASSED!"

# cleanup. All changes made should be to the files in:
# <wolfssl-root>/src
# <wolfssl-root>/wolfcrypt/src
# or other. Never make changes to the files copied here as they are only
# temporary. Once changes are made, to test them just run this script again.
wolf_riot_cleanup $CLEANUP


