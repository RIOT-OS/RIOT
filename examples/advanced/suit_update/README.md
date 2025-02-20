# Overview

This example shows how to integrate SUIT-compliant firmware updates into a
RIOT application. It implements basic support of the SUIT architecture using
the manifest format specified in
[draft-ietf-suit-manifest-09](https://tools.ietf.org/id/draft-ietf-suit-manifest-09.txt).

**WARNING**: This code should not be considered production ready for the time being.
             It has not seen much exposure or security auditing.

This document describes the preliminary requirements for using the SUIT workflow
to update binaries on RIOT.

Table of Contents:

- [Prerequisites][prerequisites]
  - [Signing key management][key-management]
- [workflows][workflows]

## Prerequisites
[prerequisites]: #Prerequisites

- Install python dependencies (only Python3.6 and later is supported):

      $ pip3 install --user cbor2 cryptography

- Install aiocoap from the source

      $ pip3 install --user aiocoap[linkheader]>=0.4.1

  See the [aiocoap installation instructions](https://aiocoap.readthedocs.io/en/latest/installation.html)
  for more details.

- add `~/.local/bin` to PATH

  The aiocoap tools are installed to `~/.local/bin`. Either add
  "export `PATH=$PATH:~/.local/bin"` to your `~/.profile` and re-login, or execute
  that command *in every shell you use for this tutorial*.

- Clone this repository:

      $ git clone https://github.com/RIOT-OS/RIOT
      $ cd RIOT

- In all hardware-based setup below, `ethos` (EThernet Over Serial) is used to
  provide an IP link between the host computer and a board.

  Just build `ethos` and `uhcpd` with the following commands:

      $ make -C dist/tools/ethos clean all
      $ make -C dist/tools/uhcpd clean all

  It is possible to interact with the device over it's serial terminal as usual
  using `make term`, but that requires an already set up tap interface.
  See [update] for more information.


### Key Management
[key-management]: #Key-management

SUIT keys consist of a private and a public key file, stored in `$(SUIT_KEY_DIR)`.
Similar to how ssh names its keyfiles, the public key filename equals the
private key file, but has an extra `.pub` appended.

`SUIT_KEY_DIR` defaults to the `keys/` folder in `$XDG_DATA_HOME/RIOT`
(usually `~/.local/share/RIOT/keys`).

If the chosen key doesn't exist, it will be generated automatically.
That step can be done manually using the `suit/genkey` target.

## Workflows
[workflows]: #workflows

Two workflows are available with this example. The first one demonstrates the
SUIT workflow on a RIOT native instance on Linux.
The workflow described aims to update the firmware on real-world hardware.

- [SUIT on RIOT native](README.native.md)
- [SUIT on hardware](README.hardware.md)
