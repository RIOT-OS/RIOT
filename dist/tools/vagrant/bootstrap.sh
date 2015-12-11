#!/usr/bin/env bash

apt-get update
apt-get install -y pcregrep libpcre3 python3 git gcc-arm-none-eabi gcc-msp430 unzip vim \
    qemu-system-x86 g++-multilib gcc-multilib build-essential gcc-avr binutils-avr avr-libc \
    avrdude doxygen cppcheck python-setuptools libusb-1.0-0 libusb-1.0-0-dev libftdi1 libftdi-dev \
    libftdipp1-dev libftdipp1-2v5 libhidapi-dev libhidapi-hidraw0 libhidapi-libusb0 make cmake \
    autotools-dev autoconf pkg-config jimsh libtool valgrind openocd python-serial python3-serial

# give permissions for serial ports
adduser vagrant dialout

if ! hash st-util 2>/dev/null; then
    git clone --depth 1 https://github.com/texane/stlink stlink.git && cd stlink.git
    ./autogen.sh && ./configure && make && make install
    cd .. && rm -rf stlink.git
fi

# install cli-tools
if ! hash experiment-cli 2>/dev/null; then
    wget -qO - https://github.com/iot-lab/cli-tools/archive/1.6.0.tar.gz | tar xz
    cd cli-tools-1.6.0 && python setup.py install && cd .. && rm -rf cli-tools-1.6.0
fi

# copy udev rules
cp -f RIOT/dist/tools/vagrant/udev_rules/*.rules /etc/udev/rules.d/
udevadm control --reload-rules ; udevadm trigger

# cleanup
apt-get -y autoremove
