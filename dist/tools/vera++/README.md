## Overview

Vera++ is a programmable tool for verification, analysis and transformation of
C++ source code. Vera++ is mainly an engine that parses C++ source files and
presents the result of this parsing to scripts in the form of various
collections - the scripts are actually performing the requested tasks.

## Rules

Rules can be added in the `rules` folder. The default rules from Vera++ were
added. However, it's possible to implement new rules in TCL. See
https://bitbucket.org/verateam/vera/wiki/Rules for information about the
available rules.

# Profiles

A `riot_force` profile was made for describing the set of applied rules as well as
special parameters. A `riot` profile includes all rules that should be treated as
warnings. In case new rules are added, they can be updated for both profiles under
`profiles` folder.

## Example usage
Check style for all changed files changed in current branch:
    check.sh

Check all changed files and don't display the results (only exit code):
    QUIET=1 check.sh

Don't display the warnings
    WARNING=0 check.sh
