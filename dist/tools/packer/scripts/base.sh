apt-get update
apt-get -y upgrade
apt-get -y install linux-headers-$(uname -r)

# Fix locale setup
locale-gen
localectl set-locale LANG="en_US.UTF-8"

# Allow usage of serial port
adduser ${SSH_USERNAME} dialout

echo '%sudo   ALL=(ALL:ALL) NOPASSWD:ALL' >> /etc/sudoers

echo "UseDNS no" >> /etc/ssh/sshd_config
