#!/usr/bin/python3
"""
Command line utility to list and filter TTYs
"""
import argparse
import json
import os
import sys
import time

import pyudev


def unescape(string):
    """
    Decodes unicode escaping in a string, e.g. "Hallo\\x20World" is decoded as
    "Hallo World"
    """
    res = bytes(string, "utf8").decode("unicode_escape")
    res = res.encode("latin1").decode("utf8", errors="replace")
    return res


def tty2dict(dev):
    """
    Parse the given TTY udev interface into a dict() containing the most
    relevant attributes
    """
    result = {}
    result["path"] = dev.get("DEVNAME")
    result["ctime"] = os.stat(result["path"]).st_ctime
    result["serial"] = dev.get("ID_SERIAL_SHORT")
    result["driver"] = dev.get("ID_USB_DRIVER")
    result["model"] = unescape(dev.get("ID_MODEL_ENC"))
    result["model_db"] = dev.get("ID_MODEL_FROM_DATABASE")
    result["vendor"] = unescape(dev.get("ID_VENDOR_ENC"))
    result["vendor_db"] = dev.get("ID_VENDOR_FROM_DATABASE")

    return result


def filters_match(filters, tty):
    """
    Check if the given TTY interface matches all given filters
    """
    if filters.serial is not None:
        if filters.serial != tty["serial"]:
            return False

    if filters.driver is not None:
        if filters.driver != tty["driver"]:
            return False

    if filters.model is not None:
        if filters.model != tty["model"]:
            return False

    if filters.model_db is not None:
        if filters.model_db != tty["model_db"]:
            return False

    if filters.vendor is not None:
        if filters.vendor != tty["vendor"]:
            return False

    if filters.vendor_db is not None:
        if filters.vendor != tty["vendor_db"]:
            return False

    for forbidden_serial in filters.exclude_serial:
        if tty["serial"] == forbidden_serial:
            return False

    return True


def shorten(string, length):
    """
    Shorten the given string to the given length, if needed
    """
    if len(string) > length:
        return string[:length - 3] + "..."

    return string


def parse_args(args):
    """
    Parse the given command line style arguments with argparse
    """
    desc = "List and filter TTY interfaces that might belong to boards"
    supported_formats = {
        "table",
        "json",
        "path",
        "serial",
        "vendor",
        "vendor_db",
        "model",
        "model_db",
        "driver",
        "ctime",
    }
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument("--most-recent", action="store_true",
                        help="Print only the most recently connected matching "
                             + "TTY")
    parser.add_argument("--format", default="table", type=str,
                        help=f"How to format the TTYs. Supported formats: "
                             f"{sorted(supported_formats)}")
    parser.add_argument("--serial", default=None, type=str,
                        help="Print only devices matching this serial")
    parser.add_argument("--driver", default=None, type=str,
                        help="Print only devices using this driver")
    parser.add_argument("--model", default=None, type=str,
                        help="Print only devices matching this model "
                             "(as reported from device)")
    parser.add_argument("--model-db", default=None, type=str,
                        help="Print only devices matching this model "
                             "(DB entry)")
    parser.add_argument("--vendor", default=None, type=str,
                        help="Print only devices matching this vendor "
                             "(as reported from device)")
    parser.add_argument("--vendor-db", default=None, type=str,
                        help="Print only devices matching this vendor "
                             "(DB entry)")
    parser.add_argument("--exclude-serial", type=str, nargs='*', default=None,
                        help="Ignore devices with these serial numbers. "
                             + "Environment variable EXCLUDE_TTY_SERIAL can "
                             + "be used alternatively.")

    args = parser.parse_args()

    if args.format not in supported_formats:
        sys.exit(f"Format \"{args.format}\" not supported")

    if args.exclude_serial is None:
        if "EXCLUDE_TTY_SERIAL" in os.environ:
            args.exclude_serial = os.environ["EXCLUDE_TTY_SERIAL"].split()
        else:
            args.exclude_serial = []

    return args


def print_table(data, headers):
    """
    Print the list of dictionaries given in data as table, where headers is
    a list of keys to that dict and also servers as table headers.
    """
    lengths = []
    for header in headers:
        lengths.append(len(header))

    for item in data:
        for i, header in enumerate(headers):
            if len(str(item[header])) > lengths[i]:
                lengths[i] = len(item[header])

    sys.stdout.write(f"{headers[0]:{lengths[0]}}")
    for i in range(1, len(headers)):
        sys.stdout.write(f" | {headers[i]:{lengths[i]}}")
    sys.stdout.write("\n" + lengths[0] * "-")
    for i in range(1, len(headers)):
        sys.stdout.write("-|-" + lengths[i] * "-")

    for item in data:
        sys.stdout.write(f"\n{str(item[headers[0]]):{lengths[0]}}")
        for header, length in zip(headers[1:], lengths[1:]):
            sys.stdout.write(f" | {str(item[header]):{length}}")

    sys.stdout.write("\n")
    sys.stdout.flush()


def print_results(args, ttys):
    """
    Print the given TTY devices according to the given args
    """
    if args.format == "json":
        print(json.dumps(ttys, indent=2))
        return

    if args.format == "table":
        for tty in ttys:
            tty["ctime"] = time.strftime("%H:%M:%S",
                                         time.localtime(tty["ctime"]))
        headers = ["path", "driver", "vendor", "model", "model_db", "serial",
                   "ctime"]
        print_table(ttys, headers)
        return

    for tty in ttys:
        print(tty[args.format])


def print_ttys(args):
    """
    Print ttys as specified by the given command line arguments
    """
    args = parse_args(args)

    ttys = []
    for dev in pyudev.Context().list_devices(subsystem='tty', ID_BUS='usb'):
        tty = tty2dict(dev)
        if filters_match(args, tty):
            ttys.append(tty)

    if args.most_recent:
        most_recent = ttys[0]
        for tty in ttys:
            if tty["ctime"] > most_recent["ctime"]:
                most_recent = tty
        ttys = [most_recent]

    print_results(args, ttys)


if __name__ == "__main__":
    print_ttys(sys.argv)
