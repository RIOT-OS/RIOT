# Manifest Generator

This repository contains a tool to generate manifests following the specification in https://tools.ietf.org/html/draft-ietf-suit-manifest-09.

# Installing

First clone this repo:

```
$ git clone https://github.com/ARMmbed/suit-manifest-generator.git
```

Next, use pip to install the repo:

```
$ cd suit-manifest-generator
$ python3 -m pip install --user --upgrade .
```

# Input File Description

The input file is organised into four high-level elements:

* `manifest-version` (a positive integer), the version of the manifest specification
* `manifest-sequence-number` (a positive integer), the anti-rollback counter of the manifest
* `components`, a list of components that are described by the manifest


Each component is a JSON map that may contain the following elements. Some elements are required for the target to be able to install the component.

Required elements:

* `install-id` (a Component ID), the identifier of the location to install the described component.
* `install-digest` (a SUIT Digest), the digest of the component after installation.
* `install-size` (a positive integer), the size of the component after installation.
* `vendor-id` (a RFC 4122 UUID), the UUID for the component vendor. This must match the UUID that the manifest processor expects for the specified `install-id`. The suit-tool expects at least one component to have a `vendor-id`
* `class-id` (a RFC 4122 UUID), the UUID for the component. This must match the UUID that the manifest processor expects for the specified `install-id`. The `suit-tool` expects at least one component with a `vendor-id` to also have a `class-id`
* `file` (a string), the path to a payload file. The `install-digest` and `install-size` will be calculated from this file.

Some elements are not required by the tool, but are necessary in order to accomplish one or more use-cases.

Optional elements:

* `bootable` (a boolean, default: `false`), when set to true, the `suit-tool` will generate commands to execute the component, either from `install-id` or from `load-id` (see below)
* `uri` (a text string), the location at which to find the payload. This element is required in order to generate the `payload-fetch` and `install` sections.
* `loadable` (a boolean, default: `false`), when set to true, the `suit-tool` loads this component in the `load` section.
* `compression-info` (a choice of string values), indicates how a payload is compressed. When specified, payload is decompressed before installation. The `install-size` must match the decompressed size of the payload and the install-digest must match the decompressed payload. N.B. The suit-tool does not perform compression. Supported values are:

    * `gzip`
    * `bzip2`
    * `deflate`
    * `lz4`
    * `lzma`

* `download-digest` (a SUIT Digest), a digest of the component after download. Only required if `compression-info` is present and `decompress-on-load` is `false`.
* `decompress-on-load` (a boolean, default: `false`), when set to true, payload is not decompressed during installation. Instead, the payload is decompressed during loading. This element has no effect if `loadable` is `false`.
* `load-digest` (a SUIT Digest), a digest of the component after loading. Only required if `decompress-on-load` is `true`.
* `install-on-download` (boolean, default: true), If true, payload is written to `install-id` during download, otherwise, payload is written to `download-id`.
* `download-id` (a component id), the location where a downloaded payload should be stored before installation--only used when `install-on-download` is `false`.

## Component ID

The `suit-tool` expects component IDs to be a JSON list of strings. The `suit-tool` converts the strings to bytes by:

1. Attempting to convert from hex
2. Attempting to convert from base64
3. Encoding the string to UTF-8

For example,

* `["00"]` will encode to `814100` (`[h'00']`)
* `["0"]` will encode to `814130` (`[h'30']`)
* `["MTIzNA=="]` will encode to `814431323334` (`[h'31323334']`)
* `["example"]` will encode to `81476578616D706C65` (`[h'6578616d706c65']`)

N.B. Be careful that certain strings can appear to be hex or base64 and will be treated as such. Any characters outside the set `[0-9a-fA-F]` ensure that the string is not treated as hex. Any characters outside the set `[0-9A-Za-z+/]` or a number of characters not divisible by 4 will ensure that the string is not treated as base64.

## SUIT Digest

The format of a digest is a JSON map:

```JSON
{
    "algorithm-id" : "sha256",
    "digest-bytes" : "base64-or-hex"
}
```

The `algorithm-id` must be one of:

* `sha224`
* `sha256`
* `sha384`
* `sha512`

The `digest-bytes` is a string of either hex- or base64-encoded bytes. The same decoding rules as those in Component ID are applied.

## Example Input File

```JSON
{
    "components" : [
        {
            "download-id" : ["01"],
            "install-id" : ["00"],
            "install-digest": {
                "algorithm-id": "sha256",
                "digest-bytes": "00112233445566778899aabbccddeeff0123456789abcdeffedcba9876543210"
            },
            "install-size" : 34768,
            "uri": "http://example.com/file.bin",
            "vendor-id" : "fa6b4a53-d5ad-5fdf-be9d-e663e4d41ffe",
            "class-id" : "1492af14-2569-5e48-bf42-9b2d51f2ab45",
            "bootable" : true,
            "install-on-download" : false,
            "loadable" : true,
            "decompress-on-load" : true,
            "load-id" : ["02"],
            "compression-info" : "gzip",
            "load-digest" : {
                "algorithm-id": "sha256",
                "digest-bytes": "0011223344556677889901234567899876543210aabbccddeeffabcdeffedcba"
            },
        },
        {
            "install-id" : ["03", "01"],
            "install-digest": {
                "algorithm-id": "sha256",
                "digest-bytes": "0123456789abcdeffedcba987654321000112233445566778899aabbccddeeff"
            },
            "install-size" : 76834,
            "uri": "http://example.com/file2.bin"
        }
    ],
    "manifest-version": 1,
    "manifest-sequence-number": 7
}
```

# Invoking the suit-tool

The `suit-tool` supports three sub-commands:

* `create` generates a new manifest.
* `sign` signs a manifest.
* `parse` parses an existing manifest into cbor-debug or a json representation.
* `keygen` Create a signing key. Not for production use.
* `pubkey` Get the public key for a supplied private key in uECC-compatible C definition.

The `suit-tool` has a configurable log level, specified with `-l`:

* `suit-tool -l debug` verbose output
* `suit-tool -l info` normal output
* `suit-tool -l warning` suppress informational messages
* `suit-tool -l exception` suppress warning and informational messages

## Create

To create a manifest, invoke the `suit-tool` with:

```sh
suit-tool create -i IFILE -o OFILE
```

The format of `IFILE` is as described above. `OFILE` defaults to a CBOR-encoded SUIT manifest.

`-f` specifies the output format:

* `suit`: CBOR-encoded SUIT manifest
* `suit-debug`: CBOR-debug SUIT manifest
* `json`: JSON-representation of a SUIT manifest

The `suit-tool` can generate a manifest with severable fields. To enable this mode, add the `-s` flag.

To add a component to the manifest from the command-line, use the following syntax:

```
-c 'FIELD1=VALUE1,FIELD2=VALUE2'
```

The supported fields are:

* `file` the path fo a file to use as a payload file.
* `inst` the `install-id`.
* `uri` the URI where the file will be found.

## Sign

To sign an existing manifest, invoke the `suit-tool` with:

```sh
suit-tool sign -m MANIFEST -k PRIVKEY -o OFILE
```

`PRIVKEY` must be a secp256r1 ECC private key in PEM format.

If the COSE Signature needs to indicate the key ID, add a key id with:

```
-i KEYID
```

## Parse

To parse an existing manifest, invoke the `suit-tool` with:

```sh
suit-tool parse -m MANIFEST
```

If a json-representation is needed, add the '-j' flag.

## Keygen

Create an asymmetric keypair for non-production use. Production systems should use closely guarded keys, such as keys stored in an HSM.

```sh
 suit-tool keygen [-t TYPE] -o KEYFILE
 ```

`suit-tool keygen` defaults to creating SECP256r1 keys. To create another type of key, use `-t`followed by one of:

* `secp256r1`
* `secp384r1`
* `secp521r1`
* `ed25519`

## UECC public key

Derive a public key in the format used by micro ECC. The input is a PEM private key.

```sh
suit-tool pubkey -k FILE
```

The tool will then print the public key in micro ECC format.
