#!/usr/bin/env python3

# Copyright (C) 2021 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import time
import shlex
import subprocess
import argparse

from contextlib import contextmanager


SUCCESS = "\033[32;1m✓\033[0m"
FAILED = "\033[31;1m×\033[0m"
SPIN = ["⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"]


class Programmer:

    @contextmanager
    def spawn_process(self):
        """Yield a subprocess running in background."""
        kwargs = {} if self.verbose else {
            "stdout": subprocess.PIPE,
            "stderr": subprocess.STDOUT
        }
        yield subprocess.Popen(shlex.split(self.cmd), **kwargs)

    def spin(self, process):
        """Print a spinning icon while programmer process is running."""
        while process.poll() is None:
            for index in range(len(SPIN)):
                sys.stdout.write(
                    "\r \033[36;1m{}\033[0m {} in progress "
                    "(programmer: '{}')"
                    .format(SPIN[index], self.action, self.programmer)
                )
                sys.stdout.flush()
                time.sleep(0.1)

    def print_status(self, process, elapsed):
        """Print status of background programmer process."""
        print(
            "\r \u001b[2K{} {} {} (programmer: '{}' - duration: {:0.2f}s)"
            .format(
                FAILED if process.returncode != 0 else SUCCESS,
                self.action,
                "failed!" if process.returncode != 0 else "done!",
                self.programmer,
                elapsed
            )
        )
        # Print content of stdout (which also contain stderr) when the
        # subprocess failed
        if process.returncode != 0:
            print(process.stdout.read().decode())
        else:
            print(
                "(for full programmer output add PROGRAMMER_QUIET=0 or "
                "QUIET=0 to the make command line)"
            )

    def run(self):
        """Run the programmer in a background process."""
        if not self.cmd.strip():
            # Do nothing if programmer command is empty
            return 0

        if self.verbose:
            print(self.cmd)
        start = time.time()
        with self.spawn_process() as proc:
            try:
                if self.verbose:
                    proc.communicate()
                else:
                    self.spin(proc)
            except KeyboardInterrupt:
                proc.terminate()
                proc.kill()
        elapsed = time.time() - start
        if not self.verbose:
            # When using the spinning icon, print the programmer status
            self.print_status(proc, elapsed)

        return proc.returncode


def main(parser):
    """Main function."""
    programmer = Programmer()
    parser.parse_args(namespace=programmer)
    # Return with same return code as subprocess
    sys.exit(programmer.run())


def parser():
    """Return an argument parser."""
    parser = argparse.ArgumentParser()
    parser.add_argument("--action", help="Programmer action")
    parser.add_argument("--cmd", help="Programmer command")
    parser.add_argument("--programmer", help="Programmer")
    parser.add_argument(
        "--verbose", action='store_true', default=False, help="Verbose output"
    )
    return parser


if __name__ == "__main__":
    main(parser())
