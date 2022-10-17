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
  - [check-pr] checks PR labels and commit messages of PRs to prevent accidental merges of PRs
    marked as unfit to merge
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

Murdock currently runs in three instances:

- https://ci.riot-os.org runs [legacy Murdock](https://github.com/kaspar030/murdock) for all
  applications and boards
- https://ci-prod.riot-os.org runs [Murdock NG] for all
  applications but only a representative selection of boards. In the long run ci-prod.riot-os.org
  will replace ci.riot-os.org.
- https://ci-staging.riot-os.org is a staging instance to test new features of [Murdock NG]

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

## check-pr

## pr-labeler

## static-tests

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
[check-pr]: https://github.com/RIOT-OS/RIOT/actions/workflows/check-pr.yml
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
[Pi fleet]: https://github.com/kaspar030/pifleet-minion
