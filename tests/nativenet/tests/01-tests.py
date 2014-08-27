#! /usr/bin/env python

import sys
from pexpect import spawn

if __name__ == "__main__":
    try:
        if len(sys.argv) > 1:
            expected_received = int(sys.argv[1])
        else:
            expected_received = 5
    except TypeError:
        sys.stderr.write("Usage: "+str(sys.argv[0])+" [<expected 'received'>]\n")
        sys.exit(1)

    receiver = spawn("bin/native/nativenet.elf tap0")
    sender = spawn("bin/native/nativenet_sender.elf tap1")


    receiver.expect(r"main\(\): initializing transceiver")
    receiver.expect(r"main\(\): starting transceiver")
    receiver.expect(r"main\(\): starting radio thread")
    receiver.expect("Start receiving")
    receiver.expect(r"\[nativenet\] trying to set address \d+")

    sender.expect(r"main\(\): initializing transceiver")
    sender.expect(r"main\(\): starting transceiver")
    sender.expect(r"\[nativenet\] trying to set address \d+")
    sender.expect("Start sending packets")

    while expected_received > 0:
        receiver.expect("received")
        expected_received -= 1

    if not sender.terminate():
        sender.terminate(force=True)
    if not receiver.terminate():
        receiver.terminate(force=True)
