# MCUBoot test application
This test is intended to compile a hello-world program taking into account
the existence of the MCUBoot bootloader at the first 32K in the ROM.

The goal is to produce an ELF file which is linked to be flashed at a
`BOOTLOADER_OFFSET` offset rather than the beginning of ROM. MCUBoot also
expects an image padded with some specific headers containing the version
information, and TLVs with hash and signing information. This is done through
the imgtool.py application, which is executed automatically by the build
system.

This test should be called using `make mcuboot` to produce such ELF file,
which can also be flashed using `make mcuboot flash`.

In order to get the program to run, it is necessary to have MCUBoot flashed
into the device. This can be achieved by following the instructions on the
MCUBoot repository either using mynewt or zephyr operating systems.

The only supported platform at the moment is the FRDM-K64F, more will come
in the near future.

This is a first step towards MCUBoot full integration on RIOT.