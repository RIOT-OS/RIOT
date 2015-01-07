# About

This script checks if a Pull Request needs squashing or if it is waiting for
another Pull Request.

# Usage

```bash
./pr_check.sh [<master branch>]
```

The optional `<master branch>` parameter refers to the branch the pull request's
branch branched from. The script will output all commits marked as squashable
from `HEAD` to the merge-base with `<master branch>`. The default for
`<master branch>` is `master`.

A commit is marked as squashable if it contains the keywords SQUASH or FIX
(case insensitive) within the first five characters of it's subject title.
