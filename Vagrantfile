# -*- mode: ruby -*-
# vi: set ft=ruby :

require 'fileutils'

RIOTBASE ||= "./"

Vagrant.configure(2) do |config|
  # For a complete reference, please see the online documentation at https://docs.vagrantup.com.

  config.vm.box = "boxcutter/ubuntu1604"
  config.vm.synced_folder RIOTBASE, "/home/vagrant/RIOT"

  config.vm.provider "virtualbox" do |vb|
    vb.name = "RIOT VM"
    vb.customize ["modifyvm", :id, "--usb", "on"]
    vb.customize ["modifyvm", :id, "--usbxhci", "on"]
    vb.customize ['usbfilter', 'add', '0', '--target', :id, '--name', 'FTDI USB-TTL',
                  '--vendorid', '0x0403', '--productid',  '0x6001']
    vb.customize ['usbfilter', 'add', '0', '--target', :id, '--name', 'CP2102 USB to UART',
                  '--vendorid', '0x10c4', '--productid',  '0xea60']
    vb.customize ['usbfilter', 'add', '0', '--target', :id, '--name', 'STM32 STLink',
                  '--vendorid', '0x0483', '--productid',  '0x3748']
    vb.customize ['usbfilter', 'add', '0', '--target', :id, '--name', 'USBasp',
                  '--vendorid', '0x16c0', '--productid',  '0x05dc']
    vb.customize ['usbfilter', 'add', '0', '--target', :id, '--name', 'iotlab-m3',
                  '--vendorid', '0x0403', '--productid',  '0x6010']
    vb.customize ['usbfilter', 'add', '0', '--target', :id, '--name', 'samr21-xpro',
                  '--vendorid', '0x03eb', '--productid',  '0x2111']
    vb.customize ['usbfilter', 'add', '0', '--target', :id, '--name', 'Arduino Mega 2560',
                  '--vendorid', '0x2341', '--productid',  '0x0042']
    vb.customize ['usbfilter', 'add', '0', '--target', :id, '--name', 'Phytec phyWAVE KW22',
                  '--vendorid', '0x0d28', '--productid',  '0x0204']
  end

  if File.exists?(File.join(Dir.home, ".gitconfig"))
    config.vm.provision "file", source: File.join(Dir.home, ".gitconfig"), destination: ".gitconfig"
  end

  config.vm.provision "shell", path: File.join(RIOTBASE,'/dist/tools/vagrant/bootstrap.sh')
end
