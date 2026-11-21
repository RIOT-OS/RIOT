**Contributions are welcome.**

*tl;dr: Open issues. PR early, PR small. Tests are good. Callback lifetimes are hard.*

<!-- This text is aimed at people who already decided they want to contribute; let's keep it concise, acknowledging that they probably have some experience in software development. -->

## Contribution guidelines

General [RIOT contribution guidelines](https://github.com/RIOT-OS/RIOT/blob/master/CONTRIBUTING.md) apply,
with some

* focus points:

    * When wrapping a new RIOT feature, **keep it small**.
      Attempting to implement all features of a particular RIOT subsystem easily gets caught in long iterations of review.
      Try wrapping a minimal viable version first, and let's take it from there.

    * Let's **verify concepts early**.
      Opening an issue before larger changes, creating them as draft PRs or discussing it on [our Matrix channel](https://matrix.to/#/#riot-os:matrix.org)
      are all convenient ways to do that.

    * New features should come with tests, at least on a coarse granularity.
      Tests in [`./tests/`](tests) are automatically picked up by CI.

* and alterations:

    * RIOT has no coding conventions for Rust;
      instead, [common Rust conventions](https://doc.rust-lang.org/stable/style-guide/) apply.
      Public items should be documented,
      and code should be `cargo fmt` before each commit.

    * Merging criteria are not as strict as in RIOT OS:
      RIOT maintainers may merge PRs without a 2nd pair of eyes
      (but are encouraged to solicit review on larger changes).

      Before code from this crate gets used by RIOT OS,
      it undergoes a secondary review step [similar to PR #20786](https://github.com/RIOT-OS/RIOT/pull/20786),
      just like any external package code.

    * The license of this crate is "[MIT](https://spdx.org/licenses/MIT.html) OR [Apache-2.0](https://spdx.org/licenses/Apache-2.0.html)" as stated in [Cargo.toml](Cargo.toml) and the [README](README.md).
      This eases interoperability with the rest of the Rust ecosystem;
      beware that built binaries include RIOT OS, and thus have LGPL-2.1 components.

## Common pitfalls

* Threads and interrupts in RIOT are modelled as is [common](https://onevariable.com/blog/interrupts-is-threads/) in Rust:
  Data needs to be [`Send`](https://doc.rust-lang.org/std/marker/trait.Send.html) to be passed around.
  Whenever generic data (in particular references) is passed into C functions,
  trait bounds need to ensure that it is `Send`.

* Registering callbacks with RIOT OS typically means
  that the OS may invoke the callback at any time.
  Thus the callback either must be `'static`,
  or it needs to be ensured that the callback is fully unregistered
  before its lifetime ends.

  Note that we can not rely on [`Drop`](https://doc.rust-lang.org/std/ops/trait.Drop.html) to be called
  (for any user may safely [`forget()`](https://doc.rust-lang.org/std/mem/fn.forget.html) an item at any time).

  Running user code in Rust closures is a common workaround
  (eg. [in `thread::scope`](https://doc.riot-os.org/rustdoc/latest/riot_wrappers/thread/fn.scope.html)),
  but beware that any items in that scope need to be [branded](http://plv.mpi-sws.org/rustbelt/ghostcell/paper.pdf) (like with the `'id` of the thread scope):
  Otherwise, users might create nested scopes and switch around the provided types,
  delaying their cleanup from the inner to the outer scope.
