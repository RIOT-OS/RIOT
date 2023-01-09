/**
 * @defgroup pkg_tweetnacl TweetNaCl high-security cryptographic library
 * @ingroup  pkg
 * @ingroup  sys
 * @brief    Provides the TweetNaCl high-security cryptographic library.
 *
 * # TweetNaCl RIOT package
 *
 * TweetNaCl is the world's first auditable high-security cryptographic library.
 * TweetNaCl fits into just 100 tweets while supporting all 25 of the C NaCl
 * functions used by applications. TweetNaCl is a self-contained public-domain C
 * library, so it can easily be integrated into applications.
 *
 * NaCl (pronounced "salt") is a new easy-to-use high-speed software library for
 * network communication, encryption, decryption, signatures, etc. NaCl's goal is
 * to provide all of the core operations needed to build higher-level
 * cryptographic tools.
 *
 * Of course, other libraries already exist for these core operations. NaCl
 * advances the state of the art by improving security, by improving usability,
 * and by improving speed.
 *
 * (from https://nacl.cr.yp.to/ and http://tweetnacl.cr.yp.to/)
 *
 * You can find the API and more information [here](https://nacl.cr.yp.to/), since
 * the sources are not documented due to the aim for fitting in 100 tweets.
 *
 * ## Requirements
 *
 * TweetNaCl requires more than 2K of stack, so beware that you're allocating at
 * least `THREAD_STACKSIZE_DEFAULT + 2048` bytes.
 *
 * You can do it easily by adding:
 *
 * ```makefile
 * CFLAGS += '-DTHREAD_STACKSIZE_MAIN=(THREAD_STACKSIZE_DEFAULT + 2048)'
 * ```
 *
 * to your makefile.
 *
 * ## Usage
 *
 * Just add it as a package in your application:
 *
 * ```makefile
 * USEPKG += tweetnacl
 * ```
 *
 * And don't forget to include the header:
 *
 * ```c
 * #include <tweetnacl.h>
 * ```
 *
 * @see     https://tweetnacl.cr.yp.to/
 */
