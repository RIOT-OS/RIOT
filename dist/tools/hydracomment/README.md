Comment on multiple PRs
=============================

This script provides functionality to place the same comment simultaneously on
multiple PRs

It relies on having a `github` API token, stored in `~/.riotgithubtoken` by
default.

Usage
-----

Most common usage would be to run:

    hydracomment.py PR_NUMBER1 PR_NUMBER2 ...

See the backport_pr README.md for instructions on creating a token with correct
permissions
