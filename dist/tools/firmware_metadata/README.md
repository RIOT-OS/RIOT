# Metadata generator for firmware verification
This program generates a binary file containing a metadata structure as
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

This structure is filled with the data obtained from the firmware.

## Usage
To use it, call `generate-metadata` with the following arguments:

```console
./generate-metadata <firmware.bin> <version> <appid> <output-path>
```

Where:

_\<firmware.bin\>\:_ The firmware in binary format

_\<version\>\:_ The firmware version in 16-bit HEX format

_\<appid\>_\: ID for the application in 32-bit HEX format

_\<output-path/filename.bin\>_\: The path and name of the metadata binary file

If the operation succeeds, the results are printed out and a binary called
"firmware-metadata.bin" is written, if no _output-path_ option is specified.
Otherwise, the file is written to the given path with the given name.
