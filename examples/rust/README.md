# Introduction

This example shows how to write a RIOT application using Rust.

# Status

As Rust's libstd is too fat for our little MCUs, it is currently only
possible to write Rust applications using `#![no_std]`. However, crates
to map RIOT's API to rust are available in `sys/rust`.

# Prerequisites

Since building binary packages with `#![no_std]` is currently not
supported by the rust stable channel you need a nightly rust toolchain.

After installing the nightly rust toolchain including cargo you should
be good to go.

# Trying the example

As always,

    # make BOARD=native all term

is all you need.
