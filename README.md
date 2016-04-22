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

## FEATURES
RIOT OS is an operating system for Internet of Things (IoT) devices. It is based on a microkernel and designed for
* energy efficiency
* hardware independent development
* a high degree of modularity

Its features comprise
* a preemptive, tickless scheduler with priorities
* flexible memory management
* high resolution, long-term timers
* support for AVR, MSP430, ARM7, and ARM Cortex-M on over 50 boards
* the native port allows to run RIOT as-is on Linux, BSD, and MacOS. Multiple instances of RIOT running on a single machine can also be interconnected via a simple virtual Ethernet bridge
* IPv6
* 6LoWPAN (RFC4944, RFC6282, and RFC6775)
* UDP
* RPL (storing mode)
* CoAP
* CCN-Lite

## GETTING STARTED
* You want to start the RIOT? Just follow our [Getting started documentation](https://github.com/RIOT-OS/RIOT/wiki/Introduction)
* The RIOT API itself can be built from the code using doxygen. The latest version is uploaded daily to http://riot-os.org/api.

### USING THE NATIVE PORT WITH NETWORKING
If you compile RIOT for the native cpu and include the `netdev2_tap` module, you can specify a network interface like this: `PORT=tap0 make term`

#### SETTING UP A TAP NETWORK
There is a shellscript in `RIOT/dist/tools/tapsetup` called `tapsetup` which you can use to create a network of tap interfaces.

*USAGE*
To create a bridge and two (or count at your option) tap interfaces:

    ./dist/tools/tapsetup/tapsetup [-c [<count>]]

## CONTRIBUTE

To contribute something to RIOT, please refer to the [development procedures](https://github.com/RIOT-OS/RIOT/wiki/Development-procedures) and read all notes for best practice.

## MAILING LISTS
* RIOT OS kernel developers list
 * devel@riot-os.org (http://lists.riot-os.org/mailman/listinfo/devel)
* RIOT OS users list
 * users@riot-os.org (http://lists.riot-os.org/mailman/listinfo/users)
* RIOT commits
 * commits@riot-os.org (http://lists.riot-os.org/mailman/listinfo/commits)
* Github notifications
 * notifications@riot-os.org  (http://lists.riot-os.org/mailman/listinfo/notifications)

## LICENSE
* All sources and binaries that have been developed at Freie Universität Berlin are
  licensed under the GNU Lesser General Public License version 2.1 as published by the
  Free Software Foundation.
* Some external sources, especially files developed by SICS are published under
  a separate license.

All code files contain licensing information.

For more information, see the RIOT website:

http://www.riot-os.org
