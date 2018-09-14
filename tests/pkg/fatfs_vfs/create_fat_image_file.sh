#!/usr/bin/env bash
#
# Copyright (C) 2017 HAW-Hamburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#
dd if=/dev/zero of=riot_fatfs_disk.img bs=1M count=$1
mkfs.fat riot_fatfs_disk.img
sudo mkdir -p /media/riot_fatfs_disk
sudo mount -o loop,umask=000 riot_fatfs_disk.img /media/riot_fatfs_disk
touch /media/riot_fatfs_disk/test.txt
echo "the test file content 123 abc" | tr '\n' '\0' >> /media/riot_fatfs_disk/test.txt
sudo umount /media/riot_fatfs_disk
tar -cjf riot_fatfs_disk.tar.gz riot_fatfs_disk.img
rm riot_fatfs_disk.img
