SFR CongURE implementation test
===============================
This test is largely based on the [`gnrc_networking`][1] example but is changed
so SFR with different CongURE implementations can be tested. When `CONGURE_IMPL`
is not set in the environment, `gnrc_sixlowpan_frag_sfr_congure_sfr` is used,
other implementations can be used with `congure_<impl>`.

[1]: https://github.com/RIOT-OS/RIOT/tree/master/examples/networking/gnrc/gnrc_networking
