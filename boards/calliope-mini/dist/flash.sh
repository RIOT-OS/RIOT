#!/bin/sh

# Copyright (C) 2014 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# The Calliope mini can be flashed through accessing it as a mass storage
# device. To upload a new firmware, simply copy your binary onto this device.
#
# Under Ubuntu/Mint the default mount-point is /media/$(USER)/MINI
#
# @author Hauke Petersen <hauke.petersen@fu-berlin.de>
# @author Oliver Hahm <oliver.hahm@inria.fr>

OS=`uname`
DID_MOUNT=false
NAME="MINI"

HEXFILE=$1

# set the mount path depending on the OS
if [ ${OS} = "Linux" ]
then
    MOUNT=/media/${USER}/${NAME}
elif [ ${OS} = "Darwin" ]
then
    MOUNT=/Volumes/${NAME}
else
    echo ""
    echo "ERROR: No mount point defined for your OS"
    echo "Please copy the binary manually to your Calliope mini"
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
        echo "ERROR: could not mount your Calliope mini"
        echo ""
        exit
    else
        DID_MOUNT=true
    fi
fi

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
