FROM ubuntu-22.04

ARG DEBIAN_FRONTEND=noninteractive

ENV HOME=/tmp
COPY install.sh /tmp/install.sh
# binfmt (Rosetta/QEMU) is provided by the host kernel, not the image
RUN bash /tmp/install.sh --native64-binfmt=0 && rm -f /tmp/install.sh
