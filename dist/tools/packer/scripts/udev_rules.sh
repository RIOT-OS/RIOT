# Udev rules
# JLink
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="1366", ATTRS{idProduct}=="1015", MODE="0664", GROUP="dialout"' \
    > /etc/udev/rules.d/riot-dev.rules

# ST-Link
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", MODE="0664", GROUP="dialout"' \
    >> /etc/udev/rules.d/riot-dev.rules
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3748", MODE="0666", GROUP="dialout"' \
    >> /etc/udev/rules.d/riot-dev.rules

# UBSASP
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="05dc", MODE="0666", GROUP="dialout"' \
    >> /etc/udev/rules.d/riot-dev.rules

# USBTTL
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", MODE="0666", GROUP="dialout"' \
    >> /etc/udev/rules.d/riot-dev.rules

# ATmega32u4
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="2341", ATTRS{idProduct}=="0036", MODE="0666", GROUP="dialout"' \
    >> /etc/udev/rules.d/riot-dev.rules

# ATmega2560
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="2341", ATTRS{idProduct}=="0042", MODE="0666", GROUP="dialout"' \
    >> /etc/udev/rules.d/riot-dev.rules

# CMSIS-DAP (EDBG)
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="03eb", ATTRS{idProduct}=="2111", MODE="0664", GROUP="dialout"' \
    >> /etc/udev/rules.d/riot-dev.rules
echo 'KERNEL=="hidraw*", ATTRS{idVendor}=="03eb", ATTRS{idProduct}=="2111", MODE="664", GROUP="dialout"' \
    >> /etc/udev/rules.d/riot-dev.rules

# Arduino Zero
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="03eb", ATTRS{idProduct}=="2157", MODE="0664", GROUP="dialout"' \
    >> /etc/udev/rules.d/riot-dev.rules
echo 'KERNEL=="hidraw*", ATTRS{idVendor}=="03eb", ATTRS{idProduct}=="2157", MODE="664", GROUP="dialout"' \
    >> /etc/udev/rules.d/riot-dev.rules

# DAPLink
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="0d28", ATTRS{idProduct}=="0204", MODE="0664", GROUP="dialout"' \
    >> /etc/udev/rules.d/riot-dev.rules
echo 'KERNEL=="hidraw*", ATTRS{idVendor}=="0d28", ATTRS{idProduct}=="0204", MODE="664", GROUP="dialout"' \
    >> /etc/udev/rules.d/riot-dev.rules

# IoT-LAB M3
echo 'SUBSYSTEM=="usb", ATTR{idProduct}=="6010", ATTR{idVendor}=="0403", MODE="0664", GROUP="dialout"' \
    >> /etc/udev/rules.d/riot-dev.rules
echo 'SUBSYSTEM=="usb", ATTR{idProduct}=="6011", ATTR{idVendor}=="0403", MODE="0664", GROUP="dialout"' \
    >> /etc/udev/rules.d/riot-dev.rules
