RIOT Maintainers Community
Developer Memo: 4
Author(s): Martine Lenders
Date: January 2017

Writing tests
=============

## Abstract
This memo describes how to write tests for RIOT.


## Status
This document is a product of the community of RIOT maintainers, and aims to 
represent the consensus within this community.
The content of this document is licensed with a Creative Commons CC-BY-SA 
license.

## Terminology
This memo uses the following terminology and acronyms:

- embUnit: a unittest framework for C that is used by RIOT to write unittests.
- pexpect: a python library to interact with the stdio of command-line
  application.
- CI system: CI = Continuous Intergration, platform for automated building and
  testing of provided changes.

## 1. Introduction
At the moment of writing there are two strategies for testing: providing a
unittest suite in [`tests/unittests/`][unittests] or a stand-alone test
application in `tests/` directly. This document provides some recommendations
on where to put a test and what the minimum requirements for those are.

## 2. Unittests
Unittests in RIOT are provided using a costumized fork of the [embUnit]
framework. While it is possible to provide a stand-alone application using this
framework it is recommended to add or extend a test suite in the [unittests]
application (see [unittests README] for how to do that).

The advantage of that is that this application is at the time of writing this
document this application is the only one executed by the CI system used by
RIOT. Even if more applications will be added to that at a later point,
providing all unittests in one application is still preferred since it reduces
the number of required builds (resulting in lower build times).

For these reasons it is always preferred to provide unittests with a sufficient
coverage except for reasons discussed in the following section.

## 2. Stand-alone test application
There are a few reasons that speak against providing a test in the [unittests]
application:

- Unittests are not well suited to test interaction with external entities
  (other nodes, measuring devices, users, or even other threads)
- States of a component can only be reseted by restarting the node (e.g. for
  testing the initialization routine of an interface), which is not supported by
  the current embUnit fork (especially when it comes to continuing further tests
  after the reset).

If one of these reasons apply it is valid to provide a stand-alone test
application. If one part of the tested component is applicable for unittests,
while others are not, it is also possible to provide tests [unittests] for the
applicable components and one or multiple stand-alone tests for the others.

A stand-alone test application should be build-able for all supported platforms
and, if they fit, also be usable on these platforms. To simplify and streamline
testing efforts for other developers it is also recommended to provide a
comprehensive [pexpect] script that may utilize the [testrunner] library.

## 3. Examples
In the RIOT source tree there is also an `examples/` directory. This contains
applications that exemplify features of RIOT. These applications are supposed to
be well documented and should show-case features ideally very concentrated (i.e.
as few features as possible - ideally one - are show-cased in the application).
If an example is extensive enough to provide an acceptable level of coverage,
they can be provided alternatively to a stand-alone test application, but it
shall not be main motivation to provide an example or for a show-case in an
example.

To allow for easy testing of an example it is recommended to provide a
comprehensive [pexpect] script that may utilize the [testrunner] library.

[unittests]: https://github.com/RIOT-OS/RIOT/tree/master/tests/unittests
[unittests README]: https://github.com/RIOT-OS/RIOT/blob/master/tests/unittests/README.md
[embUnit]: http://embunit.sourceforge.net/embunit/
[pexpect]: http://pexpect.readthedocs.io/en/stable/
[testrunner]: https://github.com/RIOT-OS/RIOT/blob/master/dist/tools/testrunner/testrunner.py
