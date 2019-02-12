#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universitat Berlin
# Copyright (C) 2018 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#
# @author   Koen Zandberg <koen@bergzand.net>

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

LABELS_REMOVE = ['Process: needs backport']
LABELS_ADD = ['Process: release backport']

BACKPORT_BRANCH = 'backport/{release}/{origbranch}'


def _get_labels(pr):
    labels = {label['name'] for label in pr['labels']}
    for remove in LABELS_REMOVE:
        labels.discard(remove)
    labels.update(LABELS_ADD)
    return sorted(list(labels))


def _branch_name_strip(branch_name, prefix=RELEASE_PREFIX,
                       suffix=RELEASE_SUFFIX):
    """Strip suffix and prefix.

    >>> _branch_name_strip('2018.10-branch')
    '2018.10'
    """
    if (branch_name.startswith(prefix) and
            branch_name.endswith(suffix)):
        if prefix:
            branch_name = branch_name.split(prefix, maxsplit=1)[0]
        if suffix:
            branch_name = branch_name.rsplit(suffix, maxsplit=1)[0]
    return branch_name


def _get_latest_release(branches):
    version_latest = 0
    release_fullname = ''
    release_short = ''
    for branch in branches:
        branch_name = _branch_name_strip(branch['name'])
        branch_num = 0
        try:
            branch_num = int(''.join(branch_name.split('.')))
        except ValueError:
            pass
        if branch_num > version_latest:
            version_latest = branch_num
            release_short = branch_name
            release_fullname = branch['name']
    return (release_short, release_fullname)


def _get_upstream(repo):
    for remote in repo.remotes:
        if (remote.url.endswith("{}/{}.git".format(ORG, REPO)) or
                remote.url.endswith("{}/{}".format(ORG, REPO))):
            return remote


def _delete_worktree(repo, workdir):
    shutil.rmtree(workdir)
    repo.git.worktree('prune')


def main():
    keyfile = os.path.join(os.environ['HOME'], GITHUBTOKEN_FILE)
    parser = argparse.ArgumentParser()
    parser.add_argument("-k", "--keyfile", type=argparse.FileType('r'),
                        default=keyfile,
                        help="File containing github token")
    parser.add_argument("-c", "--comment", action="store_true",
                        help="Put a comment with a reference under"
                             "the original PR")
    parser.add_argument("-n", "--noop", action="store_true",
                        help="Limited noop mode, creates branch, but doesn't"
                             "push and create the PR")
    parser.add_argument("-r", "--release-branch", type=str,
                        help="Base the backport on this branch, "
                             "default is the latest")
    parser.add_argument("--backport-branch-fmt", type=str,
                        default=BACKPORT_BRANCH,
                        help="Backport branch format. "
                             "Fields '{release}' and '{origbranch} will be "
                             "replaced by the release name and remote branch "
                             "name.")
    parser.add_argument('-d', '--gitdir', type=str, default=os.getcwd(),
                        help="Base git repo to work from")
    parser.add_argument("PR", type=int, help="Pull request number to backport")
    args = parser.parse_args()

    gittoken = args.keyfile.read().strip()
    g = GitHub(token=gittoken)
    # TODO: exception handling
    status, user = g.user.get()
    if status != 200:
        print("Could not retrieve user: {}".format(user['message']))
        exit(1)
    username = user['login']
    status, pulldata = g.repos[ORG][REPO].pulls[args.PR].get()
    if status != 200:
        print("Commit #{} not found: {}".format(args.PR, pulldata['message']))
        sys.exit(2)
    if not pulldata['merged']:
        print("Original PR not yet merged")
        exit(0)
    print("Fetching for commit: #{}: {}".format(args.PR, pulldata['title']))
    orig_branch = pulldata['head']['ref']
    status, commits = g.repos[ORG][REPO].pulls[args.PR].commits.get()
    if status != 200:
        print("No commits found for #{}: {}".format(args.PR,
                                                    commits['message']))
        sys.exit(3)
    for commit in commits:
        print("found {} : {}".format(commit['sha'],
                                     commit['commit']['message']))

    # Find latest release branch
    if args.release_branch:
        release_fullname = args.release_branch
        release_shortname = _branch_name_strip(args.release_branch)
    else:
        status, branches = g.repos[ORG][REPO].branches.get()
        if status != 200:
            print("Could not retrieve branches for {}/{}: {}"
                  .format(ORG,
                          REPO,
                          branches['message']))
            sys.exit(4)
        release_shortname, release_fullname = _get_latest_release(branches)
        if not release_fullname:
            print("No release branch found, exiting")
            sys.exit(5)
    print("Backport based on branch {}".format(release_fullname))

    repo = git.Repo(args.gitdir)
    # Fetch current upstream
    upstream_remote = _get_upstream(repo)
    if not upstream_remote:
        print("No upstream remote found, can't fetch")
        exit(6)
    print("Fetching {} remote".format(upstream_remote))

    upstream_remote.fetch()
    # Build topic branch in temp dir
    new_branch = args.backport_branch_fmt.format(release=release_shortname,
                                                 origbranch=orig_branch)
    worktree_dir = os.path.join(args.gitdir, WORKTREE_SUBDIR)
    repo.git.worktree("add", "-b",
                      new_branch,
                      WORKTREE_SUBDIR,
                      "{}/{}".format(upstream_remote, release_fullname))
    bp_repo = git.Repo(worktree_dir)
    # Apply commits
    for commit in commits:
        bp_repo.git.cherry_pick('-x', commit['sha'])
    # Push to github
    print("Pushing branch {} to origin".format(new_branch))
    if not args.noop:
        repo.git.push('origin', '{0}:{0}'.format(new_branch))
    # Delete worktree
    print("Pruning temporary workdir at {}".format(worktree_dir))
    _delete_worktree(repo, worktree_dir)

    labels = _get_labels(pulldata)
    merger = pulldata['merged_by']['login']
    if not args.noop:
        # Open new PR on github
        pr = {
            'title': "{} [backport {}]".format(pulldata['title'],
                                               release_shortname),
            'head': '{}:{}'.format(username, new_branch),
            'base': release_fullname,
            'body': "# Backport of #{}\n\n{}".format(args.PR,
                                                     pulldata['body']),
            'maintainer_can_modify': True,
        }
        status, new_pr = g.repos[ORG][REPO].pulls.post(body=pr)
        if status != 201:
            print("Error creating the new pr: \"{}\". Is \"Public Repo\""
                  " access enabled for the token"
                  .format(new_pr['message']))
        pr_number = new_pr['number']
        print("Create PR number #{} for backport".format(pr_number))
        g.repos[ORG][REPO].issues[pr_number].labels.post(body=labels)
        review_request = {"reviewers": [merger]}
        g.repos[ORG][REPO].pulls[pr_number].\
            requested_reviewers.post(body=review_request)

    # Put commit under old PR
    if args.comment and not args.noop:
        comment = {"body": "Backport provided in #{}".format(pr_number)}
        status, res = g.repos[ORG][REPO].\
            issues[args.PR].comments.post(body=comment)
        if status != 201:
            print("Something went wrong adding the comment: {}"
                  .format(res['message']))
        print("Added comment to #{}".format(args.PR))


if __name__ == "__main__":
    main()
