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

~~~~~~~~~~~~~~~~~~~ {.md}
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
~~~~~~~~~~~~~~~~~~~

By putting the name of the removed feature and the commit hash in the same line,
one can very quickly find the commit using `git grep`.

Listing the authors of the removed code is done for appreciation of their work.
This way, their names are never removed from the RIOT repository.


# Removed Features

### sys/net/gnrc/application_layer/tftp [0e2a62078850e1ecc74db2db4d639cf2d8fb96d3]
Author:
- DipSwitch <dipswitch@ownage4u.nl>

Reasons for removal:
- Not maintained anymore.
- Existing quality and security defects.

### pkg/oonf_api [c829f820ee870bffc60c7df688d2da8373553212]
Author:
- Benjamin Valentin <benpicco@googlemail.com>

Reasons for removal:
- Package has not been update since 2013.
- Tests were never integrated to the CI.
- NHDP, which was the only upstream user, has been removed.

### sys/net/routing/nhdp [0201a2d273fe17aa1b8eeb22776ae9670551433d]
Author:
- Fabian Nack <nack@inf.fu-berlin.de>

Reasons for removal:
- Not maintained anymore.
- Does not comply with the current RIOT programming practices.

### sys/ubjson [868d37708bb674739ab4983441d6df904dd5a25f]
Author(s):
- René Kijewski <rene.kijewski@fu-berlin.de>

Reasons for removal:
- Unsafe code.
- Did not comply with RIOT standards.
- More standard alternatives available (such as CBOR).

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

### cpu/mips32r2_generic [a2bcd7539ce1931b7aec0077ea71dadd62c96edd]
Author:
- Neil Jones <neil.jones@imgtec.com>

- No boards use this CPU (the only one was mips-malta).
- (Same reasons as mips-malta)

### boards/mips-malta [ee6b6b9c388b78fcec7ba6e239a6c76041b9bbb7]
Author:
- Neil Jones <neil.jones@imgtec.com>

Reasons for removal:
- UART input not supported.
- Hardware not available for testing and not available for purchase either.
- Not actively maintained / broken for some time.

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

### boards/weio [cfa9580f319508f858c8fe30ecce8b2b59b6caa3]
Author(s):
- Paul Rathgeb <paul.rathgeb@skynet.be>

Reason for removal:
- hardware not available to the community for testing
- original author and maintainer won't be able to maintain the code

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

### cpu/x86 [fc45d7c5ac3d46bfdfb21c243cd3d4881bc5c836]
Author(s):
- René Kijewski <rene.kijewski@fu-berlin.de>

Reason for removal:
- code broken and excluded from all tests for a long time
- no maintainer available

### gnrc_pktbuf_duplicate_upto() [b83430aa625a1d42f11f9badf5e5cfbb8efacd99]
Author(s):
- Takuo Yonezawa <yonezawa.t2@gmail.com>
- Martine S. Lenders <m.lenders@fu-berlin.de>

Reason for removal:
- broke the abstraction of `gnrc_pktbuf`
- its only user within the RIOT code base `gnrc_ipv6_ext` was reworked in
  f671a87fe2c539c3aecd595ae03fa4f6f209d042 so it is not needed anymore.
- the function was deprecated in f2760c033c5f332be076b25aa212aca4007c3d65
