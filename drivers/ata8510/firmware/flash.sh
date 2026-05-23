#!/bin/bash

firmware="firmware.hex"
if [ ! -z "$1" ]; then
  firmware="$1"
fi

# find programmer
buspath=''
hiddevs=`find /sys/bus/usb/drivers/usbhid/ -type l|egrep -v /sys/bus/usb/drivers/usbhid/module`
for hiddev in $hiddevs
do
  if [ `egrep '^usb:v03EBp2141' ${hiddev}/modalias` ]; then
     buspath=`echo $hiddev | perl -pe 's{^/sys/bus/usb/drivers/usbhid/([^:]+)\:.*}{\1}'`
  fi
done
if [ "x$buspath" == "x" ]; then
  echo "Programmer not found."
  exit 1
fi

# flash firmware
avrdude -c atmelice_isp -p ata8510 -C +ata8510.conf -U "eeprom:w:${firmware}:i"

# reset programmer
echo $buspath | sudo tee /sys/bus/usb/drivers/usb/unbind > /dev/null
sleep .5
echo $buspath | sudo tee /sys/bus/usb/drivers/usb/bind > /dev/null
