mkdir /tmp/virtualbox
VERSION=$(cat /home/${SSH_USERNAME}/.vbox_version)
mount -o loop /home/${SSH_USERNAME}/VBoxGuestAdditions_$VERSION.iso /tmp/virtualbox
sh /tmp/virtualbox/VBoxLinuxAdditions.run
umount /tmp/virtualbox
rmdir /tmp/virtualbox
rm /home/${SSH_USERNAME}/*.iso
