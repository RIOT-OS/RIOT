---
title: Managing a Release
description: This page describes the process of managing a release.
---

## Contents

1. [Checklist](#1-checklist)
2. [Preparation](#2-preparation)
3. [Feature Freeze and Testing](#3-feature-freeze-and-testing)
4. [Drafting Release Notes](#4-drafting-release-notes)
5. [Actual Release](#5-actual-release)
6. [Other Repositories](#6-other-repositories)
7. [Important Fixes after Release](#7-important-fixes-after-release)
8. [Resources](#8-resources)
9. [Forum post templates](#9-forum-post-templates)

## 1. Checklist

Steps marked with a :scroll: icon can also be automated with the release manager script.

**Preparation**
- [ ] Read “Managing a release” guide
- [ ] Have an eye on the [release tests](https://github.com/RIOT-OS/RIOT/actions/workflows/release-test.yml) in the weeks up to the soft feature freeze.
      They might allow you to spot bugs the normal nightly builds do not reveal.
      They run automatically once a week on Saturday morning.
- [ ] Gather improvements and tips from previous release manager
- [ ] Decide and announce feature freeze dates
- [ ] Create milestone label :scream_cat:, and label PRs that have been already merged
- [ ] Gather features to push from contributors, and apply milestone label
- [ ] Go through list of deprecated features and make sure pending removals are applied
- [ ] Familiarise with release specs and tests, incl open issues, deprecations, api changes, and PRs

**Soft Feature Freeze**
- [ ] Post about soft feature freeze to the forum
- [ ] Check that there are no pending Rust changes (`find -name Cargo.toml -exec cargo update --manifest-path "{}" --package riot-wrappers --package riot-sys ";"` should be a no-op), otherwise poke Rust using maintainers
- [ ] Familiarise with nearly-merged, high impact PRs, and contact the contributors to ask them to hold

**Hard Feature Freeze**

Do the below actions iteratively, generating new release candidates, until all release specs tests pass.
- [ ] Generate branch and tag for release candidate :scroll:
- [ ] Add [branch protection rules](https://github.com/RIOT-OS/RIOT/settings/branches) for the release branch (if you don't have permissions ask an [admin](https://github.com/orgs/RIOT-OS/teams/admin) or [owner](https://github.com/orgs/RIOT-OS/teams/owners)). :warning: A new rule has to be added manually for each release, since the Github Merge Queue cannot be enabled for branches that are protected with a wildcard rule for some reason. You might need to ask for help from one of the RIOT-OS GitHub admins for that.
- [ ] Open issue in release specs repo for release candidate :scroll:
- [ ] Post about hard feature freeze to the forum
- [ ] Coordinate testing for release candidate
- [ ] Coordinate bugfixing for release candidate on master
- [ ] Backport bugfixes for release candidate to release branch

**Releasing**
- [ ] Create a GPG key and upload it to GitHub
- [ ] Generate changelog PR with list of new features, deprecations, api changes, and known issues, and share :scroll:
- [ ] Finalise release notes (incl backporting) :scroll:
- [ ] Create a VERSION file and raise PR against the release branch :scroll:. It is recommended to
      piggyback it with the release note backport to speed things up.
- [ ] Release either manually or using the release manager script :scroll:
- [ ] Inform about the release on the forum
- [ ] Update the [release statistics](https://github.com/RIOT-OS/RIOT/wiki/release-statistics)

## 2. Preparation

A good time to start preparing for the release is two months before the intended release date. Keep track of open pull requests and issues, especially of those marked with the Milestone label for the imminent release. This is a good time to clean up the GitHub repository and get rid of outdated, redundant or already fixed pull requests and issues.

Get in contact with the maintainers, discuss ongoing development and generate a **short** and realistic list of wanted features which should be published to the maintainers mailing list, on agreement. It is worth it to ask around for planned holidays to estimate an appropriate timetable. You should announce a date for the feature freeze and the estimated release date on all mailing lists.

Have a look at the [list of deprecated features](http://doc.riot-os.org/deprecated.html). If any of them are pending removal for the release (or previous releases) you should remove them or assign somebody to remove them.

During the month of release you should keep track of the development progress, coordinate between pull requests and help to get candidates merged. Shortly before feature freeze you should postpone pull requests that will (obviously) not make it until feature freeze. This SHOULD NOT be commented.

GitHub's [hub](https://github.com/github/hub) command line tool is useful to collect lists of PRs and issues.

Also review PRs tagged for the milestone that have not been updated in a long time. This state arises when a PR was tagged for a particular milestone, but was not merged in time and so the release manager tagged it for the _next_ milestone. As a guideline, if a PR already has been tagged for three milestones, then simply remove the milestone rather than advancing it to the next. After this removal, either the PR gets some attention or the stale bot eventually will close it.

## 3. Feature Freeze and Testing

There are two feature freezes: soft and hard.

Soft feature freeze is the date after which “high impact” PRs cannot be merged. What defines “high impact” is at the release manager’s discretion, but typically in the past this has meant large structural changes to common code or API changes to common code, with “common” meaning shared between the majority of boards and application scenarios.

Hard feature freeze is the date at which the release branch gets created. When hard feature freeze emerges there should be at most bugfix pull requests open with a label for this release. Ideally but not mandatorily, these get merged before the first release candidate goes out. Only bugfixes to address release specification test failures can be merged into the release branch after this date. Any PRs can be merged into RIOT master after this date.

Any bugfixes should be merged into RIOT master and backported into the release branch by creating an identical PR against it. The backport_pr tool in the RIOT repo can be used to do this automatically. To coordinate testing, you can use the checkboxes and the discussion in the release candidate's issue, and/or the release tracking spreadsheet, at your discretion. Once all bugfixes addressing test failures have been merged and backported, a new release candidate is generated and testing begins again. This happens iteratively until we get a release candidate which passes all the tests without any further bugfixes. As there are "always" known issues, and deadlines need to be satisfied, it is possible to abstain at some point from backporting and re-testing iteratively, and list the issue as known in the release notes.


### Automatic Freezing and Release Candidate Generation

First create a [classic Token](https://github.com/settings/tokens) and supply it to the `riot-release-manager` script with the `-t <token>` parameter.

**Feature Freeze:**
```bash
riot-release-manager feature-freeze yyyy.mm
```
**Generating a new Release Candidate:**
```bash
riot-release-manager rc yyyy.mm
```


### Manual Freezing and Release Candidate Generation

Even if you're using the release manager script, it's worth reading this section so you know what's going on.

Generate the release branch and git tags in the following way:
- Clone the RIOT repository via ssh: `git clone git@github.com:RIOT-OS/RIOT.git`
- Create a annotated tag that acts as starting point for the next release and a reference point for the `RIOT_VERSION` macro in master: `git tag -a -m "Development branch towards yyyy.mm release" <yyyy.mm-devel>` where the date should be the date of the **next** release
  (that tag's purpose is to be a base line for `git describe`)
- And push this tag via `git push origin <yyyy.mm-devel>`
- Checkout a new branch: `git checkout -b <yyyy.mm-branch>` where the date should be the date of the **current* release
- Push this branch to remote: `git push origin <yyyy.mm-branch>:<yyyy.mm-branch>`
(Please note that you need a [PubKey](https://github.com/settings/keys) assigned to your account and obviously you need to be a [RIOT maintainer](https://github.com/orgs/RIOT-OS/teams/maintainers))
- Next, create a (**lightweight**; so no `-a` parameter, otherwise the version string in master will point to this tag!) tag for the first release candidate: `git tag <yyyy.mm-RC1>`
- And push this tag via `git push origin <yyyy.mm-RC1>`

In the RIOT [Release Specs](https://github.com/RIOT-OS/Release-Specs/issues) repository create a new issue containing checkboxes for all release tests (have a look at closed issues from previous releases). A pointer to the release candidate and the issue for tracking testing efforts should be sent to the mailing lists with a request for help.

When generating a new release candidate, the release candidate needs to be tagged, a new issue needs to be created in the [Release Specs repository](https://github.com/RIOT-OS/Release-Specs/issues) and testing starts from the beginning (as described above).

## 4. Drafting Release Notes


### Automatic Draft Release Note Generation

**Setting Milestones :**
This allows the release-notes script to understand which PRs were merged for this release.
```bash
riot-release-manager set-milestones yyyy.mm --execute
```

**Draft Release Notes :**
```bash
riot-release-manager release-notes yyyy.mm
```

Then review the output to sort potential mismatch in categories where different PRs were placed by the script, and consider pruning some which might not be essential to report in the release notes.

A key part of the notes, however, is the selected highlights, which are not be generated automatically, and which you must draft manually.

### Manually Drafting Release Note

Obviously, you can also choose to do the above by hand.

## 5. Actual Release

Once a release candidate passed all tests you are ready to create the release notes. Open an etherpad and list new features, major improvements and features as well as known issues as done in [previous releases](https://github.com/RIOT-OS/RIOT/blob/master/release-notes.txt). Make sure the line length is about 90 characters. You should share this pad with other maintainers and give them some time for review. Alternatively you can open a pull request with your notes and discuss there. But the former has proven well so far.

If you choose to prepare a first draft of new features and changes, GitHub's [hub tool](https://github.com/github/hub) can help. The command `hub pr list -s "merged" -b "master" -L 6000 -f "%Mt %t %L%n "` collects the last 6000 merged PRs, where each line begins with the milestone, and includes the labels. You can grep out the lines for the release, and search for the various _Area_ labels.

For the Known Issues section, use the _dump-issues_ target of `riot-release-manager`. This tool uses the "Type: bug" label to identify a known issue. If you decide to edit the list of known issues, keep this heuristic is in mind: "A big enough issue, that might be of interest to people and should be mentioned" (a "Known issue") vs. "Overcrowding the release notes with tiny bugs that, yes, should be fixed, but don't ring any alarm bells" (so those don't go into the "Known issues" section). (See the [source](https://github.com/RIOT-OS/RIOT/pull/12192#issuecomment-532609660) for more context.)

Now the notes need to go into the [release-notes](https://github.com/RIOT-OS/RIOT/blob/master/release-notes.txt) archive and must be appended on RIOT master as well as on the release branch. Both can be done via pull requests.
The list of contributors can be deduced from `git shortlog --no-merges -n -s OLD_RELEASE..RELEASE_BRANCH  | cut -f 2 | sort`  after removing duplicates.

Create a `VERSION` file in the base of the repository with the release version with the following content and do a PR on the release branch.

    RIOT_VERSION = YYYY.MM

We maintain a Release statistic where you should add numbers from the previously created release. To get these numbers in a formatted way, simply execute [`./dist/tools/release-stats/release-stats.sh <new> [<old>]`](https://github.com/RIOT-OS/RIOT/blob/master/dist/tools/release-stats/release-stats.sh), where `<new>` is the tag for the release and `<old>` is unnecessary. Copy and paste the outcome to the table on our [Release statistics](https://github.com/RIOT-OS/RIOT/wiki/release-statistics) wiki page. You also can use the script to help write the release notes before the release, where `<new>` is the tag for the last RC and `<old>` is the 'devel' tag that started this release.

After you have released, be sure to inform all RIOTers about the release via mail on all lists and RIOT forum (under Announcements).

### Automatic Releasing

Run the command

```bash
riot-release-manager release yyyy.mm
```

### Manual Releasing

Again, it's worth reading this section even if you're using the release manager tool.

Tag this release as you did for the release candidate, except that you should sign it (`git tag -s yyyy.mm`). Only tag it `yyyy.mm` this time. Additionally, to assure our users that the release wasn't messed with by third parties it also needs to be [signed with GPG](https://git-scm.com/book/en/v2/Git-Tools-Signing-Your-Work). You might also need to configure your [`gpg.program`](https://git-scm.com/docs/git-config#Documentation/git-config.txt-gpgprogram) if you are using any other command for GPG than `gpg` (e.g. `gpg2` on newer Ubuntu versions). For the signature to be visible in GitHub it is advised that you also [upload your GPG public key to GitHub](https://help.github.com/articles/adding-a-new-gpg-key-to-your-github-account/).

Now that everything is ready, create the release. This can easily be done via GitHubs web interface. In the [RIOT](https://github.com/RIOT-OS/RIOT) repository click on *Releases->Draft a new release*. Assign the tag you previously created and signed, set title `RIOT-<yyyy-mm> and paste the release notes below. Et voilà, that's the release.

## 6. Other Repositories

Don't forget to update the submodule in the [applications repository](https://github.com/RIOT-OS/applications) and [Tutorials repository](https://github.com/RIOT-OS/Tutorials) and test if the applications still work.

## 7. Important Fixes after Release

After the release, issues may arise that are important enough to warrant an update to the release. As a first step, such issues should be backported to the release branch. If a single issue represents a critical fix, or if a number of lesser backported issues accumulate, then a bugfix or point release should be created.

For example, after the 2019.10 release [#12653](https://github.com/RIOT-OS/RIOT/issues/12653) was identified. This issue was a failure to execute the 'make debug' target for sam*-xpro and arduino zero boards. We created a backport, but did not create a point release. However, after the 2018.10 release, [#10739](https://github.com/RIOT-OS/RIOT/issues/10739) was identified. This issue was a serious security issue that allowed DNS spoofing. So along with other issues, we released [2018.10.1](https://github.com/RIOT-OS/RIOT/releases/tag/2018.10.1).

A point release for a critical issue is best limited to a small number of changes to the original release to reduce the amount of testing required and also reduce the introduction of new bugs. We do not have a policy on who should manage a point release, but the previous release manager at least should consult on the point release since they are most familiar with the previous release.

## 8. Resources

| Description | Location |
|-------------|----------|
| Release spec test repository | https://github.com/RIOT-OS/Release-Specs |
| Release manager tool | https://github.com/RIOT-OS/RIOT-release-manager |
| Test automation helper scripts | https://github.com/RIOT-OS/Release-Specs?tab=readme-ov-file#pytest-runner |
| Pending changes to test automation helper scripts | https://github.com/jia200x/Release-Specs/pulls |
| Backport automation script | {RIOTBASE}/dist/tools/backport_pr |
| Release statistics wiki | https://github.com/RIOT-OS/RIOT/wiki/release-statistics |
| Release stats script | {RIOTBASE}/dist/tools/release-stats |
| Release management org spreadsheet | https://drive.google.com/open?id=1rKYswFgQGYj4jSA4TBHjg980pAGoaKPj |
| Release test tracking spreadsheet | https://drive.google.com/open?id=0B384VtEXbD_HRzJSY1NGdnFpWERxb2JFeGdaS09iUjV0TGhN |


## 9. Forum Post Templates

These templates are suggestions, if useful. Whatever the phrasing, posts with similar content should be made to the forum at various stages of the release process.

### Date Announcement and Feature Request
**Subject**
Release [YYYY.MM] - dates and feature requests

**Body**
```
Dear RIOTers,


The release dates for the upcoming release cycle are fixed as follows:

- [Date] - soft feature freeze, for high impact features

- [Date] - hard feature freeze, for all features

- [Date] - Release date

Could you please send your suggestions for features which you would like to see merged during this release cycle.


Best regards, and happy hacking!

[Name]
```

### Soft Feature Freeze Announcement
**Subject**
Release [YYYY.MM] - Soft feature freeze now effective

**Body**
```
Dear RIOTers,

The soft feature freeze (for high impact PRs) is now effective.

As a gentle reminder: the final feature freeze (for all PRs) is [DATE OF HARD FEATURE FREEZE].

Wishing you a happy code/polish/review/merge sprint,
[Name]
```

### Hard Feature Freeze Announcement
**Subject**
Release [YYYY.MM] - Hard feature freeze now effective

**Body**
```
Dear RIOTers,

We've created the [YYYY.MM] release branch, so we are now officially in
feature freeze. This means from now only bugfixes will be backported to
the `[YYYY.MM]-branch`. If anyone has any bugfixes or documentation
improvements that they feel should be part of this release please let me know.

New features can still be merged into master during this period.

The test tracking issue of release candidate 1 [1] is opened. Help with
testing and resources for the testing is as always very welcome. Most of
the testing is automated [2], but some stuff still needs to be run manually.
So if you want to help, please put your name after the corresponding item
in the linked issue to streamline the testing efforts.

Thanks all for the awesome work!

Best regards,

[Name]

[1] https://github.com/RIOT-OS/Release-Specs/issues/[XX]
[2] https://github.com/RIOT-OS/Release-Specs#pytest-runner
```

#### Release Announcement
**Subject**
Release [YYYY.MM]

**Body**
```
Dear RIOTers,

We are happy to announce the [XX]th official release of RIOT:

----------------------- * RIOT [YYYY.MM] * -----------------------

[Paste the “about the release” text here]

You can download the RIOT release from Github by cloning the repository
and checkout the release tag [1] or by downloading the tarball [2], and
look up the release notes for further details [3].

A big thank you to everybody who contributed!

Best regards,
[Name]


[1]:https://github.com/RIOT-OS/RIOT/tree/[YYYY.MM]

[2]:https://github.com/RIOT-OS/RIOT/archive/[YYYY.MM].tar.gz

[3]:https://github.com/RIOT-OS/RIOT/releases/tag/[YYYY.MM]
```
