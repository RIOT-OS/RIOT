#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2017 Martine Lenders <m.lenders@fu-berlin.de>
#
# Distributed under terms of the MIT license.

from __future__ import print_function
import re
import sys


def generate_changelog(template_filename, changelog_filename, output_filename):
    with open(template_filename) as template, \
         open(changelog_filename) as changelog, \
         open(output_filename, "w") as output:
        changelog_lines = []
        release_title = re.compile(r"((RIOT-\d{4}\.\d{2} - Release Notes)|(Release 2013\.08))")
        notes_template = re.compile(r"\[Notes\]")
        title = 0
        prev_newline = False
        # Traverse changelog file line-wise
        for line in changelog:
            # Remove duplicate newlines
            if line == "\n" and not prev_newline:
                prev_newline = True
            elif line == "\n" and prev_newline:
                continue
            else:
                prev_newline = False
            if title:                           # if a release title was previously detected
                changelog_lines.append("\n")    # Remove the underline
                title = False
                prev_newline = True             # this introduces a newline, so count it
            elif release_title.match(line):
                # if line contains a release title
                release_match = re.search(r"(\d{4}\.\d{2})", line)
                assert(release_match is not None)
                # parse out release number
                release = release_match.group(1)
                title = "Release %s" % release
                tag = "release-%s" % release.replace('.', '-')
                # append as level 1 title with reference anchor tag
                changelog_lines.append("# %s {#%s}\n" % (title, tag))
                title = True
            else:
                # append non-title log lines as verbatim (so notation is kept)
                changelog_lines.append("    %s" % line)
        for line in template:
            # Traverse template file line-wise
            if notes_template.match(line):
                # if template string is matched: start adding changelog lines
                for line_log in changelog_lines:
                    print(line_log, file=output, end="")
            else:
                # just print the template line
                print(line, file=output, end="")


if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("usage %s <md template> <changelog> <output md>" % sys.argv[0], file=sys.stderr)
        sys.exit(1)
    generate_changelog(sys.argv[1], sys.argv[2], sys.argv[3])
