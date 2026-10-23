#!/bin/sh

# SPDX-FileCopyrightText: 2014 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

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

BINFILE=$1

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
cp ${BINFILE} ${MOUNT}
# make sure binary was written
sync

# unmount the device if we have manually mounted it before
if [ ${DID_MOUNT} = true ]
then
    umount ${MOUNT}
fi

echo ""
echo "UPLOAD SUCCESSFUL"
echo ""
