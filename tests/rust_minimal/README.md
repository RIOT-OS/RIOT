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

* Identify what breaks.

* Update the `riot-sys` and `riot-wrappers` crates as necessary
  to support both the old and the new version of the breaking piece.

  If actual workarounds (like an explicit cast) are needed to accommodate the old version,
  a comment should indicate that this is for versions up to the current release,
  to be eventually cleaned out when the crate bumps its minimum supported RIOT version.

* Update the examples to use the new versions in a separate PR that just does
  `cargo update` in the examples, and consequentally contains only Cargo.lock changes.

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

Common failure modes are:

* Primitive types changed.

  This can trigger a change in Rust numeric internals,
  which should then just take the larger of the different versions' types.

  If the type is publicly visible, it is usually already `usize` or a similar maximal type;
  if not, and if using the new RIOT version means that the crate's API needs to change,
  the crate needs to undergo a breaking release.

* Functions are moved between static inline and linked.

  For the functions themselves, this is caught automatically because `riot-sys`
  presents a unified view on symbols produced from linked and static functions.
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

<!-- The alternative strategy, of course,
is to @-mention known Rust users in the issue
and ask them to update the Rust side... -->
