#!/usr/bin/env python3

# Copyright (C) 2019, Gabriele Restuccia <restuccia.1548310@studenti.uniroma1.it>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS`` AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from bluepy.btle import Scanner, DefaultDelegate
import os
import sys
import argparse

BLE_IPSS = "1820"	# Internet Protocol Support (0x1820)

DEV_NAME = ""
DEV_SERVICE = ""

NAME_FLAG = False
NAME_FILTER = ""

class ScanDelegate(DefaultDelegate):
	def __init__(self):
		DefaultDelegate.__init__(self)

	def handleDiscovery(self, dev, isNewDev, isNewData):
		if isNewDev:
			# Search for the service in the Incomplete List (0x02)
			if(dev.getValueText(2) is not None):
				DEV_SERVICE = dev.getValueText(2)
				if(NAME_FLAG):
					DEV_NAME = dev.getValueText(9)
					# Filter searching our service in the first 4 bytes GATT hex string (we can ignore what comes after)
					# and by filtering the name of the device (0x09)
					if((BLE_IPSS in DEV_SERVICE[0:8]) and (DEV_NAME == NAME_FILTER)):
						print("Discovered device", dev.addr, DEV_NAME)
						cmd = 'echo "connect '+dev.addr+' 2" > /sys/kernel/debug/bluetooth/6lowpan_control'
						os.system(cmd)
						print("Connection request sent to", dev.addr)
				else:
					# Filter searching our service in the first 4 bytes GATT hex string (we can ignore what comes after)
					if(BLE_IPSS in dev.getValueText(2)[0:8]):
						print("Discovered device", dev.addr)
						cmd = 'echo "connect '+dev.addr+' 2" > /sys/kernel/debug/bluetooth/6lowpan_control'
						os.system(cmd)
						print("Connection request sent to", dev.addr)

def main():
	# Initiate
	print("Loading 6lowpan module...")
	cmd = 'modprobe bluetooth_6lowpan'
	os.system(cmd)
	print("Enabling 6lowpan...")
	cmd = 'echo 1 > /sys/kernel/debug/bluetooth/6lowpan_enable'
	os.system(cmd)
	scanner = Scanner().withDelegate(ScanDelegate())
	scanner.start()
	print("Autoconn started...")
	# Endless loop for scanning BLE devices
	while True:
		scanner.process()

if __name__ == "__main__":

	# Check if user == root
	if os.geteuid() != 0:
	  print("You need to be logged in as root to execute this!\n\nDo 'sudo su' and then execute me.")
	  sys.exit(0)

	# Program description
	text = 	"""This script loads and enables 6lowpan Linux module and then starts
			scanning BLE devices in order to autoconnect to them. You have to choose
			(optionally) what name of the devices you want to filter."""

	# Initiate the parser with a description
	parser = argparse.ArgumentParser(description = text)

	# Add long and short argument
	parser.add_argument("--name", "-n", help="set device name filter")

	# Read arguments from the command line
	args = parser.parse_args()

	# Check for --name
	if args.name:
		NAME_FLAG = True
		NAME_FILTER = args.name

	main()