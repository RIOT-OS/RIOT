#!/usr/bin/env python3

# Copyright (C) 2023 Koen Zandberg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#
# @author   Koen Zandberg <koen@bergzand.net>

"""
Place identical comments on multiple PRs.
Note that with great power comes great responsibility
"""

import os
import sys
import argparse
from agithub.GitHub import GitHub

ORG = "RIOT-OS"
REPO = "RIOT"
GITHUBTOKEN_FILE = ".riotgithubtoken"


def main():
    keyfile = os.path.join(os.environ["HOME"], GITHUBTOKEN_FILE)
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-k",
        "--keyfile",
        type=argparse.FileType("r"),
        default=keyfile,
        help="File containing github token",
    )
    parser.add_argument(
        "-c",
        "--comment",
        default="Bors merge",
        type=str,
        help="Comment to place under all PRs.",
    )
    parser.add_argument(
        "PR",
        nargs='+',
        type=int,
        help="Pull request number to place comment on",
    )
    args = parser.parse_args()

    gittoken = args.keyfile.read().strip()
    github_api = GitHub(token=gittoken)

    status, user = github_api.user.get()
    if status != 200:
        print(f'Could not retrieve user: {user["message"]}')
        sys.exit(1)

    response_headers = dict(github_api.getheaders())
    if "X-OAuth-Scopes" in response_headers:
        scopes = response_headers["X-OAuth-Scopes"]
    else:
        scopes = response_headers["x-oauth-scopes"]
    scopes_list = [x.strip() for x in scopes.split(",")]
    if not ("public_repo" in scopes_list or "repo" in scopes_list):
        print(
            "missing public_repo scope from token settings."
            " Please add it on the GitHub webinterface"
        )
        sys.exit(1)

    comment = {"body": args.comment}
    for prnum in args.PR:
        status, pulldata = github_api.repos[ORG][REPO].pulls[prnum].get()
        if status != 200:
            print(f'PR #{prnum} not found: {pulldata["message"]}')
            continue
        status, res = (
            github_api.repos[ORG][REPO].issues[prnum].comments.post(body=comment)
        )
        if status != 201:
            print(f'Something went wrong adding the comment to #{prnum}: {res["message"]}')
        else:
            print(f"Added comment to #{prnum}")


if __name__ == "__main__":
    main()
