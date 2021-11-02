apt-get update
apt-get install -y ccache clang cmake curl git build-essential vim python-setuptools \
    python-argparse python3-pip python-pip mosquitto-clients socat g++-multilib \
    net-tools pcregrep libpcre3 gcc-avr binutils-avr avr-libc \
    avrdude doxygen cppcheck valgrind coccinelle \
    gcc-msp430 mspdebug unzip pkg-config jimsh libtool \
    #

DEBIAN_FRONTEND=noninteractive DEBCONF_NONINTERACTIVE_SEEN=true apt-get install -y tshark

# Add .local/bin to PATH
# Required for Python packages installed in user home directory
echo "export PATH=\$PATH:/home/${SSH_USERNAME}/.local/bin" >> /home/${SSH_USERNAME}/.bashrc

# IoT-LAB CLI tools
sudo -u /${SSH_USERNAME} pip3 install --user iotlabwscli iotlabsshcli iotlabcli

# Python tools
sudo -u /${SSH_USERNAME} pip3 install --user aiocoap pyocd paho-mqtt pyserial flake8 tox \
            pyasn1 ecdsa pexpect pycryptodome ed25519 cbor cryptography \
            scapy codespell protobuf jupyterlab \
            #

# OpenOCD
apt-get install -y build-essential libftdi-dev libhidapi-dev \
        libusb-1.0-0-dev libudev-dev autoconf libsqlite3-dev \
        libpopt-dev libxml2-dev ruby libtool pkg-config

git clone https://github.com/ntfreak/openocd openocd && \
    cd openocd && \
    git checkout 09ac9ab135ed35c846bcec4f7d468c3656852f26 && \
    ./bootstrap && ./configure && \
    make && \
    make install && \
    cd .. && rm -rf openocd

# JLink
JLINK_PKG_DEB="JLink_Linux_V644g_x86_64.deb"
wget -nv http://demo-fit.saclay.inria.fr/vms/utils/${JLINK_PKG_DEB} && \
dpkg -i ${JLINK_PKG_DEB} && \
rm -f ${JLINK_PKG_DEB}
