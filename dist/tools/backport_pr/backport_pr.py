#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universitat Berlin
# Copyright (C) 2018 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#
# @author   Koen Zandberg <koen@bergzand.net>

"""Script used to backport PRs."""

import os
import os.path
import sys
import shutil
import argparse

import git
from agithub.GitHub import GitHub

ORG = "RIOT-OS"
REPO = "RIOT"
GITHUBTOKEN_FILE = ".riotgithubtoken"
WORKTREE_SUBDIR = "backport_temp"

RELEASE_PREFIX = ""
RELEASE_SUFFIX = "-branch"

LABELS_REMOVE = ["Process: needs backport", "Reviewed: "]
LABELS_ADD = ["Process: release backport"]

BACKPORT_BRANCH = "backport/{release}/{origbranch}"


def _get_labels(pull_request):
    """
    >>> _get_labels({'labels': [{'name': 'test'}, {'name': 'abcd'}]})
    ['Process: release backport', 'abcd', 'test']
    >>> _get_labels({'labels': [{'name': 'Reviewed: what'}, \
        {'name': 'Reviewed: 3-testing'}]})
    ['Process: release backport']
    >>> _get_labels({'labels': [{'name': 'Process: release backport'}]})
    ['Process: release backport']
    >>> _get_labels({'labels': [{'name': 'Process: needs backport'}]})
    ['Process: release backport']
    """
    labels = set(
        label["name"]
        for label in pull_request["labels"]
        if all(not label["name"].startswith(remove) for remove in LABELS_REMOVE)
    )
    labels.update(LABELS_ADD)
    return sorted(list(labels))


def _branch_name_strip(branch_name, prefix=RELEASE_PREFIX, suffix=RELEASE_SUFFIX):
    """Strip suffix and prefix.

    >>> _branch_name_strip('2018.10-branch')
    '2018.10'
    """
    if branch_name.startswith(prefix) and branch_name.endswith(suffix):
        if prefix:
            branch_name = branch_name.split(prefix, maxsplit=1)[0]
        if suffix:
            branch_name = branch_name.rsplit(suffix, maxsplit=1)[0]
    return branch_name


def _get_latest_release(branches):
    """Get latest release from a list of branches.

    >>> _get_latest_release([{'name': '2018.10-branch'}, \
        {'name': '2020.10-branch'}])
    ('2020.10', '2020.10-branch')
    >>> _get_latest_release([{'name': '2020.01-branch'}, \
        {'name': '2020.04-branch'}])
    ('2020.04', '2020.04-branch')
    >>> _get_latest_release([{'name': 'non-release-branch'}, \
        {'name': '2020.04-branch'}])
    ('2020.04', '2020.04-branch')
    """
    version_latest = 0
    release_fullname = ""
    release_short = ""
    for branch in branches:
        branch_name = _branch_name_strip(branch["name"])
        branch_num = 0
        try:
            branch_num = int("".join(branch_name.split(".")))
        except ValueError:
            pass
        if branch_num > version_latest:
            version_latest = branch_num
            release_short = branch_name
            release_fullname = branch["name"]
    return (release_short, release_fullname)


def _find_remote(repo, user, repo_name):
    for remote in repo.remotes:
        if remote.url.endswith(f"{user}/{repo_name}.git") or remote.url.endswith(
            f"{user}/{repo_name}"
        ):
            return remote
    raise ValueError(f"Could not find remote with URL ending in {user}/{repo_name}.git")


def _get_upstream(repo):
    return _find_remote(repo, ORG, REPO)


def _delete_worktree(repo, workdir):
    shutil.rmtree(workdir)
    repo.git.worktree("prune")


def main():
    # pylint:disable=too-many-locals,too-many-branches,too-many-statements
    """Main function of this script."""
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
        action="store_true",
        help="Put a comment with a reference under" "the original PR",
    )
    parser.add_argument(
        "-n",
        "--noop",
        action="store_true",
        help="Limited noop mode, creates branch, but doesn't" "push and create the PR",
    )
    parser.add_argument(
        "-r",
        "--release-branch",
        type=str,
        help="Base the backport on this branch, " "default is the latest",
    )
    parser.add_argument(
        "--backport-branch-fmt",
        type=str,
        default=BACKPORT_BRANCH,
        help="Backport branch format. "
        "Fields '{release}' and '{origbranch} will be "
        "replaced by the release name and remote branch "
        "name.",
    )
    parser.add_argument(
        "-d",
        "--gitdir",
        type=str,
        default=os.getcwd(),
        help="Base git repo to work from",
    )
    parser.add_argument("PR", type=int, help="Pull request number to backport")
    args = parser.parse_args()

    gittoken = args.keyfile.read().strip()
    github_api = GitHub(token=gittoken)
    # TODO: exception handling
    status, user = github_api.user.get()
    if status != 200:
        print(f'Could not retrieve user: {user["message"]}')
        sys.exit(1)
    # Token-scope-check: Is the token is powerful enough to complete
    # the Backport?
    response_headers = dict(github_api.getheaders())
    # agithub documentation says it's lower case header field-names but
    # at this moment it's not
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
    username = user["login"]
    status, pulldata = github_api.repos[ORG][REPO].pulls[args.PR].get()
    if status != 200:
        print(f'Commit #{args.PR} not found: {pulldata["message"]}')
        sys.exit(2)
    if not pulldata["merged"]:
        print("Original PR not yet merged")
        sys.exit(0)
    print(f'Fetching for commit: #{args.PR}: {pulldata["title"]}')
    orig_branch = pulldata["head"]["ref"]
    status, commits = github_api.repos[ORG][REPO].pulls[args.PR].commits.get()
    if status != 200:
        print(f'No commits found for #{args.PR}: {commits["message"]}')
        sys.exit(3)
    for commit in commits:
        print(f'found {commit["sha"]} : {commit["commit"]["message"]}')

    # Find latest release branch
    if args.release_branch:
        release_fullname = args.release_branch
        release_shortname = _branch_name_strip(args.release_branch)
    else:
        status, branches = github_api.repos[ORG][REPO].branches.get()
        if status != 200:
            print(
                f"Could not retrieve branches for {ORG}/{REPO}: "
                f'{branches["message"]}'
            )
            sys.exit(4)
        release_shortname, release_fullname = _get_latest_release(branches)
        if not release_fullname:
            print("No release branch found, exiting")
            sys.exit(5)
    print(f"Backport based on branch {release_fullname}")

    repo = git.Repo(args.gitdir)
    # Fetch current upstream
    upstream_remote = _get_upstream(repo)
    if not upstream_remote:
        print("No upstream remote found, can't fetch")
        sys.exit(6)
    print(f"Fetching {upstream_remote} remote")

    upstream_remote.fetch()
    # Build topic branch in temp dir
    new_branch = args.backport_branch_fmt.format(
        release=release_shortname, origbranch=orig_branch
    )
    if new_branch in repo.branches:
        print(f"ERROR: Branch {new_branch} already exists")
        sys.exit(1)
    worktree_dir = os.path.join(args.gitdir, WORKTREE_SUBDIR)
    repo.git.worktree(
        "add",
        "-b",
        new_branch,
        WORKTREE_SUBDIR,
        f"{upstream_remote}/{release_fullname}",
    )
    # transform branch name into Head object for later configuring
    new_branch = repo.branches[new_branch]
    try:
        bp_repo = git.Repo(worktree_dir)
        # Apply commits
        for commit in commits:
            bp_repo.git.cherry_pick("-x", commit["sha"])
        # Push to github
        origin = _find_remote(repo, username, REPO)
        print(f"Pushing branch {new_branch} to {origin}")
        if not args.noop:
            push_info = origin.push(f"{new_branch}:{new_branch}")
            new_branch.set_tracking_branch(push_info[0].remote_ref)
    except Exception as exc:
        # Delete worktree
        print(f"Pruning temporary workdir at {worktree_dir}")
        _delete_worktree(repo, worktree_dir)
        # also delete branch created by worktree; this is only possible after
        # the worktree was deleted
        repo.delete_head(new_branch)
        raise exc
    else:
        # Delete worktree
        print(f"Pruning temporary workdir at {worktree_dir}")
        _delete_worktree(repo, worktree_dir)

    labels = _get_labels(pulldata)
    merger = pulldata["merged_by"]["login"]
    if not args.noop:
        # Open new PR on github
        pull_request = {
            "title": f'{pulldata["title"]} [backport {release_shortname}]',
            "head": f"{username}:{new_branch}",
            "base": release_fullname,
            "body": f'# Backport of #{args.PR}\n\n{pulldata["body"]}',
            "maintainer_can_modify": True,
        }
        status, new_pr = github_api.repos[ORG][REPO].pulls.post(body=pull_request)
        if status != 201:
            print(
                f'Error creating the new pr: "{new_pr["message"]}". '
                'Is "Public Repo" access enabled for the token?'
            )
        pr_number = new_pr["number"]
        print(f"Create PR number #{pr_number} for backport")
        github_api.repos[ORG][REPO].issues[pr_number].labels.post(body=labels)
        review_request = {"reviewers": [merger]}
        github_api.repos[ORG][REPO].pulls[pr_number].requested_reviewers.post(
            body=review_request
        )

    # Put commit under old PR
    if args.comment and not args.noop:
        comment = {"body": f"Backport provided in #{pr_number}"}
        status, res = (
            github_api.repos[ORG][REPO].issues[args.PR].comments.post(body=comment)
        )
        if status != 201:
            print(f'Something went wrong adding the comment: {res["message"]}')
        print(f"Added comment to #{args.PR}")


if __name__ == "__main__":
    main()
