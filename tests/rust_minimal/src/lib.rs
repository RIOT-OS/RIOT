// Copyright (C) 2020 Christian Amsüss
//
// This file is subject to the terms and conditions of the GNU Lesser
// General Public License v2.1. See the file LICENSE in the top level
// directory for more details.
#![no_std]

use riot_wrappers::riot_main;
use riot_wrappers::println;

extern crate rust_riotmodules;

riot_main!(main);

fn main() {
    println!("SUCCESS");
}
