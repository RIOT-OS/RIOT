#!/usr/bin/python
# Enable Python 3 style division in case this script is running via Python 2
from __future__ import division
import argparse
import re
import sys
import subprocess
import os.path

# Python 2 does not have subprocess.DEVNULL, so a little manual work is required
# if the script is run via Python 2
try:
    from subprocess import DEVNULL
except ImportError:
    # Python 2
    import os
    DEVNULL = open(os.devnull, "wb")

SIZE_REGEX = re.compile(r"^\s*([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)")
RAM_WARN_THRESHOLD = 0.9
ROM_WARN_THRESHOLD = 0.95


class Color:
    """ANSI escape sequences to colorize terminal output"""
    RESET = '\033[0m'
    TEXT = '\033[94m'
    DATA = '\033[31m'
    BSS = '\033[32m'
    OFFSET = '\033[93m'
    RED = '\033[31m'
    BOLD = '\033[1m'


def sanitize_size_argument(arg):
    if arg is None:
        return None
    if (len(arg) > 0) and (arg[-1] == 'K'):
        return 1024 * int(arg[:-1], 0)
    elif (len(arg) > 0) and (arg[-1] == 'M'):
        return 1024 * 1024 * int(arg[:-1], 0)
    return int(arg, 0)


def parse_arguments():
    """Parses the command line arguments"""
    parser = argparse.ArgumentParser(description="Eye candy for size")
    parser.add_argument(
            "--rom", "-f", default=None, type=str, metavar="ROM",
            help="amount of rom on the device in Bytes")
    parser.add_argument(
            "--ram", "-r", default=None, type=str, metavar="RAM",
            help="amount of RAM available in Bytes")
    parser.add_argument(
            "--bootloader_size", "-o", default="0", type=str, metavar="OFF",
            help="ROM Bytes reserved for the bootloader")
    parser.add_argument(
            "--cmd", "-c", required=True, type=str, metavar="CMD",
            help="path to binutils' size utility")
    parser.add_argument(
            "file", type=str, metavar="FILE",
            help="path to the file to examine")
    result = parser.parse_args()
    result.rom = sanitize_size_argument(result.rom)
    result.ram = sanitize_size_argument(result.ram)
    result.bootloader_size = sanitize_size_argument(result.bootloader_size)
    return result


def parse_size(args):
    """Runs binutils' size and returns the size of the sections as dict
    :param args: The arguments as returned form parse_args, which contain the
                 path to binutils' size and the path to the file to examine
    """
    proc = subprocess.Popen([args.cmd, "-d", "-B", args.file],
                            stdout=subprocess.PIPE, stderr=DEVNULL)
    line = proc.stdout.readlines()[1]
    match = SIZE_REGEX.match(line.decode("utf-8"))
    text = int(match.group(1))
    data = int(match.group(2))
    bss = int(match.group(3))
    return {
            ".text": text,
            ".data": data,
            ".bss": bss}


def print_bar(items):
    """Prints a bar representing RAM/rom usage as horizontal stacked bar
    :param items: A list of tuples, one tuple per entry. A tuple consists of
                  (label, fraction, color)
    """
    WIDTH = 78
    bar = "["
    legend = "Legend:"
    rest = WIDTH
    for label, fraction, color in items:
        count = int(fraction * WIDTH)
        rest -= count
        bar += color + "#" * count
        legend += " " + color + "#" + Color.RESET + " " + label
    bar += Color.RESET + "-" * rest + "]"
    print(bar)
    print(legend)


def print_size(args, size):
    """Prints the size of the section and (if possible) stacked bar charts for
    the estimated RAM/rom usage
    :param args: The command line arguments as returned by parse_args
    :param size: The sizes of each sections as dict (see parse_size)
    """
    overflown = False

    if (args.ram):
        ram_used = size[".data"] + size[".bss"]
        ram_usage = ram_used / args.ram
        print("Estimated RAM usage: {:.1f} KiB / {:.1f} KiB ({:.1%})".format(
                ram_used / 1024, args.ram / 1024, ram_usage))
        if (ram_used > args.ram):
            print("RAM overflown by {} Bytes".format(ram_used - args.ram))
            overflown = True
        else:
            if (ram_usage >= RAM_WARN_THRESHOLD):
                print(Color.RED + "WARNING: Estimated RAM usage could be too " +
                      "low. Also, only statically allocated\nis taken into " +
                      "account. Be prepared that your app might still use " +
                      "more RAM than\navailable." + Color.RESET)
            print_bar([
                    (".data", size[".data"]/args.ram, Color.DATA),
                    (".bss", size[".bss"]/args.ram, Color.BSS)])
    else:
        print("No information about available RAM given.")

    print("")

    if (args.rom):
        rom_used = size[".data"] + size[".text"] + args.bootloader_size
        rom_usage = rom_used / args.rom
        print("Estimated ROM usage: {:.1f} KiB / {:.1f} KiB ({:.1%})".format(
                rom_used / 1024, args.rom / 1024, rom_usage))
        if (rom_used > args.rom):
            print("ROM overflown by {} Bytes".format(rom_used - args.rom))
            overflown = True
        else:
            items = []
            if (args.bootloader_size != 0):
                items.append((
                        "bootloader",
                        args.bootloader_size / args.rom,
                        Color.OFFSET
                ))
            items.append((".text", size[".text"] / args.rom, Color.TEXT))
            items.append((".data", size[".data"] / args.rom, Color.DATA))
            if (rom_usage >= ROM_WARN_THRESHOLD):
                print(Color.RED + "WARNING: Estimated rom usage could be " +
                      "too low. Be prepared that your app might\nstill " +
                      "not fit." + Color.RESET)
            print_bar(items)
    else:
        print("No information about available rom given.")

    if overflown:
        sys.exit(1)


def main():
    args = parse_arguments()
    if not os.path.isfile(args.file):
        sys.exit("File doesn't exist. (Hint: Run make)")
    size = parse_size(args)
    print_size(args, size)


if __name__ == "__main__":
    main()
