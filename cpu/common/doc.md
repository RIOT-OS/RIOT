@defgroup    cpu_common Common CPU Groups
@ingroup     cpu
@brief       Common definitions and implementations for CPU groups

CPUs may share many definitions and peripheral implementations, these are
collectively stored into a common module per CPU group in `cpu/common/<name>`
where `<name>` should boil down to a common denominator, e.g. `nrf5x` for
nRF51, nRF52, ... CPUs.

A CPU module can use that common module by adding `USEMODULE += cpu_common_<name>`
to its `Makefile.dep`. The `cpu/common/<name>/Makefile{.dep,.include}` files
are loaded automatically. The common `Makefile.include` is included first.

The common `Makefile.features` has to be included explicitly in
the `Makefile.features` of the CPU, as the features provided by a board have
to be known before the dependency resolution, e.g. for
`make info-boards-supported`.
