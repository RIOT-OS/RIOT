#!/usr/bin/python3
"""
Command line utility to check if only a subset of board / application combinations
need to be build in the CI
"""
import argparse
import io
import json
import os
import re
import subprocess
import sys
from functools import partial

REGEX_GIT_DIFF_RENAME_COMPLEX = re.compile(r"^[0-9-]+\s+[0-9-]+\s+(.*)\{(.*) => (.*)\}(.*)$")
REGEX_GIT_DIFF_RENAME_SIMPLE = re.compile(r"^[0-9-]+\s+[0-9-]+\s+(.*) => (.*)$")
REGEX_GIT_DIFF_SINGLE_FILE = re.compile(r"^[0-9-]+\s+[0-9-]+\s+(.*)$")

# Beware: Order matters. The first matching rule will be applied
OTHER_CLASSIFIERS = [
    [re.compile(r"^(Makefile.*|sys\/Makefile.*|drivers\/Makefile.*|makefiles\/.*)$"), "build-system"],
    [re.compile(r"^(drivers\/include\/.*|sys\/include\/.*)$"), "public-headers"],
    [re.compile(r"^(Kconfig|kconfigs\/.*|pkg\/Kconfig|sys\/Kconfig|drivers\/Kconfig)$"), "kconfig"],
    [re.compile(r"^(.*\.cff|doc\/.*|.*\.md|.*\.txt)$"), "doc"],
    [re.compile(r"^(CODEOWNERS|.mailmap|.gitignore|.github\/.*)$"), "git"],
    [re.compile(r"^(\.murdock|dist\/ls\/.*|\.drone.yml)$"), "ci-murdock"],
    [re.compile(r"^(\.bandit|\.circleci\/.*|\.drone.yml)$"), "ci-other"],
    [re.compile(r"^(dist\/.*|Vagrantfile)$"), "tools"],
]

REGEX_MODULE = re.compile(r"^(boards\/common|core|cpu|drivers|sys)\/")
REGEX_PKG = re.compile(r"^pkg\/")
REGEX_BOARD = re.compile(r"^boards\/")
REGEX_APP = re.compile(r"^(bootloaders|examples|fuzzing|tests)\/")

EXCEPTION_MODULES = {"boards/common/nrf52"}

print_err = partial(print, file=sys.stderr)


def print_change_set_section(name, contents):
    """
    Print the given change set section human reable
    """
    if not contents:
        return
    print_err(name)
    print_err("=" * len(name))
    print_err("")
    for category in sorted(contents):
        print_err(category)
        print_err("-" * len(category))
        print_err("")
        for file in sorted(contents[category]):
            print_err("- {}".format(file))
        print_err("")


class ChangeSet:
    """
    Representation of the modules affected by a change set
    """
    def __init__(self, riotbase=os.getcwd()):
        self.apps = {}
        self.boards = {}
        self.modules = {}
        self.other = {}
        self.pkgs = {}
        self._riotbase = os.path.normpath(riotbase)

    def __add_module(self, dest, file):
        module = os.path.dirname(file)
        while module != "":
            makefile = os.path.join(self._riotbase, module, "Makefile")
            if os.path.isfile(makefile) or module in EXCEPTION_MODULES:
                if module in dest:
                    dest[module].append(file)
                else:
                    dest[module] = [file]
                return
            module = os.path.dirname(module)
        raise Exception("Module containing file \"{}\" not found".format(file))

    def add_file(self, file):
        """
        Add the given file to the change set
        """
        # normalize path
        file = os.path.normpath(file)
        if file.startswith('./'):
            file = file[2:]
        # turn path into path relative to riotbase, if needed
        if file.startswith(self._riotbase):
            file = file[len(self._riotbase):]

        for regex, name in OTHER_CLASSIFIERS:
            if regex.match(file):
                if name in self.other:
                    self.other[name].append(file)
                else:
                    self.other[name] = [file]
                return

        if REGEX_MODULE.match(file):
            self.__add_module(self.modules, file)
        elif REGEX_PKG.match(file):
            self.__add_module(self.pkgs, file)
        elif REGEX_BOARD.match(file):
            self.__add_module(self.boards, file)
        elif REGEX_APP.match(file):
            self.__add_module(self.apps, file)
        else:
            raise Exception("File \"{}\" doesn't match any known category".format(file))

    def print_files_and_classifications(self):
        """
        Print all files and their classification in human readable format
        """
        print_change_set_section("Other", self.other)
        print_change_set_section("Modules", self.modules)
        print_change_set_section("Packages", self.pkgs)
        print_change_set_section("Boards", self.boards)
        print_change_set_section("Apps", self.apps)


def classify_changes(riotbase=None, upstream_branch="master"):
    """
    Runs the given compiler with -v -E on an no-op compilation unit and parses the built-in
    include search directories and the GCC version from the output

    :param args: parse command line arguments
    :type args: dict
    :param pr_branch: name of the PR branch
    :type pr_branch: str
    :param upstream_branch: name of the main upstream branch the PR should be merged into
    :type upstream_branch: str

    :return: True if fast rebuilt is possible, False otherwise
    :rtype: bool
    """
    change_set = ChangeSet(riotbase)

    with subprocess.Popen(["git", "diff", "--numstat", "HEAD..{}".format(upstream_branch)],
                          stdout=subprocess.PIPE) as proc:
        for line in io.TextIOWrapper(proc.stdout, encoding="utf-8"):
            match = REGEX_GIT_DIFF_RENAME_COMPLEX.match(line)
            if match:
                prefix = match.group(1)
                suffix = match.group(4)
                file_before = prefix + match.group(2) + suffix
                file_after = prefix + match.group(3) + suffix
                change_set.add_file(file_before)
                change_set.add_file(file_after)
                continue

            match = REGEX_GIT_DIFF_RENAME_SIMPLE.match(line)
            if match:
                file_before = match.group(1)
                file_after = match.group(2)
                change_set.add_file(file_before)
                change_set.add_file(file_after)
                continue

            match = REGEX_GIT_DIFF_SINGLE_FILE.match(line)
            if match:
                file = match.group(1)

                if only_comment_change(file, upstream_branch) is False:
                    change_set.add_file(file)

                continue

            raise Exception("Failed to parse \"{}\"".format(line))

    return change_set


def get_hash_without_comments(file, ref):
    result = subprocess.run(
        f"git show {ref}:{file} | gcc -fpreprocessed -dD -E -P - | md5sum",
        shell=True,
        capture_output=True,
        check=True,
    )
    return result.stdout


def only_comment_change(file, upstream_branch):
    try:
        if file[-2:] in {".c", ".h"}:
            hash_a = get_hash_without_comments(file, "HEAD")
            hash_b = get_hash_without_comments(file, upstream_branch)
            return hash_a == hash_b
    except Exception:
        pass

    return False


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Check if a fast CI run is possible and which "
                                     + "boards / applications to build")
    parser.add_argument("--explain", action="store_true",
                        help="Explain the reasoning of the decision")
    parser.add_argument("--debug", default=False, action="store_const", const=True,
                        help="Show detailed list of classifications")
    parser.add_argument("--riotbase", default=os.getcwd(),
                        help="Use given paths as RIOT's base path (instead of pwd)")
    parser.add_argument("--upstreambranch", default="master",
                        help="The branch / commit containing the upstream state")

    parser.add_argument("--changed-boards", action="store_true",
                        help="list all boards for all apps should be rebuilt")

    parser.add_argument("--changed-apps", action="store_true",
                        help="list all apps that should be built for all boards")

    parser.add_argument("--json", action="store_true",
                        help="json dump changed boards / apps")

    args = parser.parse_args()
    try:
        change_set = classify_changes(riotbase=args.riotbase, upstream_branch=args.upstreambranch)
    except Exception as e:
        print_err("Couldn't classify changes: {}".format(e))
        sys.exit(1)

    if args.debug:
        change_set.print_files_and_classifications()

    if "kconfig" in change_set.other or "build-system" in change_set.other:
        if args.explain:
            print_err("General build system / KConfig changes require a full CI run")
        sys.exit(1)

    if "ci-murdock" in change_set.other:
        if args.explain:
            print_err("Murdock related changes require a full CI run")

        # only exit/error if MURDOCK_TEST_CHANGE_FILTER is not set.
        # useful for testing.
        if not os.environ.get("MURDOCK_TEST_CHANGE_FILTER"):
            sys.exit(1)

    if "public-headers" in change_set.other:
        if args.explain:
            print_err("Changes in public headers require a full CI run")
        sys.exit(1)

    if len(change_set.modules) > 0:
        if args.explain:
            print_err("Currently changing modules require a full CI run")
        sys.exit(1)

    if len(change_set.pkgs) > 0:
        if args.explain:
            print_err("Currently changing packages require a full CI run")
        sys.exit(1)

    if args.json or args.changed_boards or args.changed_apps:
        result = {
            "apps": sorted(change_set.apps.keys()),
            "boards": sorted(change_set.boards.keys())
        }

    if args.json:
        print(json.dumps(result, indent=2))

    if args.changed_boards:
        changed_boards = " ".join(result.get("boards", []))
        print(f"BOARDS_CHANGED=\"{changed_boards}\"")

    if args.changed_apps:
        changed_apps = " ".join(result.get("apps", []))
        print(f"APPS_CHANGED=\"{changed_apps}\"")
