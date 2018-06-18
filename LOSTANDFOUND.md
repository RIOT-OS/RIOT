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


## Removed Features

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
