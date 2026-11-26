# Changes in 0.9.1

* gnrc::nib: New module for GNRC neighbor information.

* LED: Differentiate constructors, deprecate `::new()`.

* Internally use `const { }` and other recent Rust idioms suich as `c""`.

* Update dependencies: Rust to 1.79 and the coap-message crate (implementing message upgrading).

* Extend tests.

# Changes in 0.9.0

## Breaking changes

*These items will require code changes for all who use them.*

* VFS: Opening directories now takes a pinned slot to store the open directory in.

* ZTimer: Sleep functions were renamed for consistency

  * `sleep()` became `sleep_extended()`

  * `sleep_ticks()` became `sleep()` and takes `Ticks(u32)` instead of a plain `u32`

* Support for all outdated traits was removed:

  * `embedded-hal` 0.2 (except for ADC and SPI)
  
  * `coap-handler` 0.1 and `coap-message` 0.2

  * `embedded-nal-async` 0.6

## Subtle breakage

*These items will only require code changes for users who had unresolved deprecation warnings,
or who explicitly named clock or error types.*

* All deprecated items were removed.

* gcoap: `PacketBuffer` now has a lifetime.

* saul: `Unit` is non-exhaustive.

* shell: `.and()` returns an `impl CommandList` instead of a `Command`.
  This drastically enhances usability because chaining is now possible without extra effort going into type annotations.

* ztimer: Clocks are now generated as `ValueInThread<Clock<HZ>>`
  and require to stay in a thread to allow using the `sleep{,_*}` methods.

* Traits that previously used `!` as associated error type
  now use `Infallible`.

* Dependencies: heapless is now used in version 0.8.
  This is a public dependency because `saul::Unit::name_owned` returns a buffer.

## Enhancements

* gpio: `.toggle()` was added.

* shell: `.run_forever()` and `.run_once()` are now available with provided buffers;
  the previous `…_providing_buf()` variants are now deprecated aliases.

* ztimer: Timers can be `.acquire()`'d and then provide a `.now()` method,
  as provide a `.time()` method to measure the execution time of a closure.

## Bug fixes

* VFS: Fixed out-of-bounds read for file names that were not nul-terminated.

## Internal changes

* Tests were added for shell and ztimer.

* Rust shows no more warnings for this crate.
