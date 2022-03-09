Test description
----------------

This test ensures that code produced through Rust can actually run;
it does that by simply printing "SUCCESS".


When things fail here
---------------------

This section is aimed at RIOT developers who are taken aback by this test failing
after they changed something completely unrelated.

That can happen for two reasons:
For one, RIOT's API is not as strict as library APIs
(e.g. it is accepted that functions move between being `static inline` and not being),
and then, Rust is sometimes stricter than C when it comes to APIs
(e.g. no implicit numeric casting).

The general strategy for addressing build failures resulting from this is:

<!-- The alternative strategy, of course,
is to @-mention known Rust users in the issue
and ask them to update the Rust side... -->

* Identify what breaks.

* Update the `riot-sys` and `riot-wrappers` crates as necessary
  to support both the old and the new version of the breaking piece.

  If actual workarounds (like an explicit cast) are needed to accommodate the old version,
  a comment should indicate that this is for versions up to the current release,
  to be eventually cleaned out when the crate bumps its minimum supported RIOT version.

* Update the examples to use the new versions in a separate PR that just does
  `cargo update` in the examples, and consequentally contains only Cargo.lock changes.

  These updates can be full releases or merely git updates (see below).

  Such a PR should be easy to get ACKed, as "CI says GO" is usually suffient for them.

* Rebase the breaking PR on the one updating Cargo.lock.

If old and new version can *not* be supported,
the affected crate should get a major release --
but it's also grounds to revisit whether the change is actually as small as originally thought.
Then, the the Cargo.lock file can be updated in the very PR that introduces the breaking change.

When,
for the above reasons or others,
a major release of creates is done,
their new versions should have a `.0-alpha.0`  or similar release.
and a full release of the crate is done after the changing PR is merged.

## Common failure modes

* Primitive types changed.

  This can trigger a change in Rust numeric internals,
  which should then just take the larger of the different versions' types.

  If the type is publicly visible, it is usually already `usize` or a similar maximal type;
  if not, and if using the new RIOT version means that the crate's API needs to change,
  the crate needs to undergo a breaking release.

* Functions are moved between static inline and linked.

  For the functions themselves,
  they should be added to `riot-sys`'s list of `toplevel_from_inline` functions.
  That list ensures that both inline and non-inline versions of the functions are presented as toplevel items --
  typically because they're inline in some platforms or versions, and not others.
  If any such function takes a struct (even through a pointer), that struct's type changes.
  The `inline_cast` function can be used to transmute the pointers with some safety checks.

* Atomics are introduced.

  The C2Rust transpiler that allows using all the non-linked parts of the RIOT API
  is incapable of handling atomics;
  a workaround in `riot-sys`'s `riot-c2rust.h` file contains a list of headers
  that do depend on `stdatomic.h` but don't really expose anything of that as their API;
  they get included with a crude fake of atomics.

  If the addition of a board makes this test fail on that board only,
  it is likely because that board's `board.h` pulls in stdatomic through one of the not yet cleared headers.
  That file needs to be vetted for visible use of atomics,
  and added to that list.

## Updating strategy

Depending on the time frames and people involved,
adjustments to `riot-sys` or `-wrappers` can happen in either of two forms:

* Cargo releases.

  This is the preferred update path:
  The change is made and tested locally,
  PR'd to the upstream [riot-sys] or [riot-wrappers] repository.

  After a release to crates.io has been made with these changes
  (which can often happen right after merging if explicitly requested in the PR),
  the change to RIOT that does the `cargo update` on all the tracked `Cargo.lock` files can happen.
  When that is merged, the breaking PR can rebase onto that and pass CI.

* Switching to the local forks.

  When in a hurry,
  especially when a breaking change has slipped the CI's attention,
  it can be preferable not to go through the full release dance,
  but go for a hot fix.

  Follow this procedure:

  * At the pkgmirror fork of the crate that needs changing
    (<https://github.com/RIOT-OS-pkgmirror/riot-sys> or <https://github.com/RIOT-OS-pkgmirror/riot-wrappers>),
    start from the point currently indicated by the Cargo.lock files.
    This is typically a tag matching the released version.
    It may be necessary to pull updates into the mirror from upstream.

  * Make the necessary adjustments in a branch on that repository.

  * Add lines like these to the Rust examples and tests:

    ```
    [patch.crates-io]
    riot-wrappers = { git = "https://github.com/RIOT-OS-pkgmirror/riot-wrappers", branch = "my-branch-name" }
    ```

  * Run `cargo update`, and commit the change Cargo.lock and Cargo.toml files.

    This commit should still preferably be in a **separate RIOT PR**
    and not bundled up with the main PR.
    This ensures that the change on the Rust side works both before and after the change on the C side.

  * Rebase the breaking PR onto the PR updating the Cargo files
    (unless this is a situation recovering from breakage on the master branch,
    merging the Cargo PR already fixes everything).

  * To enable switching back to released versions,
    file a PR with the changes upstream.

    Once a release has been made that includes the changes,
    the crates.io patch can be removed in a new change that increments the crate version dependency.

## When things work locally but not on CI / Docker

Occasionally,
it might happen that after a `cargo update`,
docker builds (including those in CI) fail.

This can be due to a dependency with nightly features requiring a newer nightly version than riotdocker ships.

In that case, suitable steps are:

* On the short term, keep the offending crate at a known-good version, for example like:

  `cargo update --precise 0.2.4 -p cstr_core`

* For the long term fix, change the pinned Rust nightly version
  in the `--default-toolchain` line of [riotbuild's Dockerfile].

[riot-sys]: https://gitlab.com/etonomy/riot-sys/
[riot-wrappers]: https://gitlab.com/etonomy/riot-wrappers/
[riotbuild's Dockerfile]: https://github.com/RIOT-OS/riotdocker/blob/master/riotbuild/Dockerfile
