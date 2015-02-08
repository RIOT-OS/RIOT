#!/usr/bin/env python

# Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os, signal, sys, time
from pexpect import spawn, TIMEOUT, EOF

class Abort(Exception):
    pass

board = os.environ.get('BOARD', 'native')

DEFAULT_TIMEOUT=5

def main():
    receiver = None
    sender = None

    if "PORT" in os.environ:
        del os.environ["PORT"]

    if "TERMFLAGS" in os.environ:
        del os.environ["TERMFLAGS"]

    try:
        os.environ["SENDER"] = "0"
        receiver = spawn("make term", timeout=5)
        time.sleep(1)
        os.environ["SENDER"] = "1"
        sender = spawn("make term", timeout=5)

        receiver.expect(r"RIOT netdev test")
        sender.expect(r"RIOT netdev test")

        i = receiver.expect([r"Default netdev type and driver unknown!",
                             r"Initialized dev "])
        if i == 0:
            print('No netdev driver for board %s. Aborting test.' % board)
            raise Abort
        elif i == 1:
            pass

        i = sender.expect([r"Default netdev type and driver unknown!",
                           r"Initialized dev "])
        if i == 0:
            print('No netdev driver for board %s. Aborting test.' % board)
            raise Abort
        elif i == 1:
            pass

        receiver.expect([r"Channel was set to [0-9]+",
                         r"Does not support setting the channel"])
        receiver.expect([r"Got channel of value [0-9]+ of length [0-9]+",
                         r"Does not support getting the channel"])
        sender.expect([r"Channel was set to [0-9]+",
                       r"Does not support setting the channel"])
        sender.expect([r"Got channel of value [0-9]+ of length [0-9]+",
                       r"Does not support getting the channel"])

        receiver.expect([r"Address was set to [0-9]+",
                         r"Does not support setting the address"])
        receiver.expect([r"Got address of value [0-9]+ of length [0-9]+",
                         r"Got address of length [0-9]+ of value: ([0-9a-f]{2} )+",
                         r"Does not support getting the address"])
        sender.expect([r"Address was set to [0-9]+",
                       r"Does not support setting the address"])
        sender.expect([r"Got address of value [0-9]+ of length [0-9]+",
                       r"Got address of length [0-9]+ of value: ([0-9a-f]{2} )+",
                       r"Does not support getting the address"])

        receiver.expect([r"Long address was set to [0-9]+",
                         r"Does not support setting the long address"])
        receiver.expect([r"Got long address of value [0-9]+ of length [0-9]+",
                         r"Got long address of value 0x[0-9a-f]+ of length 8",
                         r"Got long address of length [0-9]+ of value: ([0-9a-f]{2} )+",
                         r"Does not support getting the long address"])
        sender.expect([r"Long address was set to [0-9]+",
                       r"Does not support setting the long address"])
        sender.expect([r"Got long address of value [0-9]+ of length [0-9]+",
                       r"Got long address of value 0x[0-9a-f]+ of length 8",
                       r"Got long address of length [0-9]+ of value: ([0-9a-f]{2} )+",
                       r"Does not support getting the long address"])

        receiver.expect([r"Network ID was set to [0-9]+",
                         r"Does not support setting the network ID"])
        receiver.expect([r"Got network ID of value [0-9]+ of length [0-9]+",
                         r"Does not support getting the network ID"])
        sender.expect([r"Network ID was set to [0-9]+",
                       r"Does not support setting the network ID"])
        sender.expect([r"Got network ID of value [0-9]+ of length [0-9]+",
                       r"Does not support getting the network ID"])

        receiver.expect([r"Got maximum packet size of value [0-9]+ of length [0-9]+",
                         r"Does not support getting the maximum packet size"])
        sender.expect([r"Got maximum packet size of value [0-9]+ of length [0-9]+",
                       r"Does not support getting the maximum packet size"])

        receiver.expect([r"Got protocol: ",
                         r"Does not support getting the protocol"])
        sender.expect([r"Got protocol: ",
                       r"Does not support getting the protocol"])

        i = receiver.expect(["Device was set to RX mode",
                             "Device does not have a RX mode"])

        if i == 0:
            receiver.expect("Device is in RX mode")
        elif i == 1:
            receiver.expect("Device is")

        i = sender.expect(["Device was set to RX mode",
                           "Device does not have a RX mode"])

        if i == 0:
            sender.expect("Device is in RX mode")
        elif i == 1:
            sender.expect("Device is")

        receiver.expect(r"Added receive callback successfully")

        sender.expect(r'Sending "header 1,header 2,payload" to [0-9a-f ]+')
        receiver.expect(r'Received "header 1,header 2,payload"')
        receiver.expect(r"Sender was [0-9a-f ]+")

        return 0
    except TIMEOUT as exc:
        print(exc)
        return 1
    except Abort:
        return 0
    finally:
        if sender and not sender.terminate():
            os.killpg(sender.pid, signal.SIGKILL)
        if receiver and not receiver.terminate():
            os.killpg(receiver.pid, signal.SIGKILL)

if __name__ == "__main__":
    sys.exit(main())
