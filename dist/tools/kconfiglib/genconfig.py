#!/usr/bin/env python3

# Copyright (c) 2018-2019, Ulf Magnusson
#               2020 HAW Hamburg
# SPDX-License-Identifier: ISC

"""
This script is used to merge multiple configuration sources and generate
different outputs related to Kconfig:

- Generate a header file with #defines from the configuration, matching the
format of include/generated/autoconf.h in the Linux kernel.

- Write the configuration output as a .config file. See --config-out.

- The --sync-deps, --file-list, and --env-list options generate information that
can be used to avoid needless rebuilds/reconfigurations.

Before writing a header or configuration file, Kconfiglib compares the old
contents of the file against the new contents. If there's no change, the write
is skipped. This avoids updating file metadata like the modification time, and
might save work depending on your build setup.

A custom header string can be inserted at the beginning of generated
configuration and header files by setting the KCONFIG_CONFIG_HEADER and
KCONFIG_AUTOHEADER_HEADER environment variables, respectively. The string is
not automatically made a comment (this is by design, to allow anything to be
added), and no trailing newline is added, so add '/* */', '#', and newlines as
appropriate.
"""
import argparse
import logging
import os
import sys

import kconfiglib


DEFAULT_SYNC_DEPS_PATH = "deps/"


class Colors:
    """
    ASCII colors for logging.
    """
    GREEN = "\033[1;32m"
    RED = "\033[1;31m"
    YELLOW = "\033[1;33m"
    PURPLE = "\033[1;35m"
    RESET = "\033[0m"


class NoConfigurationFile(Exception):
    """
    Raised when an operation that requires a configuration input file is
    executed but the file is not specified.
    """
    pass


def is_module(symbol):
    """
    Checks if a given symbol represents a module, depending on its prefix.
    """
    return symbol.name.startswith("MODULE_")


def is_error(symbol):
    """
    Checks if a given symbol represents an error, depending on its prefix.
    """
    return symbol.name.startswith("ERROR_")


def log_error(message):
    """
    Convenience function to log an error.
    """
    log(message, level=logging.ERROR)


def log(message, level=logging.DEBUG, color=None):
    """
    Logs a message using 'logging', with a given color and level. If no level is
    passed the message is logged using debug level. If no color is passed the
    following rules apply:
        - error messages are RED
        - warning messages are YELLOW
        - all other messages have no color
    """
    if sys.stdout.isatty():
        # running in a real terminal
        if color is None:
            if level == logging.ERROR:
                color = Colors.RED
            elif level == logging.WARNING:
                color = Colors.YELLOW
            else:
                color = Colors.RESET

        logging.log(level, "{}{}{}".format(color, message, Colors.RESET))
    else:
        # being piped or redirected
        logging.log(level, "{}".format(message))


def merge_configs(kconf, configs=[]):
    """
    Merges multiple configuration files given a Kconfig tree. configs should be
    an array of paths to the files that need to be merged.
    """
    # Enable warnings for assignments to undefined symbols
    kconf.warn_assign_undef = True

    # (This script uses alldefconfig as the base. Other starting states could be
    # set up here as well. The approach in examples/allnoconfig_simpler.py could
    # provide an allnoconfig starting state for example.)

    # Disable warnings generated for multiple assignments to the same symbol within
    # a (set of) configuration files. Assigning a symbol multiple times might be
    # done intentionally when merging configuration files.
    kconf.warn_assign_override = False
    kconf.warn_assign_redun = False

    # Create a merged configuration by loading the fragments with replace=False.
    if configs:
        for config in configs:
            log(kconf.load_config(config, replace=False))


def check_config_symbols(kconf):
    """
    Verifies that symbols got the values assigned by the user. This does not
    check choices. For that, please refer to check_config_choices.
    """
    ret = True

    for sym in kconf.unique_defined_syms:
        # choices are evaluated separately because when merging configurations
        # the choice could be overridden
        if sym.choice:
            continue

        # Was the symbol assigned to?
        if sym.user_value is None:
            continue

        # Tristate values are represented as 0, 1, 2. Having them as
        # "n", "m", "y" is more convenient here, so convert.
        if sym.type in (kconfiglib.BOOL, kconfiglib.TRISTATE):
            user_value = kconfiglib.TRI_TO_STR[sym.user_value]
        else:
            user_value = sym.user_value

        if user_value != sym.str_value:
            log("{} was assigned the value '{}' but got the value"
                " '{}'. Check the dependencies.".format(sym.name, user_value,
                                                        sym.str_value))

            symbol_type = "module" if is_module(sym) else "parameter"
            log_error("=> The {} {} could not be set to {}."
                      .format(symbol_type, sym.name, user_value))

            msg = ""

            missing_deps = get_sym_missing_deps(sym)
            if len(missing_deps):
                msg = "   Check the following unmet dependencies: "
                msg += ", ".join(missing_deps) + "\n"
            elif sym.type == kconfiglib.HEX or sym.type == kconfiglib.INT:
                rng = get_sym_applying_range(sym)
                if rng is not None:
                    msg = "   Check that the value is in the correct range:"
                    msg += "[{} - {}] {}\n".format(rng[0], rng[1], rng[2])

            log_error(msg)

            ret = False
    return ret


def check_config_choices(kconf):
    """
    Verifies that the choice options that have been selected after processing
    the configuration match what the user selected.
    This is verified separately from the rest of the symbols because as we
    are merging multiple configuration files the choice selection can be
    overridden, so this check needs a different logic.
    """
    ret = True

    for choice in kconf.unique_choices:
        if choice.user_selection and choice.user_selection is not choice.selection:
            ret = False
            log("{} choice option could not be set".format(choice.name))
            log_error("=> The choice {} was selected but was not set.\n"
                      .format(choice.user_selection.name_and_loc))

    return ret


def check_application_symbol(kconf):
    """
    Check that the APPLICATION symbol is set.

    If the special symbol APPLICATION is present it should be set to 'y'. It is
    used to:
      - imply modules that are optional for the application
      - add dependencies on other symbols or conditions (e.g. only use 32-bits
        architectures on this application)
    """
    app_sym = list(filter(lambda s: s.name == "APPLICATION", kconf.unique_defined_syms))
    if len(app_sym) > 1:
        log("=> The special symbol APPLICATION is defined more than once",
            level=logging.WARNING)
        log_error("=> The special symbol APPLICATION is defined more than once")
        return False
    elif len(app_sym) == 1:
        app = app_sym[0]
        if app.str_value != 'y':
            log("=> The application symbol (APPLICATION) is not set.",
                level=logging.WARNING)
            log_error("=> The application symbol (APPLICATION) is not set.")
            log_error("   Check that the symbol defaults to 'y'.")
            missing_deps = get_sym_missing_deps(app)
            if len(missing_deps):
                msg = "   Check the following unmet dependencies: "
                msg += ", ".join(missing_deps) + "\n"
                log_error(msg)
            return False

    return True


def check_configs(kconf):
    """
    Verifies that the generated configuration is valid.

    A configuration is not valid when:
        - A module could not be set to the value defined by the user.
        - A configuration parameter could not be set to value defined by the
          user.
    """
    test_kconfig = os.getenv("TEST_KCONFIG")

    if (test_kconfig):
        app_check = check_application_symbol(kconf)
    else:
        app_check = True

    sym_check = check_config_symbols(kconf)
    choice_check = check_config_choices(kconf)
    return app_check and sym_check and choice_check


def get_sym_missing_deps(sym):
    """
    Returns an array of strings, where each element is the string representation
    of the expressions on which `sym` depends and are missing.
    """
    # this splits the top expressions that are connected via AND (&&)
    # this will be the case, for example, for expressions that are defined in
    # multiple lines
    top_deps = kconfiglib.split_expr(sym.direct_dep, kconfiglib.AND)

    # we only need the expressions that are not met
    expr = [dep for dep in top_deps if kconfiglib.expr_value(dep) == 0]

    # convert each expression to strings and add the value for a friendlier
    # output message
    expr_str = []
    for expr in expr:
        s = kconfiglib.expr_str(expr)
        if isinstance(expr, tuple):
            s = "({})".format(s)
        expr_str.append("{} (={})".format(s, kconfiglib.TRI_TO_STR[kconfiglib.expr_value(expr)]))

    return expr_str


def get_sym_applying_range(sym):
    """
    Returns the first range that applies to a symbol (the active range) and the
    condition when it is not a constant.
    The return value is a tuple holding string representations of the range like
    so:
        (low, high, condition)
    When the condition is a constant (e.g. 'y' when there is no condition) it
    will be an empty string.
    """
    # multiple ranges could apply to a symbol
    for rng in sym.ranges:
        (low, high, cond) = rng
        # get the first active one
        if kconfiglib.expr_value(cond):
            return (low.str_value, high.str_value,
                    "(if {})".format(cond.name) if not cond.is_constant else "")
    return None


def main():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=__doc__)

    parser.add_argument(
        "--header-path",
        metavar="HEADER_FILE",
        help="""
Path to write the generated header file to. If not specified the header file is
not written.
""")

    parser.add_argument(
        "--config-out",
        metavar="CONFIG_FILE",
        help="""
Write the configuration to CONFIG_FILE. If not specified the file is not
written.
""")

    parser.add_argument(
        "--kconfig-filename",
        metavar="KCONFIG_FILENAME",
        nargs="?",
        default="Kconfig",
        help="Top-level Kconfig file (default: Kconfig)")

    parser.add_argument(
        "--sync-deps",
        metavar="OUTPUT_DIR",
        nargs="?",
        const=DEFAULT_SYNC_DEPS_PATH,
        help="""
Enable generation of symbol dependency information for incremental builds,
optionally specifying the output directory (default: {}). See the docstring of
Kconfig.sync_deps() in Kconfiglib for more information.
""".format(DEFAULT_SYNC_DEPS_PATH))

    parser.add_argument(
        "--file-list",
        metavar="FILE_LIST_FILE",
        help="""
Write a makefile listing all the Kconfig files used, and adding them as
dependencies of HEADER_FILE. The paths are absolute. Files appear in the order
they're 'source'd.
""")

    parser.add_argument(
        "--env-list",
        metavar="ENV_LIST_FILE",
        help="""
Write a list of all environment variables referenced in Kconfig files to
ENV_LIST_FILE, with one variable per line. Each line has the format NAME=VALUE.
Only environment variables referenced with the preprocessor $(VAR) syntax are
included, and not variables referenced with the older $VAR syntax (which is
only supported for backwards compatibility).
""")

    parser.add_argument(
        "--ignore-config-errors",
        action="store_true",
        help="""Configuration errors are reported but the script does not exit
with error.""")

    parser.add_argument(
        "--warnings-are-not-errors",
        action="store_true",
        help="Kconfig warnings are not considered errors")

    parser.add_argument(
        "-d", "--debug",
        action="store_true",
        help="Enable debug messages")

    parser.add_argument(
        "--config-sources",
        metavar="CONFIG_SOURCES",
        nargs='*',
        help="List of configuration files to merge and apply. May be empty.")

    args = parser.parse_args()
    log_level = logging.DEBUG if args.debug else logging.ERROR
    logging.basicConfig(format='[genconfig.py]:%(levelname)s-%(message)s',
                        level=log_level)

    kconf = kconfiglib.Kconfig(args.kconfig_filename, warn_to_stderr=False)
    merge_configs(kconf, args.config_sources)

    # HACK: Force all symbols to be evaluated, to catch warnings generated
    # during evaluation (such as out-of-range integers)
    kconf.write_config(os.devnull)

    if not check_configs(kconf) and not args.ignore_config_errors:
        sys.exit(1)

    if kconf.warnings:
        if args.warnings_are_not_errors:
            for warning in kconf.warnings:
                log(warning, level=logging.WARNING)
        else:
            log_error("Treating Kconfig warnings as errors:")
            for warning in kconf.warnings:
                log_error("=> {}".format(warning))
            if not args.ignore_config_errors:
                sys.exit(1)

    if args.config_out is not None:
        logging.debug(kconf.write_config(args.config_out, save_old=False))

    if args.header_path is not None:
        logging.debug(kconf.write_autoconf(args.header_path))

    if args.sync_deps is not None:
        log("Incremental build header files generated at '{}'".format(args.sync_deps))
        kconf.sync_deps(args.sync_deps)

    if args.file_list is not None:
        if args.config_out is None:
            raise NoConfigurationFile("Can't generate Kconfig dependency file without configuration file")
        log("Kconfig dependencies written to '{}'".format(args.file_list))
        with open(args.file_list, "w", encoding="utf-8") as f:
            f.write("{}: \\\n".format(args.config_out))
            # add dependencies
            for path in kconf.kconfig_filenames:
                f.write("    {} \\\n".format(os.path.abspath(path)))
            # add empty recipes for dependencies
            f.write("\n\n")
            for path in kconf.kconfig_filenames:
                f.write("{}:\n\n".format(os.path.abspath(path)))

    if args.env_list is not None:
        log("Kconfig environmental variables written to '{}'".format(args.env_list))
        with open(args.env_list, "w", encoding="utf-8") as f:
            for env_var in kconf.env_vars:
                f.write("{}={}\n".format(env_var, os.environ[env_var]))


if __name__ == "__main__":
    main()
