#
# RIOT Dockerfile
#
# the resulting image will contain everything needed to build RIOT.
#
# Setup: (only needed once per Dockerfile change)
# 1. install docker, add yourself to docker group, enable docker, relogin
# 2. # docker build -t riotbuild .
#
# Usage:
# 3. cd to riot root
# 4. # docker run -i -t -u $UID -v $(pwd):/data/riotbuild riotbuild ./dist/tools/compile_test/compile_test.py
#
# If you want to use a persistent ccache, map a directory to '/data/ccache'
# and set CCACHE=ccache:
#
# 4. # docker run -i -t -u $UID -v $(pwd):/data/riotbuild -v /tmp/riot_ccache:/data/ccache \
#           -e CCACHE=ccache -e RIOT_VERSION_OVERRIDE=buildtest \
#            riotbuild ./dist/tools/compile_test/compile_test.py
#

FROM ubuntu

MAINTAINER Kaspar Schleiser <kaspar@schleiser.de>

ENV DEBIAN_FRONTEND noninteractive

RUN echo "deb http://ppa.launchpad.net/terry.guo/gcc-arm-embedded/ubuntu trusty main" > /etc/apt/sources.list.d/gcc-arm-embedded.list
RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-key FE324A81C208C89497EFC6246D1D8367A3421AFB

RUN apt-get update
RUN apt-get -y dist-upgrade

RUN apt-get -y install build-essential \
 git \
 gcc-multilib \
 gcc-arm-none-eabi \
 gcc-msp430 \
 pcregrep libpcre3 \
 qemu-system-x86 python3 \
 g++-multilib \
 gcc-avr binutils-avr avr-libc \
 subversion curl wget python p7zip unzip

RUN wget http://launchpadlibrarian.net/206632429/ccache_3.2.2-2_amd64.deb \
        -O /tmp/ccache_3.2.2-2_amd64.deb \
        && dpkg -i /tmp/ccache_3.2.2-2_amd64.deb

RUN mkdir -m 777 -p /data/ccache
ENV CCACHE_DIR /data/ccache

RUN mkdir -p /data/riotbuild
WORKDIR /data/riotbuild
