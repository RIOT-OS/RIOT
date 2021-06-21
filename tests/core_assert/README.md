Test for `assert()`
=l=================

This test will test that `assert()` has the documented semantics when compiled with `NDEBUG`: A
function called within the condition in `assert()` is dropped. In addition, it will test that
`assert()` does *not* cause panics if (and only if) `NDEBUG` is defined during compilation. Finally,
in a second compilation unit that is compiled without `NDEBUG` a panic is provoked using `assert(0)`
to test failing assertions as well.
