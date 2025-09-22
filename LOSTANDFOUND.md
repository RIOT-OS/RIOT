# Removed Features and Modules

This document contains a listing of all features and modules that were removed
from RIOT at some point in time, mostly due to missing maintenance. The purpose
of this list is to have a reference point for reintegrating those features in
the future, if new interest arises or a new maintainer is found.

This list is **not** supposed to contain a change log of all the things that are
being removed from RIOT, but should contain only full modules that are
removed without any alternative being merged at the same time. For example
things like the first `netdev` should not be put in this list, as a successor
(former `netdev2`) has been merged.

## How to read this list

For each high-level feature removed there should be one entry in this list. The
entries should comply to the following template:

```markdown
### path/feature_name [HASH of removal commit]
Author(s):
- author 1 <foo.bar@abc.com>
- author 2 <a.b@c.net>

Reason for removal:
- give a short and comprehensive reasoning why this feature was removed
- typical reasons are:
- feature is not maintained anymore
- lack of hardware so feature can not be tested (anymore)
- feature was never used and there is no reason for keeping it
```

Additionally, a link reference for the commit on GitHub is added to the bottom of this document.

By putting the name of the removed feature and the commit hash in the same line,
one can very quickly find the commit using `git grep`.

Listing the authors of the removed code is done for appreciation of their work.
This way, their names are never removed from the RIOT repository.

## Removed Features

### boards/chronos [ed3887ac5c1e95308c2827bce3cdca8b0f146c22]

Author(s):

- Oliver Hahm <oliver.hahm@inria.fr>
- Kaspar Schleiser <kaspar@schleiser.de>

Reason for removal:

- board is quite outdated
- board cannot be automatically tested for lack of UART
- board has many special cases, which hurt upgrade to msp430-elf-gcc 9.x
- the onboard radio has been unsupported for a while
- no one seems to use the board, no one seems to maintain it

### boards/fox [81458c8eed8949c686d5ded652dbee10748e860b]

Author(s):

- Thomas Eichinger <thomas.eichinger@fu-berlin.de>

Reason for removal:

- Hardware not available so can't be tested

### boards/jiminy-mega256rfr2 [232aed3e18118624b862d36bfec7cd1c21ca2d26]

Author:

- Josua Arndt <jarndt@ias.rwth-aachen.de>
- Steffen Robertz <steffen.robertz@rwth-aachen.de>

Reasons for removal:

- The board is a custom design at the RWTH Aachen, so only two persons have
  access to the hardware
- It was unmaintained for several month now
- Various refactoring PRs affected the ATmega CPU family have not been tested,
  it is unclear if the board is actually usable with RIOT

### boards/mips-malta [ee6b6b9c388b78fcec7ba6e239a6c76041b9bbb7]

Author:

- Neil Jones <neil.jones@imgtec.com>

Reasons for removal:

- UART input not supported.
- Hardware not available for testing and not available for purchase either.
- Not actively maintained / broken for some time.

### boards/nrf6310 [4953ba8e6759d2b1a1a1ea497a4ad1e71489195a]

Author(s):

- Christian Kühling <kuehling@zedat.fu-berlin.de>
- Timo Ziegler <timo.ziegler@fu-berlin.de>
- Hauke Petersen <hauke.petersen@fu-berlin.de>

Reason for removal:

- this was an early development board by Nordic which was discontinued
 (successor: `nrf51dk`)
- also for a `BLE DEVKIT.N` board from Mommosoft, which was discontinued as well
- no hardware available anymore for testing, many exclusions from CI

### boards/pca10000 [9447cb303426d7c6348bb84999f88bf929cd6263]

Author(s):

- Christian Kühling <kuehling@zedat.fu-berlin.de>
- Timo Ziegler <timo.ziegler@fu-berlin.de>
- Hauke Petersen <hauke.petersen@fu-berlin.de>

Reason for removal:

- this was an early development board by Nordic which was discontinued (successor: `nrf51dongle`)
- no hardware available anymore for testing

### boards/pca10005 [ea73cc49eacd45640b9660134c4c92b887cb2716]

Author(s):

- Christian Kühling <kuehling@zedat.fu-berlin.de>
- Timo Ziegler <timo.ziegler@fu-berlin.de>
- Hauke Petersen <hauke.petersen@fu-berlin.de>

Reason for removal:

- this was an early development board by Nordic which was discontinued (successor: `nrf51dk`)
- no hardware available anymore for testing

### boards/qemu-i386 [99009af25e201bbc182d376e99df34133417be6c]

Author(s):

- René Kijewski <rene.kijewski@fu-berlin.de>

Reason for removal:

- code broken and excluded from all tests for a long time
- no maintainer available

### boards/slwstk6220a [cab1ea66b49e3c72e2827c8686c09d84ae3ef9a9]

Author(s):

- Hauke Petersen <hauke.petersen@fu-berlin.de>
- Bas Stottelaar <basstottelaar@gmail.com>

Reason for removal:

- The old implementation of the SLWSTK6220A based on `cpu/ezr32wg` did not use
  the Gecko SDK, that other EFM32-based boards do use
- The old implementation had limited peripherals supported
- New implementation (with the same name) that is based on `cpu/efm32` was
  added in commit [fe941ac9fe3f81c0f08ff3b8564cf439639abcda]

### boards/waspmote-pro [2b8a0d48940517f7df4e78c7a0b16024f46a8694]

Author(s):

- Hinnerk van Bruinehsen
- Kaspar Schleiser <kaspar@schleiser.de>
- Francisco Acosta
- J. David Ibáñez <jdavid.ibp@gmail.com>

Reason for removal:

- This board is sold only B2B, so that our community members from the
DIY/hobbyist community and academia do not have access to the board. Our
community members from the industry mostly develop their own hardware and,
therefore, have no interest in supporting this board. As result, none of the
RIOT core contributors has access to the hardware, preventing us from doing the
necessary testing for maintaining this board.

### boards/weio [cfa9580f319508f858c8fe30ecce8b2b59b6caa3]

Author(s):

- Paul Rathgeb <paul.rathgeb@skynet.be>

Reason for removal:

- hardware not available to the community for testing
- original author and maintainer won't be able to maintain the code

### boards/wsn430-v1_3b, boards/wsn430-v1_4 [e63cd54f3b1e002a7895bb7c46af889b341c1a15]

Author(s):

- Hauke Petersen <hauke.petersen@fu-berlin.de>
- Martine Lenders <m.lenders@fu-berlin.de>
- Milan Babel <babel@inf.fu-berlin.de>
- Oliver Hahm <oliver.hahm@inria.fr>

Reason for removal:

- no longer accessible on the IoT-LAB testbed

### boards/x86-multiboot-common [b3eb9b8cf23f0702fa725d536df6ad6528f84189]

Author(s):

- René Kijewski <rene.kijewski@fu-berlin.de>

Reason for removal:

- code broken and excluded from all tests for a long time
- no maintainer available

### cpu/lpc11u34 [7bc271807cecbffbb01a37c56a367b98fb823573]

Author(s):

- Paul Rathgeb <paul.rathgeb@skynet.be>

Reason for removal:

- hardware not available to the community for testing
- original author and maintainer won't be able to maintain the code

### cpu/mips32r2_generic [a2bcd7539ce1931b7aec0077ea71dadd62c96edd]

Author:

- Neil Jones <neil.jones@imgtec.com>

- No boards use this CPU (the only one was mips-malta).
- (Same reasons as mips-malta)

### cpu/mips* [6cad5d24771ba6199228351a11b5062cd2e9b36d]

Author:

- Neil Jones <Neil.Jones@imgtec.com>

Reasons for removal:

- Not maintained anymore
- Current state is not very useful and nobody claimed to use it

### cpu/x86 [fc45d7c5ac3d46bfdfb21c243cd3d4881bc5c836]

Author(s):

- René Kijewski <rene.kijewski@fu-berlin.de>

Reason for removal:

- code broken and excluded from all tests for a long time
- no maintainer available

### GNRC_NETTYPE_IOVEC [4f243c52eabefe709d78560ce7f1d502d737a999]

Author(s):

- Martine S. Lenders <m.lenders@fu-berlin.de>
- Hauke Petersen <devel@haukepetersen.de>

Reason for removal:

- Unused since [9fb2f541baca469e34fa01b004d6f19385700ce9]

### gnrc_pktbuf_duplicate_upto() [b83430aa625a1d42f11f9badf5e5cfbb8efacd99]

Author(s):

- Takuo Yonezawa <yonezawa.t2@gmail.com>
- Martine S. Lenders <m.lenders@fu-berlin.de>

Reason for removal:

- broke the abstraction of `gnrc_pktbuf`
- its only user within the RIOT code base `gnrc_ipv6_ext` was reworked in
  [f671a87fe2c539c3aecd595ae03fa4f6f209d042] so it is not needed anymore.
- the function was deprecated in [f2760c033c5f332be076b25aa212aca4007c3d65]

### gnrc_pktbuf_replace_snip() [72821a502f073006643cb4ef7815fc8c42563ce6]

Author(s):

- Joakim Nohlgård <joakim.nohlgard@eistec.se>
- Martine S. Lenders <m.lenders@fu-berlin.de>

Reason for removal:

- Unused, untested and no longer needed

### pkg/emb6 [4f243c52eabefe709d78560ce7f1d502d737a999]

Author(s):

- Martine S. Lenders <m.lenders@fu-berlin.de>

Reason for removal:

- Stale upstream (last update to date in Mar 2018)
- Ported version even older (last update Feb 2016)
- Updating to more recent version would be more effort than its worth

### pkg/libcoap [d83d08f0995a88f399e70a7d07b44dd780082436]

Author:

- Martine Lenders <mlenders@inf.fu-berlin.de>

Reasons for removal:

- Not maintained anymore, no updates for 8 years
- Upstream libcoap allows building with RIOT, so there is a migration path for users

### pkg/nordic_softdevice_ble [35b6ccedf31f10a5f8e4f97609ad5b10c28bdc34]

Author(s):

- Hauke Petersen <hauke.petersen@fu-berlin.de>
- Kaspar Schleiser <kaspar@schleiser.de>
- Martine Lenders <m.lenders@fu-berlin.de>
- Wojciech Bober <wojciech.bober@nordicsemi.no>

Reason for removal:

- Outdated, unmaintained and no longer working

### pkg/oonf_api [c829f820ee870bffc60c7df688d2da8373553212]

Author:

- Benjamin Valentin <benpicco@googlemail.com>

Reasons for removal:

- Package has not been update since 2013.
- Tests were never integrated to the CI.
- NHDP, which was the only upstream user, has been removed.

### pkg/qdsa [cc357f9638ce22a39eab8f79055f1154388ccb74]

Author(s):

- Kaspar Schleiser <kaspar@schleiser.de> (integration into RIOT as package)
- Joost Renes <joost.renes@nxp.com> (upstream code)

Reason for removal:

- No users besides the test
- The qDSA signature does not seem to have received the level of attention and
  scrutiny by the crypto community that would justify inclusion in a general
  purpose OS

### sys/irq_handler [a2dd6f90e51ca9edef643ba72bd1fd18113cf0d2]

Author(s):

- Gunar Schorcht <gunar@schorcht.net>

Reason for removal:

- Provides some feature as `sys/event_thread`
- Has been deprecated directly in the first release it was provided

### sys/net/gnrc/application_layer/tftp [0e2a62078850e1ecc74db2db4d639cf2d8fb96d3]

Author:

- DipSwitch <dipswitch@ownage4u.nl>

Reasons for removal:

- Not maintained anymore.
- Existing quality and security defects.

### sys/net/gnrc/nettest [cdc252ab7bd4161cc046bf93a3e55995704b24d4]

Author:

- Martine Lenders <mlenders@inf.fu-berlin.de>

Reasons for removal:

- Not maintained anymore
- Not used in GNRC

### sys/net/routing/nhdp [9026823bb906f64168d7d38e52be92f390353dc8]

Author:

- Fabian Nack <nack@inf.fu-berlin.de>

Reasons for removal:

- Not maintained anymore.
- Does not comply with the current RIOT programming practices.

### sys/ubjson [bea30c3f8949ebd9fdf4a9bf0a987652889930f5]

Author(s):

- René Kijewski <rene.kijewski@fu-berlin.de>

Reasons for removal:

- Unsafe code.
- Did not comply with RIOT standards.
- More standard alternatives available (such as CBOR).

[6cad5d24771ba6199228351a11b5062cd2e9b36d]: https://github.com/RIOT-OS/RIOT/commit/6cad5d24771ba6199228351a11b5062cd2e9b36d
[d83d08f0995a88f399e70a7d07b44dd780082436]: https://github.com/RIOT-OS/RIOT/commit/d83d08f0995a88f399e70a7d07b44dd780082436
[cdc252ab7bd4161cc046bf93a3e55995704b24d4]: https://github.com/RIOT-OS/RIOT/commit/cdc252ab7bd4161cc046bf93a3e55995704b24d4
[ed3887ac5c1e95308c2827bce3cdca8b0f146c22]: https://github.com/RIOT-OS/RIOT/commit/ed3887ac5c1e95308c2827bce3cdca8b0f146c22
[0e2a62078850e1ecc74db2db4d639cf2d8fb96d3]: https://github.com/RIOT-OS/RIOT/commit/0e2a62078850e1ecc74db2db4d639cf2d8fb96d3
[c829f820ee870bffc60c7df688d2da8373553212]: https://github.com/RIOT-OS/RIOT/commit/c829f820ee870bffc60c7df688d2da8373553212
[9026823bb906f64168d7d38e52be92f390353dc8]: https://github.com/RIOT-OS/RIOT/commit/9026823bb906f64168d7d38e52be92f390353dc8
[bea30c3f8949ebd9fdf4a9bf0a987652889930f5]: https://github.com/RIOT-OS/RIOT/commit/bea30c3f8949ebd9fdf4a9bf0a987652889930f5
[232aed3e18118624b862d36bfec7cd1c21ca2d26]: https://github.com/RIOT-OS/RIOT/commit/232aed3e18118624b862d36bfec7cd1c21ca2d26
[a2bcd7539ce1931b7aec0077ea71dadd62c96edd]: https://github.com/RIOT-OS/RIOT/commit/a2bcd7539ce1931b7aec0077ea71dadd62c96edd
[ee6b6b9c388b78fcec7ba6e239a6c76041b9bbb7]: https://github.com/RIOT-OS/RIOT/commit/ee6b6b9c388b78fcec7ba6e239a6c76041b9bbb7
[9447cb303426d7c6348bb84999f88bf929cd6263]: https://github.com/RIOT-OS/RIOT/commit/9447cb303426d7c6348bb84999f88bf929cd6263
[ea73cc49eacd45640b9660134c4c92b887cb2716]: https://github.com/RIOT-OS/RIOT/commit/ea73cc49eacd45640b9660134c4c92b887cb2716
[99009af25e201bbc182d376e99df34133417be6c]: https://github.com/RIOT-OS/RIOT/commit/99009af25e201bbc182d376e99df34133417be6c
[cfa9580f319508f858c8fe30ecce8b2b59b6caa3]: https://github.com/RIOT-OS/RIOT/commit/cfa9580f319508f858c8fe30ecce8b2b59b6caa3
[b3eb9b8cf23f0702fa725d536df6ad6528f84189]: https://github.com/RIOT-OS/RIOT/commit/b3eb9b8cf23f0702fa725d536df6ad6528f84189
[7bc271807cecbffbb01a37c56a367b98fb823573]: https://github.com/RIOT-OS/RIOT/commit/7bc271807cecbffbb01a37c56a367b98fb823573
[fc45d7c5ac3d46bfdfb21c243cd3d4881bc5c836]: https://github.com/RIOT-OS/RIOT/commit/fc45d7c5ac3d46bfdfb21c243cd3d4881bc5c836
[b83430aa625a1d42f11f9badf5e5cfbb8efacd99]: https://github.com/RIOT-OS/RIOT/commit/b83430aa625a1d42f11f9badf5e5cfbb8efacd99
[f671a87fe2c539c3aecd595ae03fa4f6f209d042]: https://github.com/RIOT-OS/RIOT/commit/f671a87fe2c539c3aecd595ae03fa4f6f209d042
[f2760c033c5f332be076b25aa212aca4007c3d65]: https://github.com/RIOT-OS/RIOT/commit/f2760c033c5f332be076b25aa212aca4007c3d65
[e63cd54f3b1e002a7895bb7c46af889b341c1a15]: https://github.com/RIOT-OS/RIOT/commit/e63cd54f3b1e002a7895bb7c46af889b341c1a15
[4f243c52eabefe709d78560ce7f1d502d737a999]: https://github.com/RIOT-OS/RIOT/commit/4f243c52eabefe709d78560ce7f1d502d737a999
[9fb2f541baca469e34fa01b004d6f19385700ce9]: https://github.com/RIOT-OS/RIOT/commit/9fb2f541baca469e34fa01b004d6f19385700ce9
[35b6ccedf31f10a5f8e4f97609ad5b10c28bdc34]: https://github.com/RIOT-OS/RIOT/commit/35b6ccedf31f10a5f8e4f97609ad5b10c28bdc34
[72821a502f073006643cb4ef7815fc8c42563ce6]: https://github.com/RIOT-OS/RIOT/commit/72821a502f073006643cb4ef7815fc8c42563ce6
[a2dd6f90e51ca9edef643ba72bd1fd18113cf0d2]: https://github.com/RIOT-OS/RIOT/commit/a2dd6f90e51ca9edef643ba72bd1fd18113cf0d2
[cab1ea66b49e3c72e2827c8686c09d84ae3ef9a9]: https://github.com/RIOT-OS/RIOT/commit/cab1ea66b49e3c72e2827c8686c09d84ae3ef9a9
[fe941ac9fe3f81c0f08ff3b8564cf439639abcda]: https://github.com/RIOT-OS/RIOT/commit/fe941ac9fe3f81c0f08ff3b8564cf439639abcda
[81458c8eed8949c686d5ded652dbee10748e860b]: https://github.com/RIOT-OS/RIOT/commit/81458c8eed8949c686d5ded652dbee10748e860b
[2b8a0d48940517f7df4e78c7a0b16024f46a8694]: https://github.com/RIOT-OS/RIOT/commit/2b8a0d48940517f7df4e78c7a0b16024f46a8694
[4953ba8e6759d2b1a1a1ea497a4ad1e71489195a]: https://github.com/RIOT-OS/RIOT/commit/4953ba8e6759d2b1a1a1ea497a4ad1e71489195a
[cc357f9638ce22a39eab8f79055f1154388ccb74]: https://github.com/RIOT-OS/RIOT/commit/cc357f9638ce22a39eab8f79055f1154388ccb74
