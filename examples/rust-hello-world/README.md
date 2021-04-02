Hello World!
============

This is a basic example how to use Rust to write your RIOT application.
It prints out the famous text `Hello World!`.

This example should foremost give you an overview how an application built
completely in Rust is structured:

* The Makefile resembles the regular application Makefile, see ../hello-world/
  for more introduction to that.

* The Cargo.toml file describes the Rust code, and declares its dependencies.

  Prominently, it contains a `[lib]` / `crate-type = ["staticlib"]` section,
  which is necessary for how RIOT later links together the C and Rust portions.

* The file src/lib.rs (and any modules referenced by it) contain Rust code to
  be run.

  It uses the `riot_main!` macro provided by the riot-wrappers crate to declare
  the entry point of the program.

The code itself looks like the usual Rust hello-world example.
