#![no_std]

use riot_wrappers::println;
use riot_wrappers::random::Random;
use riot_wrappers::riot_main;

riot_main!(main);

fn check_csrng(mut rng: impl rand_core::CryptoRng + rand_core::RngCore) {
    use rngcheck::nist::*;

    // This is also in https://github.com/ryankurte/rngcheck/pull/3
    struct BitIter<'a, R: rand_core::RngCore> {
        rng: &'a mut R,
        remaining: usize,
        buffer: u32,
        buffered: u8,
    }

    impl<'a, R: rand_core::RngCore> BitIter<'a, R> {
        fn new(rng: &'a mut R, items: usize) -> Self {
            Self {
                rng,
                remaining: items,
                buffer: 0,
                buffered: 0,
            }
        }
    }
    impl<'a, R: rand_core::RngCore> Iterator for BitIter<'a, R> {
        type Item = bool;
        fn next(&mut self) -> Option<bool> {
            if self.remaining == 0 {
                return None;
            }

            self.remaining -= 1;

            if self.buffered == 0 {
                self.buffer = self.rng.next_u32();
                self.buffered = 32;
            }
            let result = self.buffer & 1 != 0;
            self.buffer >>= 1;
            self.buffered -= 1;
            Some(result)
        }
    }

    // 1 megabit; enough to complete on an nRF52840 within the test timeout
    let count = 1000000;

    println!(
        "Monobit stat: {}",
        nist_freq_monobit(BitIter::new(&mut rng, count)).unwrap()
    );
    // To be fair, I have no clue whether 16 is a good number here.
    //
    // These are all reporting NaN due to https://github.com/ryankurte/rngcheck/issues/1 -- but
    // then again, we're mainly doing these so that the RNG runs for a bit.
    println!(
        "Frequency block stat: {}",
        nist_freq_block(BitIter::new(&mut rng, count), 16).unwrap()
    );

    println!("Generating 3 more random numbers");
    for _ in 0..3 {
        println!("{}", rng.next_u32());
    }

    println!("Done");
}

fn main() {
    // works because we have periph_hwrng
    let rng: Random = Default::default();
    check_csrng(rng);
}
