## Overview

Coccinelle allows testing C code against semantic patches, automatically
creating diffs on the way. See http://coccinelle.lip6.fr/ for more information.

## Coccinelle in RIOT

RIOT uses Coccinelle as part of the CI static-tests as follows:

- every added or modified file will be checked
- there are two sets of coccinelle patches: "force" and "warn", found in
  dist/tools/coccinelle/(warn|force)
- if coccinelle has a hit in "warn", the resulting patch will be printed, but
  doesn't cause the CI to fail
- if coccinelle has a hit in "force", the CI build fails

We add semantic patches first to "warn", fix our codebase, and if the patch is
confident enough, move it to "force".

If coccinelle is installed, you can test the whole RIOT checkout by executing

    $ dist/tools/coccinelle/check.sh

... or just test a single patch:

    $ dist/tools/coccinelle/check.sh path/to/semantic_patch.cocci

... or just the files changed to <branch>:

    $ BRANCH=<branch> dist/tools/coccinelle/check.sh

... or combined:

    $ BRANCH=<branch> dist/tools/coccinelle/check.sh path/to/semantic_patch.cocci

... or test a single file:

    $ FILES=path/to/file dist/tools/coccinelle/check.sh [path/to/semantic_patch.cocci]

## Applying coccinelle's patches:

If the coccinelle diff makes sense and you'd like to apply it, you can do so by
piping coccinelle's output into patch:

    $ dist/tools/coccinelle/check.sh | patch -p0

Specifying BRANCH, FILES and/or a specific patch file works here, too.

Beware that some of our semantic patch files might just *find* occurrences of
bad code or style! Not all semantic patches output useful diffs.

## My PR has coccinelle warnings/errors!

Using above commands, try to find out which file caused the warning/error, and
why.  If you're absolutely sure your code is correct, you can ignore it if it's
a  warning.  It's advisable to get this confirmed by others.

If the PR triggers a coccinelle error on code that you're sure is correct, open
a PR that moves the corresponding patch from dist/tools/coccinelle/force to
dist/tools/coccinelle/warn.
