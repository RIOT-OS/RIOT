---
title: Continuous Integration for RIOT
description: Tips and tricks working with the automatic build system
---

To ensure the functionality of RIOT on its [large number of supported hardware platforms][boards]
and high quality of code, the RIOT community deploys several means of continuous integration (CI)
and testing platforms.

The RIOT buildsystem has two general components: [Murdock] and
[GitHub Action Workflows]. Murdock is the main CI system for building and
testing pull requests (PRs) and nightly builds of the master branch as well as
release branches.

The GitHub Action Workflows run a number of tests and checks on each PR
before executing the compilation and testing on Murdock to reduce the limited
computing power available.

:::note
All the computing power available for Murdock is donated by kind supporters
of the RIOT OS project. Please use it carefully and responsibly!
:::

[boards]: https://guides.riot-os.org/boards/
[Murdock]: https://ci.riot-os.org
[GitHub Action Workflows]: https://github.com/RIOT-OS/RIOT/actions

## Murdock
Murdock is the custom Continuous Integration System for RIOT OS.
The integration with GitHub is quite seamless for the user and after the
first build has been completed, a status message will be posted to the
Pull Request by the `riot-ci` bot.

This status message has a summary of failed and successful build jobs as well
as the execution time and a link to the latest build job.

You can access the list of all build jobs on https://ci.riot-os.org.

There is a second instance of Murdock deployed on
https://ci-staging.riot-os.org that can be used for changes and updates to
Murdock itself.

The sources of Murdock can be found in its GitHub repository:
https://github.com/murdock-ng/murdock

### GitHub Labels to control Murdock
When opening a Pull Request, the build process does not start automatically.
A maintainer has to manually set the labels after giving the Pull Request
a quick look to check if it is safe to run and actually makes sense.

#### CI: ready for build
The `CI: ready for build` label starts the build process on Murdock. This
will only build the code for a subset of boards to save computing resources.
The selected boards are supposed to represent a crosssection of all supported
boards to detect potential issues early.

The [`dist/tools/ci/can_fast_ci_run.py`][fast-ci-run] script determines which
code areas were changed by the PR and potentially limits the build scope even
further, e.g. for a newly added board, only the new board will be built.

#### CI: full build
To enable a full build that is identical to a build that would be executed
before a merge, a maintainer can set the `CI: full build` label. This will
build *every* application for *every* board and takes ***A LOT*** of resources.
This label should only be used very sparingly, for example if a previous merge
failed and a second attempt is likely to fail again.

After the full build has been completed, the label should be removed again!

#### CI: no fast fail
The build process stops after the first failed build. This is done to save
computing resources and avoid unnecessarily and unhelpfully long failure logs
on systematic errors.

Sometimes it is helpful though to see exactly what will fail after the first
failed build, so the `CI: no fast fail` can be set. This raises the abort
limit from 1 to 500 failed builds.

#### CI: skip compile test
For changes that only affect e.g. the documentation, it is unnecessary to
build any code at all. To avoid building code (if the `can_fast_ci_run.py`
script did not classify the changes already), the `CI: skip compile test`
label can be set.

:::note
This label has no influence on the merge build. If the `can_fast_ci_run.py`
script decided that a full build is required, a full build will be performed
for the merge.
:::

#### CI: run tests
To explicitly run the hardware tests on the [Pi fleet] for a Pull Request,
the `CI: run tests` label can be set.

Otherwise, the tests will only be run when the next Nightly build is done.

[fast-ci-run]: https://github.com/RIOT-OS/RIOT/blob/master/dist/tools/ci/can_fast_ci_run.py
[Pi fleet]: https://github.com/kaspar030/pifleet-minion

### Nightly Builds
To be extended in the future.

### Murdock Scripts and Configuration
To be extended in the future, `.murdock` and `.murdock.yml`.

### Donating Computing Power
To be extended in the future.

### Adding a Node for Hardware in the Loop Testing
To be extended in the future.

## GitHub Action Workflows
GitHub provides computing resources for Open Source projects that can be used
to run various tasks and tests. Usually the workflows check if the code is
compilable and adheres to the general conventions set by a project.

Specifically, the following workflows are executed either on a scheduled basis
or when a new Pull Request is opened.

### check-commits
The [check-commits] workflow checks the commit messages of a PR for two faults:

1. The [commit-msg script] (called `check-commits [commit-msg]` in the check
   summary) checks if the length of the summary line adheres to the [50/72 rule].
2. The [pr\_check script] (called `check-commits [pr_check]` in the summary)
   checks if the summary contains certain keywords indicating that the
   PR is not ready for merge yet, for example temporary or fix-up commits.
   It will check against a list of **No Merge Keywords** stored in
   [`dist/tools/pr_check/no_merge_keywords`][no-merge-keywords].
3. The `check-commits-success` workflow summarizes the results of the previous
   two workflows and provides the blocking mechanic if one of them fails.

### check-labels
The [check-labels] workflow checks for labels and review states that indicate
that a PR is not mergeable yet. It uses the [check-labels-action] to do this.
See the [check-labels YAML file][check-labels-file] for which label conditions
are handled by this workflow.

### deploy-guides
The [deploy-guides] workflow pushes the updated Starlight documentation to the
RIOT OS guide server after each merge.

### first-contribution
The [first-contribution] workflow checks if a PR or an issue has been opened
by a first time contributor and prints additional information to help the new
contributor getting along with the RIOT contribution process.

### labeler
The [labeler] workflow labels a PR with e.g. `Area:` and `Platform:` labels
according the files changed in the PR using the [labeler action].
The mappings can be found in the in the [labeler config] file.

### pr-ai-disclosure
The [pr-ai-disclosure] workflow checks that the AI disclosure section is present
on new PRs. If it is not present, it will add a comment on the PR to inform
the author, and add appropriate labels.


### release-test
The [release-test] workflow runs our [release specifications] weekly and on
every release candidate and release tag. This ensures that the latest Release
still builds while it is supported, even when changes to the CI infrastructure
are made or tools are updated.

### static-test
The [static-test] workflow runs several linters, style checkers and spell
checkers via the [static\_tests script] using the `make static-test` target
for each PR and on merges to the master branch. It includes (but is not
limited to) the following checks:

- `whitespacecheck` using [the whitespace check of git][git-whitespace].
- `licenses` checks if changed files contain a proper license header.
- `doccheck` runs [Doxygen] on the code and errors if any non-excluded warnings
  are returned.
- `externc` checks if C header files contain the `extern "C"` statements for
  C++ compatibility.
- `vera++` runs the [Vera++] style checker on the code.
- `coccinelle` runs the [Coccinelle] program matching and transformation
  engine on the code.
- ...

### sync-codeberg
The [sync-codeberg] workflow synchronizes the codebase from GitHub to Codeberg.
Currently the integration with Codeberg is a one-way mirror until the
migration effort continues.

### test-guides
The [test-guides] workflow builds the Starlight documentation to see if a PR
introduces errors or breaking changes. This workflow currently does not provide
a guide preview.

### test-on-iotlab
The [test-on-iotlab] workflow runs the
[`dist/tools/compile_and_test_for_board/compile_and_test_for_board.py`][compile_and_test_for_board]
script on the boards provided in the [FIT IoT-Lab] testbed to ensure that the code
still works on actual hardware. This workflow only covers a limited number
of tests and boards.

### tools-buildtest
The [tools-buildtest] workflow does build tests for various helper tools
provided in `dist/tools` on every PR, the master branch itself as well as
release tags.

### tools-test
The [tools-test] workflow tests various helper scripts and libraries provided
in `dist/tools` and `dist/pythonlibs` on every PR, the master branch, and tags.
The scripts tested in `tools-test` do not have to be compiled, contrary to
the `tools-buildtest`.

[check-commits]: https://github.com/RIOT-OS/RIOT/actions/workflows/check-commits.yml
[check-labels]: https://github.com/RIOT-OS/RIOT/actions/workflows/check-labels.yml
[deploy-guides]: https://github.com/RIOT-OS/RIOT/actions/workflows/deploy-guides.yml
[first-contribution]: https://github.com/RIOT-OS/RIOT/actions/workflows/first-contribution.yml
[labeler]: https://github.com/RIOT-OS/RIOT/actions/workflows/labeler.yml
[pr-ai-disclosure]: https://github.com/RIOT-OS/RIOT/actions/workflows/pr-ai-disclosure.yml
[release-test]: https://github.com/RIOT-OS/RIOT/actions/workflows/release-test.yml
[static-test]: https://github.com/RIOT-OS/RIOT/actions/workflows/static-test.yml
[sync-codeberg]: https://github.com/RIOT-OS/RIOT/actions/workflows/sync_codeberg.yml
[test-on-iotlab]: https://github.com/RIOT-OS/RIOT/actions/workflows/test-on-iotlab.yml
[test-guides]: https://github.com/RIOT-OS/RIOT/actions/workflows/test-guides.yml
[tools-buildtest]: https://github.com/RIOT-OS/RIOT/actions/workflows/tools-buildtest.yml
[tools-test]: https://github.com/RIOT-OS/RIOT/actions/workflows/tools-test.yml

[commit-msg script]: https://github.com/RIOT-OS/RIOT/blob/master/dist/tools/commit-msg/check.sh
[Doxygen]: https://github.com/RIOT-OS/RIOT/blob/master/dist/tools/doccheck/check.sh
[Vera++]: https://github.com/RIOT-OS/RIOT/tree/master/dist/tools/vera%2B%2B
[Coccinelle]: https://github.com/RIOT-OS/RIOT/tree/master/dist/tools/coccinelle
[50/72 rule]: https://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html
[pr\_check script]: https://github.com/RIOT-OS/RIOT/blob/master/dist/tools/pr_check/check.sh
[check-labels-action]: https://github.com/RIOT-OS/check-labels-action
[no-merge-keywords]: https://github.com/RIOT-OS/RIOT/blob/master/dist/tools/pr_check/no_merge_keywords
[check-labels-file]: https://github.com/RIOT-OS/RIOT/blob/master/.github/workflows/check-labels.yml
[release specifications]: https://github.com/RIOT-OS/Release-Specs
[labeler action]: https://github.com/actions/labeler
[labeler config]: https://github.com/RIOT-OS/RIOT/blob/master/.github/labeler.yml
[static\_tests script]: https://github.com/RIOT-OS/RIOT/blob/master/dist/tools/ci/static_tests.sh
[git-whitespace]: https://www.git-scm.com/docs/git-config#Documentation/git-config.txt-corewhitespace
[compile_and_test_for_board]: https://github.com/RIOT-OS/RIOT/tree/master/dist/tools/compile_and_test_for_board
[FIT IoT-Lab]: https://www.iot-lab.info
