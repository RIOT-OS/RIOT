##### Zedboard README file #####

### JTAG ###
To run using jtag:
	make BOARD=zedboard flash

### QEMU ###
To run on qemu:
	make BOARD=zedboard zynq-qemu

To debug on qemu using GDB:
	make BOARD=zedboard zynq-qemu-debug
and in seperate terminal:
	make BOARD=zedboard debug



### FSBL/Bootfile generation ###
There is a script to create a FSBL for the zedboard and generate a boot.bin file.
The boot.bin file can then be put on a sdcard or put in the QSPI flash.
This requires the Xilinx SDK to be installed.

	make BOARD=zedboard xil-boot-image

