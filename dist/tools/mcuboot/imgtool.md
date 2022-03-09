## Image tool

The Python program `imgtool.py` can be used to perform the
operations that are necessary to manage keys and sign images.

This program is written for Python3, and has several dependencies on
Python libraries.  These can be installed using 'pip3' manually:

    pip3 install --user pycryptodome
    pip3 install --user pyasn1
    pip3 install --user ecdsa

or, on Ubuntu, using the package manager:

    sudo apt-get install python3-crypto python3-pyasn1 python3-ecdsa

## Managing keys

This tool currently supports rsa-2048 and ecdsa-p256 keys.  You can
generate a keypair for one of these types using the 'keygen' command:

    ./imgtool.py keygen -k filename.pem -t rsa-2048

or use ecdsa-p256 for the type.  The key type used should match what
mcuboot is configured to verify.

This key file is what is used to sign images, this file should be
protected, and not widely distributed.

## Incorporating the public key into the code

There is a development key distributed with mcuboot that can be used
for testing.  Since this private key is widely distributed, it should
never be used for production.  Once you have generated a production
key, as described above, you should replace the public key in the
bootloader with the generated one.

For Zephyr, the keys live in the file `boot/zephyr/keys.c`.  For
mynewt, follow the instructions in `doc/signed_images.md` to generate
the key file.

    ./imgtool.py getpub -k filename.pem

will extract the public key from the given private key file, and
output it as a C data structure.  You can replace or insert this code
into the key file.

## Signing images

Image signing takes a binary image intended for Slot 0 and adds a
header and trailer that the bootloader is expecting:

    usage: imgtool.py sign [-h] -k filename --align ALIGN -v VERSION -H
                           HEADER_SIZE [--pad PAD] [--rsa-pkcs1-15]
                           infile outfile

    positional arguments:
      infile
      outfile

    optional arguments:
      -h, --help            show this help message and exit
      -k filename, --key filename
      --align ALIGN
      -v VERSION, --version VERSION
      -H HEADER_SIZE, --header-size HEADER_SIZE
      --included-header     Image has gap for header
      --pad PAD             Pad image to this many bytes, adding trailer magic
      --rsa-pkcs1-15        Use old PKCS#1 v1.5 signature algorithm

The main arguments given are the key file generated above, a version
field to place in the header (1.2.3 for example), the alignment of the
flash device in question, and the header size.

The header size depends on the operating system and the particular
flash device.  For Zephyr, it will be configured as part of the build,
and will be a small power of two.  By default, the header will be
prepended to the image.  If `--included-header` is given, the image
must start with header-size bytes of zeros, and the header will be
overwritten over these bytes.

The optional --pad argument will place a trailer on the image that
indicates that the image should be considered an upgrade.  Writing
this image in slot 1 will then cause the bootloader to upgrade to it.

Lastly, the --rsa-pkcs1-15 will cause the tool to use the older,
deprecated pkcs#1 v1.5 signing algorithm when using RSA.  This can be
enabled in the bootloader as well, and may be needed if you are using
an older version of the bootloader.
