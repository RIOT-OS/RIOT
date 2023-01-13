Continuous Integration for RIOT {#riot-ci}
===============================

[TOC]

To ensure the functionality of RIOT on its [large number of supported hardware platforms][boards]
and high quality of code, the RIOT community deploys several means of continuous integration (CI)
and testing platforms.

- [Murdock] is the main CI system for testing pull requests (PRs) and nightly builds of the master
  branch
- [RIOT HiL] provides a subset of dedicated nightly tests for our Hardware in the Loop (HiL)
- Several [GitHub Action Workflows]
  - [check-commits] checks commit messages of PRs
  - [check-labels] checks the labels of PRs to prevent accidental merges of PRs marked as unfit to
    merge
  - [pr-labeler] triages PRs by labeling them with fitting `Area:` and `Platform:`
    labels according to their code content
  - [static-tests] run the `make static-tests` target on every PR and the master branch
  - [tools-buildtest] does build tests for various helper tools provided in `dist/tools` on every
    PR, the master branch, and release tags
  - [tools-test] tests various helper scripts and libraries provided in `dist/tools` and
    `dist/pythonlibs` on every PR, the master branch, and tags
  - [release-tests] test our [release specifications] weekly and on every release candidate and
    release tag
  - [test-on-iotlab] runs the script
    [`dist/tools/compile_and_test_for_board/compile_and_test_for_board.py`][compile_and_test_for_board]
    on the boards provided in the [FIT IoT-Lab] testbed
  - [test-on-ryot] runs the script
    [`dist/tools/compile_and_test_for_board/compile_and_test_for_board.py`][compile_and_test_for_board]
    on boards connected to a self-hosted [RYOT] runner

# Murdock
Murdock runs `make all` for all applications for all boards that support this application.
[bors] is used with Murdock to combine multiple PRs to be merged into one test.
This both speeds up the integration process and prevents [semantic merge conflicts].
The full list of applications that are tested can be retrieved with the

~~~~~~~~~~~~~~~~~ {.sh}
make info-applications
~~~~~~~~~~~~~~~~~

Make target. The list of boards that support an application can be retrieved with the

~~~~~~~~~~~~~~~~~ {.sh}
make -C "<application directory>" info-boards-supported
~~~~~~~~~~~~~~~~~

The `make test` target is run for all applications on the [`native board`](@ref boards_native) and
for a selected number of boards attached to the [Pi fleet] during the nightlies and when the
`CI: run tests` label is set for a PR under test.

If the `CI: skip compile test` label is set on a PR, both `make all` and `make test` will be skipped
for all boards and applications.

`.murdock` and `.murdock.yml`... TBD

Restricting builds ... TBD

Nightlies ... TBD

Murdock runs in two instances:

- https://ci.riot-os.org runs [Murdock](https://github.com/kaspar030/murdock) for all
  applications and boards
- https://ci-staging.riot-os.org is a staging instance to test new features of [Murdock]

# RIOT HiL

The [RIOT HiL] CI tests a subset of supported RIOT boards connected to
external peripheral testing hardware ([PHiLIP]). Nightly tests are run
on all Robot Framework tests on all available boards. Specific boards
and both Robot Framework tests or RIOT tests can be executed via RIOT
HiL maintainers. These tests focus on the physical aspect of peripheral
interaction, testing peripherals such as SPI, I2C, UART, etc.

- [RIOT HiL Jenkins] is used for the coordination of the tests
- [Robot Framework tests] is the test repo
- List of [RIOT HiL supported boards]
- [RIOT HiL] nightly results
- Information about [PHiLIP]

# GitHub Action Workflows

TBD

## check-commits

The [check-commits] workflow checks the commit messages of a PR for two faults:

1. The length of the summary line not adhering to the [50/72 rule] (`check-commits [commit-msg]`)
   is checked using the [commit-msg script].
2. The commit message summary containing certain keywords that indicate the PR is currently
   containing fix-up or squashable commits. It is using the [pr\_check script].
   If the commit message starts with either of the following (case insensitive) regex pattern, the
   workflow will fail:
   - `SQUASH`
   - `FIX`
   - `REMOVE *ME`
   - `Update`

## check-labels

The [check-labels] workflow checks for labels and review states that indicate that a PR is not
mergeable yet. It uses the [check-labels-action] to do this. See the [check-labels YAML
file][check-labels-file] for which label conditions are handled by this workflow.

## pr-labeler

The [pr-labeler] workflow labels a PR with `Area:` and `Platform:` labels according the files
changed in the PR using the [labeler action].
The mappings can be found in the in the [labeler config] file.

## static-tests

The [static-tests] workflow runs several linters, style checkers and spell checkers via the
[static\_tests script] using the `make static-tests` target for each PR and on merges to the master
branch. It includes the following checks:

- `whitespacecheck`, using [the whitespace check of git][git-whitespace].
- `licenses`, checks if changed files contain a proper license header.
- `doccheck` runs [Doxygen] on the code and errors if any non-excluded warnings are returned.
- `externc` checks if C header files contain the `extern "C"` statements for C++ compatibility.
- `cppcheck` runs the [CppCheck] style checker on the code (currently deactivated).
- `vera++` runs the [Vera++] style checker on the code.
- `coccinelle` runs the [Coccinelle] program matching and transformation engine on the code.
- TBD

## tools-buildtest

## tools-test

## release-tests

## test-on-iotlab

## test-on-ryot


[boards]: @ref boards
[Murdock]: https://ci.riot-os.org
[RIOT HiL]: https://hil.riot-os.org
[PHiLIP]: https://philip.riot-apps.net/
[RIOT HiL supported boards]: https://hil.riot-os.org/jenkins/labelsdashboard/
[RIOT HiL Jenkins]: https://hil.riot-os.org/jenkins/
[Robot Framework tests]: https://github.com/RIOT-OS/RobotFW-tests
[GitHub Action Workflows]: https://github.com/RIOT-OS/RIOT/actions
[check-commits]: https://github.com/RIOT-OS/RIOT/actions/workflows/check-commits.yml
[check-labels]: https://github.com/RIOT-OS/RIOT/actions/workflows/check-labels.yml
[pr-labeler]: https://github.com/RIOT-OS/RIOT/actions/workflows/labeler.yml
[static-tests]: https://github.com/RIOT-OS/RIOT/actions/workflows/static-test.yml
[tools-buildtest]: https://github.com/RIOT-OS/RIOT/actions/workflows/tools-buildtest.yml
[tools-test]: https://github.com/RIOT-OS/RIOT/actions/workflows/tools-test.yml
[release-tests]: https://github.com/RIOT-OS/RIOT/actions/workflows/release-test.yml
[test-on-iotlab]: https://github.com/RIOT-OS/RIOT/actions/workflows/test-on-iotlab.yml
[test-on-ryot]: https://github.com/RIOT-OS/RIOT/actions/workflows/test-on-ryot.yml
[release specifications]: https://github.com/RIOT-OS/Release-Specs
[compile_and_test_for_board]: https://github.com/RIOT-OS/RIOT/tree/master/dist/tools/compile_and_test_for_board
[FIT IoT-Lab]: https://www.iot-lab.info
[RYOT]: https://github.com/fjmolinas/riot-ryot
[Murdock NG]: https://github.com/murdock-ng/murdock
[bors]: https://bors.tech
[semantic merge conflicts]: https://bors.tech/essay/2017/02/02/pitch/
[Pi fleet]: https://github.com/kaspar030/pifleet-minion
[50/72 rule]: https://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html
[commit-msg script]: https://github.com/RIOT-OS/RIOT/blob/master/dist/tools/commit-msg/check.sh
[pr\_check script]: https://github.com/RIOT-OS/RIOT/blob/master/dist/tools/pr_check/check.sh
[check-labels-action]: https://github.com/RIOT-OS/check-labels-action
[check-labels-file]: https://github.com/RIOT-OS/RIOT/blob/master/.github/workflows/check-labels.yml
[labeler action]: https://github.com/actions/labeler
[labeler config]: https://github.com/RIOT-OS/RIOT/blob/master/.github/labeler.yml
[static\_tests script]: https://github.com/RIOT-OS/RIOT/blob/master/dist/tools/ci/static_tests.sh
[git-whitespace]: https://www.git-scm.com/docs/git-config#Documentation/git-config.txt-corewhitespace
