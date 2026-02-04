// SPDX-FileCopyrightText: 2020 Christian Amsüss
// SPDX-License-Identifier: LGPL-2.1-only
#![no_std]

use riot_wrappers::println;
use riot_wrappers::riot_main;

extern crate rust_riotmodules;

riot_main!(main);

fn main() {
    println!("SUCCESS");
}
