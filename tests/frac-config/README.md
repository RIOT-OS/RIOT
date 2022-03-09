This is a helper program to generate frac_t static initializers, to avoid the
runtime overhead of calling frac_init during initialization.

When using the precomputed values it is possible to declare const frac_t in ROM,
which is not possible with runtime computation.

Example static configuration:

    static const frac_t myfrac = {
        .num = 512,
        .den = 15625,
        .div = { .magic = 0x8637bd05af6c69b6ull, .more = 0x0d },
    };

you can use myfrac as usual:

    scaled_number = frac_scale(&myfrac, unscaled_number);
