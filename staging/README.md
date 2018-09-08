# Introduction

Welcome to the staging tree!

This tree contains experimental code that does not (yet) meet all the quality
requirements of RIOT.

The idea of this tree is to provide a place to code that offers functionality,
but has not (yet) received the polishing and refinement that is usually
required in RIOT. The staging/ directory is also a place where new ideas can be
shared early with a wider audience in order to facilitate easier collaboration.

**WARNING**

Do not depend on anything within this subfolder! Everything in here might be
broken or break or be changed or removed at any time, and everything is
officially unsupported (but may be supported by one or more individuals)! API's
from this tree might change without notice.

## How to use

Please re-read the warning above.
If you still feel brave enough to use code contained in here, add

    ENABLE_STAGING=1

to your application's Makefile.

## Terminology

"main tree" is used to describe the regular RIOT sources (everything outside
staging/).

"broken" means, it doesn't compile or work anymore.

## Rules

- no dependencies from main tree into staging are allowed
- commits MUST NOT change files both in and outside of staging/
- if anything in staging/ breaks due to a change in master, it will be marked
  as broken
- the original contributor(s) is/are considered maintainer(s) for a module
- non-trivial changes SHOULD be ACK'ed by a module's maintainer(s)
- PR's to staging/ are labeled with "staging"
- PR's changing both staging and main tree code are *not* labeled "staging"
- each staging module has a README.md as described below

## Procedures

### Reviewing a change to staging/

Make sure above rules are followed. For new contributions, only accept code
that has at least a remote chance of getting merged into the main tree at some
point, e.g., filter out nonsense contributions. Unless the contribution
consists of a header-only API that's in development, check that all code gets
compiled by an accompanying test application.

### Mark as "broken"

Modules can be marked as broken in staging/staging.mk.
Add a comment with a date and a reason, e.g.,

    BROKEN += heart   # 2018/12/24: broken through change in friend API (#12345)

This will disable the CI for the module and every module that depends on it.

If a contribution to the main tree fails to build because of a module in
staging/, it is perfectly valid to mark that module as "broken".
This is meant to prevent code from the staging tree (which might not be
optimally integrated) from blocking the main tree from advancing. Ideally, if
possible, try to update staging/ code along with main tree code, but if that
shows to be non-trivial for a given PR, it is alright to mark the difficult
staging code as "broken".
If someone cares enough, the staging code will be fixed eventually. If not, it
will be removed from the tree (see also "Removal from staging").

### README.md

Each module in staging/ *must* come with a README.md that outlines what the
module can do and what's missing. If possible, also outline what is the mode in
which the module will be developed.

For example, write "This is a driver for device foo. It works with a Nucleo
foobar, but didn't get much testing, and only mode 42 is implemented. Please
help improving.

### Advance to main tree

If a module in staging/ has matured enough to be deemed fit for the main tree,
anyone can open a PR that moves the corresponding code out of staging/. At this
point, a full review of the code has to be made.
The PR SHOULD do the move in a single commit that only contains the file moves
(possibly after other preparatory commits).

### Removal from staging

During the final stages of a release, the release manager creates a PR removing
all modules that have been marked "broken" for more than two release cycles.
