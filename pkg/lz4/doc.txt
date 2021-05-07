/**
 * @defgroup pkg_lz4   LZ4 compression library
 * @ingroup  pkg
 * @brief    LZ4 is a fast compression library
 *
 * # Note about memory
 *
 * By default, the LZ4 implementation allocates memory on the stack. This
 * way compression/decompression functions are fast but this comes at the cost
 * of RAM usage.
 * It is important to allocated enough stack memory (at least 16384 bytes) to
 * any thread using LZ4 primitives.
 *
 * # License
 *
 * Licensed under BSD 2-clause.
 *
 * @see      https://github.com/lz4/lz4
 */
