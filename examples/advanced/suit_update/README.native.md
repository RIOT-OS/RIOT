# Testing Without Hardware

The SUIT update example is compatible with the native application to try out the
update process without requiring separate hardware. While it is not possible to
update the running code in the application, the workflow can be used to
update memory-backed storage.

- [Quickest start][quickest-start]
- [Introduction][introduction]
- [Workflow][workflow]
  - [Setting up networking][setting-up-networking]
  - [Starting the CoAP server][starting-the-coap-server]
  - [Building and starting the example][building-and-starting-the-example]
  - [Exploring the native instance][exploring-the-native-instance]
  - [Generating the payload and manifest][generating-the-payload-and-manifest]
  - [Updating the storage location][updating-the-storage-location]

## Quickest start
[quickest-start]: #quickest-start

1. Set up networking with:
```console
$ sudo dist/tools/tapsetup/tapsetup -c
$ sudo ip address add 2001:db8::1/64 dev tapbr0
```

2. Start a CoAP server in a separate shell and leave it running:
```
$ aiocoap-fileserver coaproot
```

3. Build and start the native instance:
```
$ BOARD=native make -C examples/advanced/suit_update all term
```
   and add an address from the same range to the interface in RIOT
```console
> ifconfig 5 add 2001:db8::2/64
```

4. Generate a payload and a signed manifest for the payload:
```console
$ echo "AABBCCDD" > coaproot/payload.bin
$ dist/tools/suit/gen_manifest.py --urlroot coap://[2001:db8::1]/ --seqnr 1 -o suit.tmp coaproot/payload.bin:0:ram:0
$ dist/tools/suit/suit-manifest-generator/bin/suit-tool create -f suit -i suit.tmp -o coaproot/suit_manifest
$ dist/tools/suit/suit-manifest-generator/bin/suit-tool sign -k keys/default.pem -m coaproot/suit_manifest -o coaproot/suit_manifest.signed
```

5. Pull the manifest from the native instance:
```
> suit fetch coap://[2001:db8::1]/suit_manifest.signed
```

6. Verify the content of the storage location

```Console
> storage_content .ram.0 0 64
41414242434344440A
```

## Introduction
[introduction]: #introduction

When building the example application for the native target, the firmware update
capability is removed. Instead two in-memory slots are created that can be
updated with new payloads. These act as a demonstrator for the SUIT
capabilities.

The steps described here show how to use SUIT manifests to deliver content
updates to a RIOT instance. The full workflow is described, including the setup
of simple infrastructure.

![Native execution steps](native_steps.svg?sanitize=true)

The steps are as follow: First the network configuration is done. A CoAP server
is started to host the files for the RIOT instance. The necessary keys to sign
the manifest with are generated. After this the RIOT native instance is compiled
and launched. With this infrastructure running, the content and the manifest is
generated. Finally the RIOT instance is instructed to fetch the manifest and
update the storage location with the content.

## Workflow
[workflow]: #workflow

While the above examples use make targets to create and submit the manifest,
this workflow aims to provide a better view of the SUIT manifest and signature
workflow. Because of this the steps below use the low level scripts to manually
creates a payload and manifest and sign it.

### Setting up networking
[setting-up-networking]: #setting-up-networking

To deliver the payload to the native instance, a network connection between a
coap server and the instance is required.

First a bridge with two tap devices is created:

echo "AABBCCDD" > coaproot/payload.bin
dist/tools/suit/gen_manifest.py --urlroot coap://[2001:db8::1]/ --seqnr 1 -o suit.tmp coaproot/payload.bin:0:ram:0
dist/tools/suit/suit-manifest-generator/bin/suit-tool create -f suit -i suit.tmp -o coaproot/suit_manifest
dist/tools/suit/suit-manifest-generator/bin/suit-tool sign -k keys/default.pem -m coaproot/suit_manifest -o coaproot/suit_manifest.signed
```console
$ sudo dist/tools/tapsetup/tapsetup -c
```

This creates a bridge called `tapbr0` and a `tap0` and `tap1`. These last two
tap devices are used by native instances to inject and receive network packets
to and from.

On the bridge device `tapbr0` an routable IP address is added such as
`2001:db8::1/64`:

```console
$ sudo ip address add 2001:db8::1/64 dev tapbr0
```

### Starting the CoAP server
[starting-the-coap-server]: #starting-the-coap-server

As mentioned above, a CoAP server is required to allow the native instance to
retrieve the manifest and payload. The `aiocoap-fileserver` is used for this,
hosting files under the `coaproot` directory:

```console
$ aiocoap-fileserver coaproot
```

This should be left running in the background. A different directory can be used
if preferred.

### Building and starting the example
[building-and-starting-the-example]: #building-and-starting-the-example

Before the natice instance can be started, it must be compiled first.
Compilation can be started from the root of your RIOT directory with:

```
$ BOARD=native make -C examples/advanced/suit_update
```

Then start the example with:

```console
$ BOARD=native make -C examples/advanced/suit_update term
```

This starts an instance of the suit_update example as a process on your
computer. It can be stopped by pressing `ctrl+c` from within the application.

The instance must also be provided with a routable IP address in the same range
configured on the `tapbr0` interface on the host. In the RIOT shell, this can be
done with:

```console
> ifconfig 5 add 2001:db8::2/64
```

Where 5 is the interface number of the interface shown with the `ifconfig`
command.


### Exploring the native instance
[exploring-the-native-instance]: #exploring-the-native-instance

The native instance has two shell commands to inspect the storage backends for
the payloads.

- The `lsstorage` command shows the available storage locations:

```console
> lsstorage
lsstorage
RAM slot 0: ".ram.0"
RAM slot 1: ".ram.1"
VFS 0: "/nvm0/SLOT0.txt"
VFS 1: "/nvm0/SLOT1.txt"
```

As shown above, four storage locations are available, RAM based storage: `.ram.0` and `.ram.1`
as well as VFS based storage: `/nvm0/SLOT0.TXT` and `/nvm0/SLOT1.TXT`.
While multiple slots are available, in this example only the content of the `.ram.0`
slot will be updated, but the procedure is the same for any other slot, just replace
`.ram.0

- The `storage_content` command can be used to display a hex dump command of one
  of the RAM storage locations. It requires a location string, an offset and a
  number of bytes to print:

```console
> storage_content .ram.0 0 64

```
As the storage location is empty on boot, nothing is printed.

For VFS based storage the `vfs` command can be used instead:
```console
> vfs r /nvm0/SLOT0.txt
Error opening file "/nvm0/SLOT0.txt": -ENOENT
```
But as the file does not initially exist, nothing is printed.

### Generating the payload and manifest
[generating-the-payload-and-manifest]: #generating-the-payload-and-manifest

To update the storage location we first need a payload. A trivial payload is
used in this example:

```console
$ echo "AABBCCDD" > coaproot/payload.bin
```

Make sure to store it in the directory selected for the CoAP file server.

Next, a manifest template is created. This manifest template is a JSON file that
acts as a template for the real SUIT manifest. Within RIOT, the script
`dist/tools/suit/gen_manifest.py` is used.

```console
$ dist/tools/suit/gen_manifest.py --urlroot coap://[2001:db8::1]/ --seqnr 1 -o suit.tmp coaproot/payload.bin:0:ram:0
```

or for vfs storage:

```console
$ dist/tools/suit/gen_manifest.py --urlroot coap://[2001:db8::1]/ --seqnr 1 -o suit.tmp coaproot/payload.bin:0:/nvm0/SLOT0.txt
```

This generates a suit manifest template with the sequence number set to `1`, a
payload that should be stored at slot offset zero in slot `.ram.0`. The url for
the payload starts with `coap://[fe80::4049:bfff:fe60:db09]/`. Make sure to
match these with the locations and IP addresses used on your own device.

SUIT supports a check for a slot offset. Within RIOT this is normally used to
distinguish between the different firmware slots on a device. As this is not
used on a native instance, it is set to zero here. The location within a SUIT
manifest is an array of path components. Which character is used to separate
these path components is out of the scope of the SUIT manifest. The
`gen_manifest.py` command uses colons (`:`) to separate these components.
Within the manifest this will show up as an array containing `[ "ram", "0" ]`.

The content of this template file should look like this:

```json
{
    "manifest-version": 1,
    "manifest-sequence-number": 1,
    "components": [
        {
            "install-id": [
                "ram",
                "0"
            ],
            "vendor-id": "547d0d746d3a5a9296624881afd9407b",
            "class-id": "bcc90984fe7d562bb4c9a24f26a3a9cd",
            "file": "coaproot/suit_test.bin",
            "uri": "coap://[fe80::4049:bfff:fe60:db09]/suit_test.bin",
            "bootable": false
        }
    ]
}
```

The manifest version indicates the SUIT manifest specification version numbers,
this will always be 1 for now. The sequence number is the monotonically
increasing anti-rollback counter.

Each component, or payload, also has a number of parameters. The install-id
indicates the unique path where this component must be installed.
The vendor and class ID are used in manifest conditionals to ensure that the
payload is valid for the device it is going to be installed in. It is generated
based on the UUID(v5) of `riot-os.org` and the board name (`native`).

The file and uri are used to generated the URL parameter and the digest in the
manifest. The bootable flag specifies if the manifest generator should instruct
the node to reboot after applying the update.

Generating the actual SUIT manifest from this is done with:

```console
$ dist/tools/suit/suit-manifest-generator/bin/suit-tool create -f suit -i suit.tmp -o coaproot/suit_manifest
```

This generates the manifest in SUIT CBOR format. The content can be inspected by
using the `parse` subcommand:

```console
$ dist/tools/suit/suit-manifest-generator/bin/suit-tool parse -m coaproot/suit_manifest
```

The manifest generated doesn't have an authentication wrapper, it is unsigned
and will not pass inspection on the device or RIOT instance. The manifest can be
signed with the `sign` subcommand together with the keys generated earlier.

```console
$ dist/tools/suit/suit-manifest-generator/bin/suit-tool sign -k keys/default.pem -m coaproot/suit_manifest -o coaproot/suit_manifest.signed
```

This generates an authentication to the manifest. This is visible when
inspecting with the `parse` subcommand. The URL to this signed manifest will be
submitted to the instance so it can retrieve it and in turn retrieve the
component payload specified by the manifest.

### Updating the storage location
[updating-the-storage-location]: #updating-the-storage-location

The update process is a two stage process where first the instance pulls in the
manifest via a supplied url. It will download the manifest and verify the
content. After the manifest is verified, it will proceed with executing the
command sequences in the manifest and download the payload when instructed to.

The URL for the manifest can be supplied to the instance via the command line.

```console
> suit fetch coap://[2001:db8::1]/suit_manifest.signed
```

The payload is the full URL to the signed manifest. The native instance should
respond on this by downloading and executing the manifest. If all went well, the
output of the native instance should look something like this:

```
suit coap://[2001:db8::1]/suit_manifest.signed
suit_coap: trigger received
suit_coap: downloading "coap://[2001:db8::1]/suit_manifest.signed"
suit_coap: got manifest with size 276
suit: verifying manifest signature
suit: validated manifest version
Retrieved sequence number: 0
Manifest seq_no: 1, highest available: 0
suit: validated sequence number
Formatted component name: .ram.0
validating vendor ID
Comparing 547d0d74-6d3a-5a92-9662-4881afd9407b to 547d0d74-6d3a-5a92-9662-4881afd9407b from manifest
validating vendor ID: OK
validating class id
Comparing bcc90984-fe7d-562b-b4c9-a24f26a3a9cd to bcc90984-fe7d-562b-b4c9-a24f26a3a9cd from manifest
validating class id: OK
SUIT policy check OK.
Formatted component name: .ram.0
Fetching firmware |█████████████████████████| 100%
Finalizing payload store
Verifying image digest
Starting digest verification against image
Install correct payload
Verifying image digest
Starting digest verification against image
Install correct payload
```

The storage location can now be inspected using the built-in command. If the
same payload as suggested above was used, it should look like this:

```Console
> storage_content .ram.0 0 64
41414242434344440A
```

Or for vfs storage:

```
> vfs r /nvm0/SLOT0.txt
vfs r /nvm0/SLOT0.txt
00000000: 4141 4242 4343 4444 0a                   AABBCCDD.
-- EOF --
```
The process can be done multiple times for any of the slots and
different payloads. Keep in mind that the sequence number is a strict
monotonically number and must be increased after every update.
