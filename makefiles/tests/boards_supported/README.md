Simple Unit Tests for `make info-boards-supported`
==================================================

This folder contains a Makefile that runs a set of unit tests for the logic that figures out which
boards are supported. In the past there have been some issues with this despite the feature
resolution for a single board worked fine, so it is worth testing this functionality. This is
especially true since this list is used by the CI to check which boards to build for.

Right now, only a single test case is added: It will run the logic behind
`make info-boards-supported` without any modules used other than the default modules and subtracts
the result from the list of all available boards. The resulting difference is the set of boards
which will never be build by the CI - not even for `examples/essentials/hello-world`. If this result is empty,
the test succeeds. Otherwise the list of never build boards will be printed and the test fails.

It is intended that some more advanced unit tests will be added later on.
