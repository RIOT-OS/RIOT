# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure(2) do |config|
  # For a complete reference, please see the online documentation at
  # https://docs.vagrantup.com.

  # Every Vagrant development environment requires a box. You can search for
  # boxes at https://atlas.hashicorp.com/search.
  config.vm.box = "boxcutter/ubuntu1510"

  # Disable automatic box update checking. If you disable this, then
  # boxes will only be checked for updates when the user runs
  # `vagrant box outdated`. This is not recommended.
  # config.vm.box_check_update = false

  config.vm.network "forwarded_port", guest: 4242, host: 4242

  # Share an additional folder to the guest VM. The first argument is
  # the path on the host to the actual folder. The second argument is
  # the path on the guest to mount the folder. And the optional third
  # argument is a set of non-required options.
  # config.vm.synced_folder "../data", "/vagrant_data"

  # Provider-specific configuration so you can fine-tune various
  # backing providers for Vagrant. These expose provider-specific options.
  config.vm.provider "virtualbox" do |vb|
  #   # Display the VirtualBox GUI when booting the machine
  #   vb.gui = true
  #
  #   # Set name of the VM
    vb.name = "RIOT-OS VM"
  #
  #   # Customize the amount of memory on the VM:
  #   vb.memory = "1024"
  #   # enable usb passthrough
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
  end

  config.vm.provision "file", source: "~/.gitconfig", destination: ".gitconfig"
  config.vm.provision "shell", path: "dist/tools/vagrant/bootstrap.sh"
end
