# About

This script checks if a Pull Request needs squashing.

# Usage

```bash
./check.sh [<branch>]
```

The `<branch>` parameter refers to the branch the pull request's branch branched
from. The script will output all commits marked as squashable from `HEAD` to the
merge-base with `<branch>`. The default for `<branch>` is `master`.

A commit is marked as squashable if it contains the keywords SQUASH, FIX or
REMOVE ME (case insensitive) within the first five characters of it's subject
title.
