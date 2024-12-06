Release cycle                                                   {#release-cycle}
=============

RIOT has a new release every three months and is named after the month it
was feature frozen in. E.g. `2021.01` was feature frozen in January 2021.
Feature freeze means the branch off point of the `<YYYY.MM>-branch` with
`<YYYY.MM>` being the releases name. As such, any new feature merged into the
`master` after that point, will be part of the next release. That branch, which
we will call the release branch in the following is then the stable branch of
RIOT.

After feature freeze, the current release candidate in the release branch is
tested heavily. For more information on the release testing, have a look at our
[release specifications][release specs]. Bug fixes that are made during this
testing period are back ported from `master` to the release branch and a new
release candidate might be created when a certain milestone is found. If all
major bugs are fixed, the new RIOT release is signed off by the appointed
release manager, usually within 2 weeks after the feature freeze.
More details and instructions for release managers can be found in the
[managing a release guide].

RIOT follows a rolling release cycle, meaning, that support and bug fixes are
only provided for the most current release.

Download a release                                                   {#download}
==================

You can download the source code of our releases [via Github][releases] as ZIP
file or tarball. Alternatively, you can check them out if you already cloned
RIOT with Git:

~~~~~~~~~~~~~~~~~~~~
$ git pull --tags
$ git checkout <YYYY.MM>
~~~~~~~~~~~~~~~~~~~~

Point releases and hot fixes                                   {#point-releases}
============================

For major bug fixes, we may provide a point release `YYYY.MM.N` which results in
a new ZIP file or tarball over at the [release page][releases]. However, minor
bug fixes are only pushed to the release branch. You can fetch that via Git
using

~~~~~~~~~~~~~~~~~~~~
$ git clone -b `<YYYY.MM>-branch` https://github.com/RIOT-OS/RIOT
~~~~~~~~~~~~~~~~~~~~

or

~~~~~~~~~~~~~~~~~~~~
$ git pull origin <YYYY.MM-branch>:<YYYY.MM-branch>
$ git checkout <YYYY.MM-branch>
~~~~~~~~~~~~~~~~~~~~

if you have RIOT already cloned.

[releases]: https://github.com/RIOT-OS/RIOT/releases
[release specs]: https://github.com/RIOT-OS/Release-Specs
[managing a release guide]: https://github.com/RIOT-OS/RIOT/tree/master/doc/guides/managing-a-release
