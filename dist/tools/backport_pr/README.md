Pull requests backport script
=============================

This script provides functionality to easily backport a merged pull request to
a release branch.

It relies on having a `github` API token stored in `~/.riotgithubtoken` by
default.


The script works by fetching information from the supplied **merged** pull
request. It then looks for the last release branch.
A temporary git `worktree` with a new branch is created based on this release
branch. All commits from the pull request are then cherry-picked into this
branch which is then pushed to `origin`.
It then creates a new pull request on `github` with a reference to the original
pull request. It optionally puts a comment under the original pull request to
the new backport pull request.



Usage
-----

Most common usage would be to run:

    backport_pr.py --comment PR_NUMBER


If you are executing from a worktree, the script cannot currently detect the
base git directory and must be used with `--gitdir PATH_TO_ORIGINAL_GIT_DIR`


Full help:

    usage: backport_pr.py [-h] [-k KEYFILE] [-c] [-n] [-r RELEASE_BRANCH]
                          [--backport-branch-fmt BACKPORT_BRANCH_FMT] [-d GITDIR]
                          PR

    positional arguments:
      PR                    Pull request number to backport

    optional arguments:
      -h, --help            show this help message and exit
      -k KEYFILE, --keyfile KEYFILE
                            File containing github token
      -c, --comment         Put a comment with a reference underthe original PR
      -n, --noop            Limited noop mode, creates branch, but doesn'tpush and
                            create the PR
      -r RELEASE_BRANCH, --release-branch RELEASE_BRANCH
                            Base the backport on this branch, default is the
                            latest
      --backport-branch-fmt BACKPORT_BRANCH_FMT
                            Backport branch format. Fields '{release}' and
                            '{origbranch} will be replaced by the release name and
                            remote branch name.
      -d GITDIR, --gitdir GITDIR
                            Base git repo to work from


### Create an authentication token

A `Personal access token` must be created on the `github` website and stored in
your home directory at `~/.riotgithubtoken`.

The setting is located at https://github.com/settings/tokens :

    Github->Settings->
        Developer settings->Personal access tokens->Generate New Token

And it should have the following scope:

    repo[public repo]: Access public repositories


**Warning** its value should be saved directly as you cannot see it later
anymore.
