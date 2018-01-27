# -*- mode: ruby -*-
# vi: set ft=ruby :

require 'fileutils'

RIOTBASE ||= "./"

Vagrant.configure(2) do |config|
  # For a complete reference, please see the online documentation at https://docs.vagrantup.com.

  config.vm.box = "RIOT/ubuntu1604"

  config.vm.provider "virtualbox" do |vb|
    vb.name = "RIOT-VM"

    vb.customize [ "guestproperty", "set", :id, "/VirtualBox/GuestAdd/VBoxService/--timesync-interval", 10000]
    vb.customize [ "guestproperty", "set", :id, "/VirtualBox/GuestAdd/VBoxService/--timesync-min-adjust", 100]
    vb.customize [ "guestproperty", "set", :id, "/VirtualBox/GuestAdd/VBoxService/--timesync-set-on-restore", 1]
    vb.customize [ "guestproperty", "set", :id, "/VirtualBox/GuestAdd/VBoxService/--timesync-set-threshold", 10000]

    # additional USB passthrough entries
    # vb.customize ['usbfilter', 'add', '0', '--target', :id, '--name', '<custom_name>', '--vendorid', '<vID>', '--productid', '<pID>']
  end

  config.vm.synced_folder RIOTBASE, "/home/vagrant/RIOT"

  if File.exists?(File.join(Dir.home, ".gitconfig"))
    config.vm.provision "file", source: File.join(Dir.home, ".gitconfig"), destination: ".gitconfig"
  end
# use a custom provisioning script
# config.vm.provision "shell", path: "dist/tools/vagrant/bootstrap.sh"
end
