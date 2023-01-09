/**
 * @defgroup pkg_wamr WebAssembly Micro Runtime
 * @ingroup  pkg
 * @brief    Provides WebAssembly support for RIOT
 * @see      https://github.com/bytecodealliance/wasm-micro-runtime
 *
 * # WebAssembly Micro Runtime Riot Package
 *
 * "WebAssembly Micro Runtime (WAMR) is a standalone WebAssembly (WASM)
 * runtime with a small footprint."
 *
 * @see      https://github.com/bytecodealliance/wasm-micro-runtime
 *
 * ## Status
 *
 * WAMR integration into RIOT should be considered experimental.
 * Changes that had to be done in WAMR are send to and integrated into upstream.
 * WAMR provides support for numerous architectures:
 * "X86_32/64", "AARCH64", "ARM", "THUMB", "XTENSA" and "RISCV"
 * this Package will build for native (x86,arm, aarch(using arm)), cortex-m, esp32 and riscv
 *
 * support for other architectures might provideable using the generic
 * `core/iwasm/common/arch/invokeNative_general.c` this is not tested
 *
 * tested : Cortex-M - "THUMB", native - "X86_32"
 *
 * ## Memory usage
 *
 * WebAssembly defines its pages to be 64kB -> WAMR needs a good amount of RAM to run.
 *
 * WASM files can be linked to use just a part of the first page.
 * In this case the VM can be run with less ram.
 * (see `wasm_sample/Makefile` in `examples/wasm` for linker options to help with this)
 * While running the example configured with 8KiB Heap and 8KiB Stack,
 * ~24KiB of System Heap are used.
 * The thread, the WAMR interpreter (iwasm) is executed in,
 * should be configured to have more than 3KiB of Stack,
 * this also depend on the architecture and the native functions that are called.
 *
 * ## building wasm-bytecode
 *
 * `clang` and `wasm-ld` of the *same version* must be used
 * The Makefile in `examples/wasm/wasm_sample/Makefile` will try to guess
 * a matching clang, wasm-ld pair, if they do not match linking will fail.
 *
 * ## Configuration
 *
 * WAMR compilation is configured using a CMAKE config file (see `pkg/wamr/config.cmake`)
 * Add `export WAMR_CONFIG := $(abspath config.cmake)` to Makefile to apply a specific config.
 * Most options (e.g. WASI) are not supported in RIOT since they have OS requirements,
 * that are no yet fulfilled.
 *
 * ## Usage Details
 *
 * WAMR should be used using the functions provided by the WAMR project their API-headers
 * they can be found in `<RIOT>/build/pkg/wamr/core/iwasm/include/`.
 * pkg/wamr adds no RIOT specific API to that.
 * For simple usages like in the example `iwasm.c` in `examples/wasm` might be useful and
 * if used should be copied and adapt to the application need.
 *
 * While WebAssembly does not define a set native functions. WAMR provides its own builtin-libc.
 * Other native functions may be provided by registering native_api to WAMR.
 * @see https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/doc/export_native_api.md
 *
 * ### Why is there no RIOT specific API?
 *
 * many use cases more complex than: take that Bytecode and run its main would require
 * a application specific usage pattern of the WAMR-API
 * (memory setup, function search, parameter, module-loading/unloading)
 * the WAMR-API does this well and they provide a second wasm-c-api compatible API.
 *
 * `iwasm.c` might therefore be a good starting point (copy) but it is no good
 * generic interface to WAMR. Making it good and generic would lead to the API
 * provided by WAMR.
 *
 * ## Upstream Documentation
 *
 * @see https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/README.md
 * @see https://github.com/bytecodealliance/wasm-micro-runtime/tree/main/doc
 * @see https://github.com/bytecodealliance/wasm-micro-runtime/wiki
 *
 * @see https://github.com/WebAssembly/wasm-c-api
 * Include Headers:
 * @see https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/core/iwasm/include/
 *
 */
