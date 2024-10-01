from fido2.hid import CtapHidDevice
from fido2.ctap2 import CTAP2


def get_device():
    devs = list(CtapHidDevice.list_devices())
    assert len(devs) == 1
    return devs[0]


if __name__ == '__main__':
    try:
        dev = get_device()
    except Exception:
        print("Unable to find authenticator")
        exit(-1)

    ctap = CTAP2(dev)
    try:
        ctap.reset()
        print("Device successfully reset")
    except Exception as e:
        print(e)
