---
title: Release Cycle
description: Information about the RIOT release cycle
---

RIOT has a new release every three months, named after the month it was feature frozen in.
For example, `2025.01` was feature frozen in January 2025. Feature freeze marks
the branch-off point of the `<YYYY.MM>-branch`, where `<YYYY.MM>` is the release name.
Any new features merged into `master` after that point will be part of the next release.
This branch, referred to as the release branch, is the stable branch of RIOT.

After feature freeze, the current release candidate in the release branch undergoes extensive testing.
For more information on the release testing, see our
[release specifications](https://github.com/RIOT-OS/Release-Specs). Bug fixes made during this testing period
are backported from `master` to the release branch, and new release candidates may be created
when certain milestones are reached. Once all major bugs are fixed, the new RIOT release
is signed off by the appointed release manager, typically within 2 weeks after the feature freeze.
For more details and instructions for release managers, see the
[managing a release guide](https://guide.riot-os.org/misc/managing-a-release/).

RIOT follows a rolling release cycle, meaning support and bug fixes are only provided
for the most current release.

## Download a Release

You can download the source code of our releases
[via GitHub](https://github.com/RIOT-OS/RIOT/releases) as a ZIP file or tarball.
Alternatively, you can check them out if you've already cloned RIOT with Git:

```bash
git pull --tags
git checkout <YYYY.MM>
```

## Point Releases and Hot Fixes

For major bug fixes, we may provide a point release `YYYY.MM.N`, resulting in a new ZIP file
or tarball on the [release page](https://github.com/RIOT-OS/RIOT/releases). However,
minor bug fixes are only pushed to the release branch. You can fetch that via Git using:

```bash
git clone -b <YYYY.MM>-branch https://github.com/RIOT-OS/RIOT
```

or if you have RIOT already cloned:

```bash
git pull origin <YYYY.MM>-branch:<YYYY.MM>-branch
git checkout <YYYY.MM>-branch
```
