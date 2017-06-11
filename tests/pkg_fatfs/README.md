Using FatFs on native
=======================================
To use this test on native you need a FAT image file. The following commands can be used to create such an image and mount it afterwards so you can add files to your virtual disk that can later be accessed from RIOT.

1. create an enpty file with a size of 128MB
`dd if=/dev/zero of=riot_fatfs_disk.img bs=1M count=128`

2. create a FAT file system within the file
`mkfs.fat riot_fatfs_disk.img`

3. create a mount point which you can use later to add files with your file browser
`sudo mkdir -p /media/riot_fatfs_disk`

4. give all needed rights for that mountpoint to your user
`sudo chown <your_username> /media/riot_fatfs_disk/`

5. mount the image -> the disk should now be accessible from any program
`sudo mount -o loop,umask=000 riot_fatfs_disk.img /media/riot_fatfs_disk`

6. When you are done -> unmount the disk before you use it under RIOT
`sudo umount /media/riot_fatfs_disk`

#####NOTE:
You shouldn't leave the image mounted while you use it in RIOT, the abstraction layer between FatFs and the image file mimics a dumb block device
(i.e. behaves much like the devices that are actually meant to be used with FAT) That implies it doesn't show any modifications in RIOT that you perform on your OS and the other way round. So always remember to mount/unmount correctly or your FS will probably get damaged.

To tell RIOT where your image file is located you can use the image_path entry in the volume_files array in fatfs_diskio_native/diskio.c.