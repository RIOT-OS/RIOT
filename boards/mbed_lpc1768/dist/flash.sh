#!/bin/sh

# Copyright (C) 2014 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# mbed devices are detected ass mass storage devices. To upload
# a new firmware, simply copy your binary onto this device and
# press the reset button.
#
# Under Ubuntu/Mint the default mount-point is /media/$(USER)/MBED
#
# NOTE: You have to manually press the boards RESET button after
#       flashing
#
# @author Hauke Petersen <hauke.petersen@fu-berlin.de>
# @author Oliver Hahm <oliver.hahm@inria.fr>

OS=`uname`
DID_MOUNT=false

# set the mount path depending on the OS
if [ ${OS} = "Linux" ]
then
    MOUNT=/media/$USER/MBED
elif [ ${OS} = "Darwin" ]
then
    MOUNT=/Volumes/MBED
else
    echo ""
    echo "ERROR: No mount point defined for your OS"
    echo "Please copy the binary manually to your MBED device"
    echo ""
    exit
fi

# check if device was mounted
mount | grep ${MOUNT} > /dev/null
if [ $? -eq 1 ]
then
    mount ${MOUNT}
    if [ $? -eq 1 ]
    then
        echo ""
        echo "ERROR: could not mount your MBED device"
        echo ""
        exit
    else
        DID_MOUNT=true
    fi
fi

# remove old binary
rm -f ${MOUNT}/*.bin
# copy new binary to device
cp ${HEXFILE} ${MOUNT}
# make sure hexfile was written
sync

# unmount the device if we have manually mounted it before
if [ ${DID_MOUNT} = true ]
then
    umount ${MOUNT}
fi

echo ""
echo "UPLOAD SUCCESFUL"
echo ""
