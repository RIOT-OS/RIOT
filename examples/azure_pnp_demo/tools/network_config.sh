#! /bin/bash
echo "configuring network interface"
#sudo ../../dist/tools/tapsetup/tapsetup -d
sudo ../../../dist/tools/tapsetup/tapsetup -c 1
sudo ip address add fd12:dead:beef::1/64 dev tapbr0
#sudo ip a a fec0:affe::1/64 dev tapbr0
#sudo ip a a 192.168.1.10/24 dev tapbr0
ifconfig tapbr0
#make clean all term
echo "ifconfig 6 add fec0:affe::99"
echo "con fec0:affe::1 1883"
echo "configuration done !"
echo "ifconfig 6 add fd12:dead:beef::200/64"
echo "nib route add 6 :: fd12:dead:beef::1"
