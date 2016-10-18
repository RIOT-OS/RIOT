# Introduction

"Rust is a systems programming language that runs blazingly fast, prevents
segfaults, and guarantees thread safety."

This example shows how to write a RIOT application using Rust.

# Status

As Rust's libstd is too fat for our little MCUs, it is currently only possible
to write Rust applications using ```#![no_std]```.
There's a crate on crate.io trying to map RIOT's API to rust, but that is in
very early stages.

# Prerequisites

You'll need a Rust nightly toolchain and xargo.

This should get you started:

    # curl https://sh.rustup.rs -sSf | sh -s -- -y --default-toolchain=nightly
    # . ~/.cargo/env
    # cargo install xargo

# Trying the example

As always,

    # BOARD=samr21-xpro make clean all flash term

is all you need.

# Internals

RIOT's make system intercepts cargo's linking step and just copies the objects
and archives cargo tries to link into RIOT's build dir, then just links
everything together.

# Known issues

- currently this works only for Cortex-M platforms
