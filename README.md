[![Nightly CI status master][master-ci-badge]][master-ci-link]
[![IRC][irc-badge]][irc-link]

                          ZZZZZZ
                        ZZZZZZZZZZZZ
                      ZZZZZZZZZZZZZZZZ
                     ZZZZZZZ     ZZZZZZ
                    ZZZZZZ        ZZZZZ
                    ZZZZZ          ZZZZ
                    ZZZZ           ZZZZZ
                    ZZZZ           ZZZZ
                    ZZZZ          ZZZZZ
                    ZZZZ        ZZZZZZ
                    ZZZZ     ZZZZZZZZ       777        7777       7777777777
              ZZ    ZZZZ   ZZZZZZZZ         777      77777777    77777777777
          ZZZZZZZ   ZZZZ  ZZZZZZZ           777     7777  7777       777
        ZZZZZZZZZ   ZZZZ    Z               777     777    777       777
       ZZZZZZ       ZZZZ                    777     777    777       777
      ZZZZZ         ZZZZ                    777     777    777       777
     ZZZZZ          ZZZZZ    ZZZZ           777     777    777       777
     ZZZZ           ZZZZZ    ZZZZZ          777     777    777       777
     ZZZZ           ZZZZZ     ZZZZZ         777     777    777       777
     ZZZZ           ZZZZ       ZZZZZ        777     777    777       777
     ZZZZZ         ZZZZZ        ZZZZZ       777     777    777       777
      ZZZZZZ     ZZZZZZ          ZZZZZ      777     7777777777       777
       ZZZZZZZZZZZZZZZ            ZZZZ      777      77777777        777
         ZZZZZZZZZZZ               Z
            ZZZZZ

The friendly Operating System for IoT!

RIOT is a real-time multi-threading operating system that supports a range of
devices that are typically found in the Internet of Things (IoT):
8-bit, 16-bit and 32-bit microcontrollers.

RIOT is based on the following design principles: energy-efficiency, real-time
capabilities, small memory footprint, modularity, and uniform API access,
independent of the underlying hardware (this API offers partial POSIX
compliance).

RIOT is developed by an international open source community which is
independent of specific vendors (e.g. similarly to the Linux community).
RIOT is licensed with LGPLv2.1, a copyleft license which fosters
indirect business models around the free open-source software platform
provided by RIOT, e.g. it is possible to link closed-source code with the
LGPL code.

## FEATURES

RIOT is based on a microkernel architecture, and provides features including,
but not limited to:

* a preemptive, tickless scheduler with priorities
* flexible memory management
* high resolution, long-term timers
* support 100+ boards based on AVR, MSP430, MIPS, ARM7 and ARM Cortex-M
* the native port allows to run RIOT as-is on Linux, BSD, and MacOS. Multiple
  instances of RIOT running on a single machine can also be interconnected via
  a simple virtual Ethernet bridge
* IPv6
* 6LoWPAN (RFC4944, RFC6282, and RFC6775)
* UDP
* RPL (storing mode, P2P mode)
* CoAP
* CCN-Lite
* Sigfox
* LoRaWAN (Experimental)


## GETTING STARTED
* You want to start the RIOT? Just follow our
[quickstart guide](http://doc.riot-os.org/index.html#the-quickest-start) or
try this
[tutorial](https://github.com/RIOT-OS/Tutorials/blob/master/README.md).
For specific toolchain installation, follow instructions in the
[getting started](http://doc.riot-os.org/getting-started.html) page.
* The RIOT API itself can be built from the code using doxygen. The latest
  version of the documentation is uploaded daily to
  [riot-os.org/api](http://riot-os.org/api).

### USING THE NATIVE PORT WITH NETWORKING
If you compile RIOT for the native cpu and include the `netdev_tap` module,
you can specify a network interface like this: `PORT=tap0 make term`

#### SETTING UP A TAP NETWORK
There is a shell script in `RIOT/dist/tools/tapsetup` called `tapsetup` which
you can use to create a network of tap interfaces.

*USAGE*

To create a bridge and two (or `count` at your option) tap interfaces:

    ./dist/tools/tapsetup/tapsetup [-c [<count>]]

## CONTRIBUTE

To contribute something to RIOT, please refer to the [development
procedures](https://github.com/RIOT-OS/RIOT/wiki/Development-procedures) and
read all notes for best practice.

## MAILING LISTS
* RIOT OS kernel developers list
 * devel@riot-os.org (http://lists.riot-os.org/mailman/listinfo/devel)
* RIOT OS users list
 * users@riot-os.org (http://lists.riot-os.org/mailman/listinfo/users)
* RIOT commits
 * commits@riot-os.org (http://lists.riot-os.org/mailman/listinfo/commits)
* Github notifications
 * notifications@riot-os.org
   (http://lists.riot-os.org/mailman/listinfo/notifications)

## LICENSE
* Most of the code developed by the RIOT community is licensed under the GNU
  Lesser General Public License (LGPL) version 2.1 as published by the Free
  Software Foundation.
* Some external sources, especially files developed by SICS are published under
  a separate license.

All code files contain licensing information.

For more information, see the RIOT website:

http://www.riot-os.org


[master-ci-badge]: https://ci.riot-os.org/RIOT-OS/RIOT/master/latest/badge.svg
[master-ci-link]: https://ci.riot-os.org/RIOT-OS/RIOT/master/latest/output.html
[irc-badge]: https://img.shields.io/badge/IRC-join%20chat%20%E2%86%92-blue.svg
[irc-link]: http://webchat.freenode.net?channels=%23riot-os
