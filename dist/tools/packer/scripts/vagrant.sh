date > /etc/vagrant_box_build_time

mkdir /home/${SSH_USERNAME}/.ssh
wget --no-check-certificate \
    'https://raw.githubusercontent.com/hashicorp/vagrant/master/keys/vagrant.pub' \
    -O /home/${SSH_USERNAME}/.ssh/authorized_keys
chown -R user /home/${SSH_USERNAME}/.ssh
chmod -R go-rwsx /home/${SSH_USERNAME}/.ssh
# Create SSH key pair
yes y | ssh-keygen -t rsa -N '' -f ~/.ssh/id_rsa
