/**
 * @defgroup pkg_nanopb small Protocol Buffers library
 * @ingroup  pkg
 * @ingroup  sys
 * @brief    Provides a protocol buffers library to RIOT
 *
 * @see      https://github.com/nanopb/nanopb
 *
 * # Limitations
 *
 * The generated headers and includes from `.proto` files not accessible outside the
 * module in which the `.proto` file is located.
 *
 * You can include `.proto` files that are not in the same directory by adding the
 * directory of the `.proto` include to `PROTOBUF_PATH`.
 */
