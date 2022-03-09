# MCUBoot test application
This test is intended to compile a hello-world program taking into account
the existence of the MCUBoot bootloader at the first 32K in the ROM.

For this first support, a pre-compiled mynewt MCUBoot binary is downloaded at
compile time.

The goal is to produce an ELF file which is linked to be flashed at a
`BOOTLOADER_OFFSET` offset rather than the beginning of ROM. MCUBoot also
expects an image padded with some specific headers containing the version
information, and TLVs with hash and signing information. This is done through
the imgtool.py application, which is executed automatically by the build
system.

Before running the test, be sure that you meet the following Python3
dependencies:

  - pycryptodome
  - ecdsa
  - pyasn1

If you don't have one of those, you can install them with the commands:

```console
pip3 install --user pycryptodome ecdsa pyasn1
```

This test can be called using `make mcuboot` to produce such ELF file,
which can also be flashed using `make mcuboot-flash`.This command also flashes
the pre-compiled bootloader.

It's also possible to build and flash MCUBoot by following the instructions on
the MCUBoot repository either using mynewt or zephyr operating systems.
