""" RIOT customization of Kconfig """
import argparse
import sys

from kconfiglib import Kconfig, KconfigError


class RiotKconfig(Kconfig):
    """ RIOT adaption of Kconfig class """
    def _parse_help(self, node):
        """ Parses the help section of a node, removing Doxygen markers """
        doxygen_markers = ["@ref ", "@see "]

        # call default parsing
        super(RiotKconfig, self)._parse_help(node)

        # remove Doxygen markers
        for marker in doxygen_markers:
            node.help = node.help.replace(marker, "")

    def write_autoconf(self, filename=None, header="/* RIOT Configuration File */\n"):
        """ Override to convert - to _ when writing autoconf.h """
        tmp_unique_defined_syms = self.unique_defined_syms.copy()
        for sym in self.unique_defined_syms:
            if not sym._write_to_conf:
                continue
            sym.name = sym.name.replace('-', '_')
        super(RiotKconfig, self).write_autoconf(filename, header)
        self.unique_defined_syms = tmp_unique_defined_syms


def standard_riot_kconfig(description=None):
    """
    Argument parsing helper for tools that take a single optional Kconfig file
    argument (default: Kconfig). Returns the RiotKconfig instance for the parsed
    configuration. Uses argparse internally.

    Exits with sys.exit() (which raises SystemExit) on errors.

    description (default: None):
      The 'description' passed to argparse.ArgumentParser().
      argparse.RawDescriptionHelpFormatter is used, so formatting is preserved.
    """
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=description)

    parser.add_argument(
        "kconfig",
        metavar="KCONFIG",
        default="Kconfig",
        nargs="?",
        help="Kconfig file (default: Kconfig)")

    args = parser.parse_args()

    # Suppress backtraces for expected exceptions
    try:
        return RiotKconfig(args.kconfig)
    except (EnvironmentError, KconfigError) as e:
        # Some long exception messages have extra newlines for better
        # formatting when reported as an unhandled exception. Strip them here.
        sys.exit(str(e).strip())
