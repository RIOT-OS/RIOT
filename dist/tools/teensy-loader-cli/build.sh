#!/usr/bin/env sh

# get sources from repository
git clone https://github.com/PaulStoffregen/teensy_loader_cli.git
cd teensy_loader_cli

# resolv build host in a hacky way
UNAME=`uname`
TARGET="WINDOWS";
if [ "x$UNAME" = "xLinux" ]; then
    TARGET="LINUX";
elif [ "x$UNAME" = "xDarwin" ]; then
    TARGET="MACOSX";
fi;

# check if the library exists
if [ "x$TARGET" != "xWINDOWS" ]; then
    if ! ldconfig -p | grep 'libusb' > /dev/null; then
        echo "please install libusb-dev";
        exit 1;
    fi;
fi;

# build the application
OS=$TARGET make

# copy the tool to the base directory
mv teensy_loader_cli ../teensy_loader
cd ..
rm -rf teensy_loader_cli
