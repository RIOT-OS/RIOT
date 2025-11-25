/**
 * @defgroup pkg_u8g2   U8G2 graphic library for monochome displays
 * @ingroup  pkg
 * @brief    Provides a monochrome graphics library for OLED and LCD displays
 * @see      https://github.com/olikraus/u8g2
 *
 * ## Requirements
 *
 * Using SDL requires more stack so in case you are using it add
 *
 * ```makefile
 * CFLAGS += '-DTHREAD_STACKSIZE_MAIN= 48*1024'
 * ```
 *
 * to your makefile.
 * 48kB is enough for the test other uses may need more or may
 * need this to be applied to other threads using `THREAD_STACKSIZE_DEFAULT`
 *
 * @include pkg/u8g2/README.md
 */
