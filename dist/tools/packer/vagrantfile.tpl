# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
    config.vm.define "RIOT-VM"
    config.vm.box = "RIOT/ubuntu1804"
    config.ssh.username = "user"
    config.vm.synced_folder ".", "/vagrant", disabled: true

    config.vm.provider :virtualbox do |v, override|
        v.gui = false
    end
end
