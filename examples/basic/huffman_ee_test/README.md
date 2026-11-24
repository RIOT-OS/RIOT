Energy-aware Huffman coding
===========================

This example shows what the `huffman_ee` module does for a sensor node.

A sensor value that is sent over the radio costs energy. On many radios sending
a `1` costs more than sending a `0`, and every change between `0` and `1` costs
a little extra. Huffman coding already gives frequent symbols short codewords,
but it does not care which bits those codewords are made of. The `huffman_ee`
module keeps the same codeword lengths (so the compressed size does not change)
and only chooses the labels, which lowers the energy of the transmitted stream.

The example sends the same stream of readings in several ways and prints the
number of bits and the energy of each:

* `raw (no compression)` — the value as a plain fixed-width field.
* `Huffman, plain` — a normal Huffman code.
* `Huffman, energy-aware` — the code built by this module.
* `Huffman, exact optimal` — the same, but with the exact labelling instead of
  the greedy one.

For comparison the example also runs the compression packages that ship with
RIOT on the same readings, one byte per reading:

* `bytes, no coding` — the readings as they come off the sensor, without any
  coding. This is the baseline of the packages below.
* `uzlib (deflate)` — LZ77 together with Huffman coding.
* `heatshrink (LZSS)` — the compressor that RIOT uses for small devices.
* `lz4` — a fast, byte oriented compressor.

The packages are given one byte per reading and their output is counted per
transmitted byte, so those rows are counted in a different unit than the
Huffman rows above.

The two approaches do different things. The compression packages remove bytes,
and that is what saves energy on radios whose transmit power is constant, such
as BLE, 802.15.4 and LoRa: there the energy follows the airtime, so only the
number of bytes matters. This module keeps the number of bits as it is and
lowers the energy of the bits that are sent, which helps on links whose energy
depends on the bit pattern itself, for example on-off-keying transmitters,
where a `1` draws current and every change between `0` and `1` costs extra. The
example prints both, so it is visible where the module helps and where it does
not.

All rows use the same energy model

    E = alpha * N1 + beta * Nt + lambda * Ns

where `N1` is the number of `1` bits, `Nt` the transitions inside a codeword and
`Ns` the transitions between two neighbouring codewords.

The first data set is the temperature distribution of the Intel Berkeley
Research Lab, a typical skewed sensor distribution. The same comparison is then
repeated with flat data, where every value is equally likely: there no labelling
can lower the energy and the saving is 0 %. This shows when the module helps and
when it does not.

Usage
-----

Build and run the example. On the `native` board it runs directly on the host:

    make BOARD=native all
    ./bin/native64/huffman_ee_test.elf

On a real board:

    make BOARD=<your-board> all flash term

The module is used like this:

    huffman_ee_params_t params = { .alpha = 20, .beta = 1, .lambda = 1 };
    huffman_ee_table_t table;

    huffman_ee_build(&table, symbols, freqs, num_symbols, &params);
    huffman_ee_encode(&table, stream, stream_len, out, out_size, &out_bits);

Expected output
---------------

    huffman_ee demo: with and without the module

    === skewed sensor data (IBRL temperatures) ===
    weights: alpha=20 beta=1 lambda=1

    method                    bits     N1     Nt     Ns     energy    saving
    raw (no compression)     25000  14119  12937   2589     297906  +0.00 %
    Huffman, plain           17099   9557   5920   2520     199580  +33.00 %
    Huffman, energy-aware    17099   7542   5920   2520     159280  +46.53 %
    Huffman, exact optimal   17099   7542   5920   2520     159280  +46.53 %

    (without the module the value is sent raw; the Huffman rows all have the same size and differ only in energy)

    (the compression packages below are given one byte per reading; their stream is counted per transmitted byte)

    bytes, no coding         40000  14119  17924   2411     302715  -1.61 %
    uzlib (deflate)          39352  13808  18557   3259     297976  -0.02 %
    heatshrink (LZSS)        29488  10658  12341   1768     227269  +23.71 %
    lz4                      35776  11761  13744   2034     250998  +15.74 %
    (the compression packages and this module work on different things: they remove bytes, the module lowers the energy of the bits that are sent)

    === same data, alpha=1 (1 bits and transitions equal) ===
    weights: alpha=1 beta=1 lambda=1

    method                    bits     N1     Nt     Ns     energy    saving
    raw (no compression)     25000  14013  12828   2572      29413  +0.00 %
    Huffman, plain           17275   9577   6119   2609      18305  +37.76 %
    Huffman, energy-aware    17275   7724   6088   2566      16378  +44.31 %
    Huffman, exact optimal   17275   8003   5706   2644      16353  +44.40 %

    === flat data (worst case: all values equally likely) ===
    weights: alpha=20 beta=1 lambda=1

    method                    bits     N1     Nt     Ns     energy    saving
    raw (no compression)     15000   7476   5097   2420     157037  +0.00 %
    Huffman, plain           15000   7476   5097   2420     157037  +0.00 %
    Huffman, energy-aware    15000   7476   5097   2420     157037  +0.00 %
    Huffman, exact optimal   15000   7476   5097   2420     157037  +0.00 %

The example uses a fixed seed, so the numbers above are reproducible. The
relations are what matter: the Huffman rows all have the same size, because the
module only changes the labels, and the energy-aware one is the cheapest. On
flat data there is nothing to save and every row is identical. Compared to
sending the values uncompressed, the module uses both fewer bits and less
energy when the distribution is skewed. The compression packages are listed
next to it only to show what they do on the same data: they remove bytes, which
the module does not, and the module lowers the energy of the bits, which they
do not.

Resources
---------

The tables and the stack used by the exact builders scale with the maximum
number of symbols, `CONFIG_HUFFMAN_EE_MAX_SYMBOLS` (256 by default). One table
needs about 14 bytes per symbol, and `huffman_ee_build_exact()` and
`huffman_ee_build_exact_lambda()` need around 60 bytes of stack per symbol,
which is about 15 KiB with the default bound. Boards whose main thread stack is
smaller than that have to lower the bound:

    CFLAGS += -DCONFIG_HUFFMAN_EE_MAX_SYMBOLS=32

This example only ever sends 14 different values, so 32 symbols are plenty and
the example sets that bound itself. The other builders need far less stack.
