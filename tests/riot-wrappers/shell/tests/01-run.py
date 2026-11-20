#!/usr/bin/env python3

import sys
from testrunner import run

def test(child):
    child.expect("> ")
    child.sendline("help")
    # Could also be the other sequence, we're not guaranteeing that
    commands = ["closure", "echo"]
    helps = ["Run a command that holds a mutable reference", "Print the arguments in separate lines"]
    command1 = child.expect(commands)
    help1 = child.expect(helps)
    command2 = child.expect(commands)
    help2 = child.expect(helps)
    if command1 != help1 or command2 != help2:
        print("Commands and helps were mixed up")
        sys.exit(1)
    child.expect("> ")
    child.sendline("echo foo bar")
    child.expect("- echo")
    child.expect("- foo")
    child.expect("- bar")
    child.expect("> ")
    child.sendline("closure")
    child.expect("New state is 1")
    child.expect("> ")
    child.sendline("closure")
    child.expect("New state is 2")

if __name__ == "__main__":
    sys.exit(run(test))
