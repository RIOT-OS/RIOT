#!/usr/bin/env bash -eux

# inspired by https://github.com/boxcutter/ubuntu

echo "==> Disabling the release upgrader"
sed -i.bak 's/^Prompt=.*$/Prompt=never/' /etc/update-manager/release-upgrades

systemctl disable apt-daily.service
systemctl disable apt-daily.timer

echo "==> Updating list of repositories"
apt-get -y update

echo "==> Performing dist-upgrade (all packages and kernel)"
DEBIAN_FRONTEND=noninteractive apt-get -y -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" dist-upgrade

SSH_USER=${SSH_USERNAME:-vagrant}
SSH_PASS=${SSH_PASSWORD:-vagrant}
SSH_USER_HOME=${SSH_USER_HOME:-/home/${SSH_USER}}
VAGRANT_INSECURE_KEY="ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAQEA6NF8iallvQVp22WDkTkyrtvp9eWW6A8YVr+kz4TjGYe7gHzIw+niNltGEFHzD8+v1I2YJ6oXevct1YeS0o9HZyN1Q9qgCgzUFtdOKLv6IedplqoPkcmF0aYet2PkEDo3MlTBckFXPITAMzF8dJSIFo9D8HfdOV0IAdx4O7PtixWKn5y2hMNG0zQPyUecp4pzC6kivAIhyfHilFR61RGL+GPXQ2MWZWFYbAGjyiYJnAmCP3NOTd0jMZEnDkbUvxhMmBYSdETk1rRgm+R4LOzFUGaHqHDLKLX+FIPKcF96hrucXzcWyLbIbEgE98OHlnVYCzRdK8jlqm8tehUc9c9WhQ== vagrant insecure public key"

echo "==> Giving ${SSH_USER} sudo powers"
echo "${SSH_USER}        ALL=(ALL)       NOPASSWD: ALL" >> /etc/sudoers.d/vagrant
chmod 440 /etc/sudoers.d/vagrant

if grep -q -E "^mesg n$" /root/.profile && sed -i "s/^mesg n$/tty -s \\&\\& mesg n/g" /root/.profile; then
  echo "==> Fixed stdin not being a tty."
fi

echo "==> Installing vagrant key"
mkdir $SSH_USER_HOME/.ssh
chmod 700 $SSH_USER_HOME/.ssh

# https://raw.githubusercontent.com/mitchellh/vagrant/master/keys/vagrant.pub
echo "${VAGRANT_INSECURE_KEY}" > $SSH_USER_HOME/.ssh/authorized_keys
chmod 600 $SSH_USER_HOME/.ssh/authorized_keys
chown -R $SSH_USER:$SSH_USER $SSH_USER_HOME/.ssh

echo "UseDNS no" >> /etc/ssh/sshd_config

if [[ $PACKER_BUILDER_TYPE =~ virtualbox ]]; then
    echo "==> Installing VirtualBox guest additions"
    VBOX_VERSION=$(cat ${SSH_USER_HOME}/.vbox_version)
    mount -o loop ${SSH_USER_HOME}/VBoxGuestAdditions_$VBOX_VERSION.iso /mnt
    sh /mnt/VBoxLinuxAdditions.run
    umount /mnt
    rm ${SSH_USER_HOME}/VBoxGuestAdditions_$VBOX_VERSION.iso
    rm ${SSH_USER_HOME}/.vbox_version
fi

echo "==> Install stlink"
wget -qO - https://github.com/texane/stlink/archive/master.tar.gz | tar xz
mkdir stlink-master/build && cd stlink-master/build && cmake .. && make && make install
cd ../.. && rm -rf stlink.git

echo "==> Install cli-tools"
wget -qO - https://github.com/iot-lab/cli-tools/archive/2.1.0.tar.gz | tar xz
cd cli-tools-* && python setup.py install && cd .. && rm -rf cli-tools-*

# Remove some packages to get a minimal install
echo "==> Removing all linux kernels except the current one"
dpkg --list | awk '{ print $2 }' | grep 'linux-image-*-generic' | grep -v $(uname -r) | xargs apt-get -y purge
echo "==> Removing linux source"
dpkg --list | awk '{ print $2 }' | grep linux-source | xargs apt-get -y purge
echo "==> Removing documentation"
dpkg --list | awk '{ print $2 }' | grep -- '-doc$' | xargs apt-get -y purge
echo "==> Removing default system Ruby"
apt-get -y purge ruby ri doc
echo "==> Removing X11 libraries"
apt-get -y purge libx11-data xauth libxmuu1 libxcb1 libx11-6 libxext6
echo "==> Removing obsolete networking components"
apt-get -y purge ppp pppconfig pppoeconf
echo "==> Removing other oddities"
apt-get -y purge popularity-contest installation-report landscape-common wireless-tools wpasupplicant

# Clean up the apt cache
apt-get -y autoremove --purge
apt-get -y autoclean
apt-get -y clean

# Clean up orphaned packages with deborphan
apt-get -y install deborphan
while [ -n "$(deborphan --guess-all --no-guesspython --libdevel)" ]; do
    deborphan --guess-all --no-guess-python --libdevel | xargs apt-get -y purge
done
apt-get -y purge deborphan dialog

echo "==> Removing man pages"
rm -rf /usr/share/man/*
echo "==> Removing APT files"
find /var/lib/apt -type f | xargs rm -f
echo "==> Removing any docs"
rm -rf /usr/share/doc/*
echo "==> Removing caches"
find /var/cache -type f -exec rm -rf {} \;

echo "==> Cleaning up leftover dhcp leases"
if [ -d "/var/lib/dhcp" ]; then
    rm /var/lib/dhcp/*
fi

# Add delay to prevent "vagrant reload" from failing
echo "pre-up sleep 2" >> /etc/network/interfaces

mv /tmp/udev_rules/* /etc/udev/rules.d/

echo "==> Cleaning up tmp"
rm -rf /tmp/*

# Remove Bash history
unset HISTFILE
rm -f /root/.bash_history
rm -f ${SSH_USER_HOME}/.bash_history

# Clean up log files
find /var/log -type f | while read f; do echo -ne '' > "${f}"; done;

echo "==> Clearing last login information"
>/var/log/lastlog
>/var/log/wtmp
>/var/log/btmp

echo "==> whiteout /"
count=$(df --sync -kP / | tail -n1  | awk -F ' ' '{print $4}')
let count--
dd if=/dev/zero of=/tmp/whitespace bs=1024 count=$count
rm /tmp/whitespace

echo "==> whiteout /boot"
count=$(df --sync -kP /boot | tail -n1 | awk -F ' ' '{print $4}')
let count--
dd if=/dev/zero of=/boot/whitespace bs=1024 count=$count
rm /boot/whitespace

echo '==> Clear out swap and disable until reboot'
set +e
swapuuid=$(/sbin/blkid -o value -l -s UUID -t TYPE=swap)
case "$?" in
    2|0) ;;
    *) exit 1 ;;
esac
set -e
if [ "x${swapuuid}" != "x" ]; then
    # Whiteout the swap partition to reduce box size
    # Swap is disabled till reboot
    swappart=$(readlink -f /dev/disk/by-uuid/$swapuuid)
    /sbin/swapoff "${swappart}"
    dd if=/dev/zero of="${swappart}" bs=1M || echo "dd exit code $? is suppressed"
    /sbin/mkswap -U "${swapuuid}" "${swappart}"
fi

echo "==> whiteout free space"
dd if=/dev/zero of=/EMPTY bs=1M  || echo "dd exit code $? is suppressed"
rm -f /EMPTY

sync

echo "==> Disk usage"
df -h
