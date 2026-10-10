#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2017 HAW-Hamburg
# SPDX-License-Identifier: LGPL-2.1-only

dd if=/dev/zero of=riot_fatfs_disk.img bs=1M count="$1"
mkfs.fat riot_fatfs_disk.img
sudo mkdir -p /media/riot_fatfs_disk
sudo mount -o loop,umask=000 riot_fatfs_disk.img /media/riot_fatfs_disk
touch /media/riot_fatfs_disk/test.txt
echo "the test file content 123 abc" | tr '\n' '\0' >> /media/riot_fatfs_disk/test.txt
sudo umount /media/riot_fatfs_disk
tar -cjf riot_fatfs_disk.tar.bz2 riot_fatfs_disk.img
rm riot_fatfs_disk.img
