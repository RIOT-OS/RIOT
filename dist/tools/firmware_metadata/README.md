# Metadata generator for firmware verification
This program will generate a binary file containing a metadata structure as
follows:

```c
typedef struct firmware_metadata {
    uint8_t hash[SHA256_DIGEST_LENGTH]; /**< SHA256 Hash of firmware image */
    uint8_t shash[SIGN_LEN];            /**< Signed SHA256 */
    uint16_t version;                   /**< Integer representing firmware version */
    uint32_t size;                      /**< Size of firmware image */
    uint32_t appid;                     /**< Integer representing the application ID */
} firmware_metadata_t;
```

This structure will be filled with the data obtained from the firmware.

## Requirements
In order to compile this program, you will need to provide the offset taken by
the metadata in itself. This can change depending on the alignment of the
architecture the firmware is compiled for.

For instance:

```makefile
CFLAGS+=-DFW_METADATA_SPACE=256 make clean all
```

will create the metadata binary in a space of 256 bytes.

## Usage
To use, you should call `generate-metadata` with the following arguments:

```console
./generate-metadata <firmware.bin> <version> <appid> [output-path] [--with-metadata]
```

Where:

_\<firmware.bin\>\:_ The firmware in binary format

_\<version\>\:_ The firmware version in 16-bit HEX

_\<appid\>_\: ID for the application in 32-bit HEX

_\[output-path\]_\: The path for fimrware_metadata.bin

_\[--with-metadata\]_\: This will take into account existing metadata in the
firmware

The results will be printed if the operation is successful, and a binary
called "_\[output-path\]_\/firmware-metadata.bin" will be created.
