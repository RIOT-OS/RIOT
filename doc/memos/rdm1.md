RIOT Maintainers Community (RIOT-MC)
Developer Memo: 1
Author(s): Hauke Petersen, more here
Date: December 2016

***
RIOT Developer Memo Format, Publishing and Maintenance Process
***

# Abstract

This memo contains a glossary with expressions commonly used in RIOT and in the
context of RIOT. The aim of this document is not only to put some names to often
used abbreviations, but also to explain their interpretation withing the RIOT
community.


# Status

This document is a product of the community of RIOT maintainers, and aims to
represent the consensus within this community.
The content of this document is licensed with a Creative Commons CC-BY-SA
license.

# Terminology

See below.


# 1. Introduction

As with many open source communities, there are a number of expressions, that
are being commonly used among community members and community documents. Though
most of these expressions are well known among developers, some of them are
specific or are used with not-so-common semantic in the context of RIOT.

This document provides a list of the most commonly used abbreviations and
bash-words in RIOT. The scope of this document is not only to map these
expressions to some more meaningful names, but also to explain their
interpretation inside the RIOT community and to give some links to further
information.

Another aim of this RDM is further to act as a referencing target for future any
RMDs.


# 2. RIOT Glossary

**ACK:** Acknowledgment of a pull request (PR) from a RIOT maintainer.

**IoT:** Internet of Things - the thing everyone talks about theses days. In the
context of RIOT we consider the IoT to be about a huge number of constrained,
interconnected devices, many operated on small power budgets (e.g. battery
driven).

**PR:** Git merge request on github. If you want to get code into RIOT's master
branch, opening a PR is the only way.

**RDM:** RIOT Developer Memo - these are documents like this one, containing
typically more high-level documentation in addition to doxygen and the wiki. See
[RDM0](TODO)

**RIOT:** The name of the operating system this is all about. NOTE: it is `RIOT`,
not `RIoT`, not `Riot`, not `riot`, and certainly not `rIoT`.

**RIOT-MC:** RIOT Maintainers Community

**Squash(ing):** When working with `git`, it is possible to change the ordering
and names of commits. Before merging code into RIOT's master, developers are
often asked to squash their commits before a PR is merged, meaning that all the
changes should be put in a small number of commits with clear names and touching
one logical aspect each.

TODO:
- Commit
- git
- merge
- rebase
- CPU
- MCU
- Board
- Device
- Flash(ing)
- Debugg(ing)
- JTAG
- SWD
- OpenOCD
- pyterm
- Network stack
- 6LoWPAN
- CoAP
- IETF
- Border router
- Gateway
- Module
- core
- sys
- devel: mailing list
- users: mailing list
- IEEE802.15.4
- BLE
- Ethos
- many many more...

# Acknowledgements

TODO


# References
- [RDM0](TODO)
- Internet Engineering Task Force ([IETF](http://ietf.org/))
