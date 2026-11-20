/* core libraries */
#include <irq.h>
#ifdef MODULE_CORE_MSG
#include <msg.h>
#endif
#include <mutex.h>
#include <panic.h>
#ifdef MODULE_CORE_THREAD_FLAGS
#include <thread_flags.h>
#endif
#include <thread.h>

/* board include */
#include <board.h>

/* periph drivers */
#ifdef MODULE_PERIPH_ADC
#include <periph/adc.h>
#endif
#ifdef MODULE_PERIPH_CAN
#include <periph/can.h>
#endif
#ifdef MODULE_PERIPH_CPUID
#include <periph/cpuid.h>
#endif
#ifdef MODULE_PERIPH_DAC
#include <periph/dac.h>
#endif
#ifdef MODULE_PERIPH_EEPROM
#include <periph/eeprom.h>
#endif
#ifdef MODULE_PERIPH_FLASHPAGE
#include <periph/flashpage.h>
#endif
#ifdef MODULE_PERIPH_GPIO
#include <periph/gpio.h>
#include <periph/gpio_util.h>
#endif
#ifdef MODULE_PERIPH_HWRNG
#include <periph/hwrng.h>
#endif
#ifdef MODULE_PERIPH_I2C
#include <periph/i2c.h>
#endif
#ifdef MODULE_PERIPH_INIT
#include <periph/init.h>
#endif
#ifdef MODULE_PERIPH_PM
#include <periph/pm.h>
#endif
#ifdef MODULE_PERIPH_PWM
#include <periph/pwm.h>
#endif
#ifdef MODULE_PERIPH_QDEC
#include <periph/qdec.h>
#endif
#ifdef MODULE_PERIPH_RTC
#include <periph/rtc.h>
#endif
#ifdef MODULE_PERIPH_RTT
#include <periph/rtt.h>
#endif
#ifdef MODULE_PERIPH_SPI
#include <periph/spi.h>
#endif
#ifdef MODULE_PERIPH_TIMER
#include <periph/timer.h>
#endif
#ifdef MODULE_PERIPH_UART
#include <periph/uart.h>
#endif
// Disabled as it'd trigger the USB_H_USER_IS_RIOT_INTERNAL checks.
//
// The right way to enable it would be to add a Rust feature to riot-sys,
// off-by-default, that an application pulls in (probably via an equivalent
// flag in riot-wrappers) that enables access to usbdev. Once anything in the
// dependency tree does that, that pulls the whole build into the "needs a
// declared USB ID, and if it's only testing" territory.
//
// #ifdef MODULE_PERIPH_USBDEV
// #include <periph/usbdev.h>
// #endif
#ifdef MODULE_PERIPH_WDT
#include <periph/wdt.h>
#endif

/* sys libraries */
#ifdef MODULE_BLUETIL_AD
#include <net/bluetil/ad.h>
#endif
#ifdef MODULE_CORD_COMMON
#include <net/cord/common.h>
#endif
#ifdef MODULE_CORD_EP
#include <net/cord/ep.h>
#endif
#ifdef MODULE_CORD_EP_STANDALONE
#include <net/cord/ep_standalone.h>
#endif
#ifdef MODULE_CORD_EPSIM
#include <net/cord/epsim.h>
#endif
#ifdef MODULE_GCOAP
#include <net/gcoap.h>
#endif
#include <net/gnrc.h>
#include <net/gnrc/udp.h>
#include <net/gnrc/pktbuf.h>
#include <net/gnrc/ipv6.h>
#include <net/gnrc/nettype.h>
#include <net/gnrc/netapi.h>
#ifdef MODULE_GNRC_ICMPV6
#include "net/gnrc/icmpv6.h"
#endif
#ifdef MODULE_HASHES
#include <hashes.h>
#include <hashes/aes128_cmac.h>
#include <hashes/md5.h>
#include <hashes/pbkdf2.h>
#include <hashes/sha1.h>
#include <hashes/sha224.h>
#include <hashes/sha256.h>
#include <hashes/sha3.h>
#include <hashes/sha512.h>
#endif
#ifdef MODULE_NANOCOAP
#include <net/nanocoap.h>
#endif
#ifdef MODULE_NANOCOAP_SOCK
#include <net/nanocoap_sock.h>
#endif
#ifdef MODULE_RANDOM
#include <random.h>
#endif
#ifdef MODULE_SOCK
#include <net/sock.h>
#endif
#ifdef MODULE_SOCK_UDP
#include <net/sock/udp.h>
#endif
#ifdef MODULE_SOCK_ASYNC
#include <net/sock/async.h>
#endif
#include <saul.h>
#include <saul_reg.h>
#include <stdio_base.h>
#ifdef MODULE_SHELL
#include <shell.h>
#endif
#ifdef MODULE_SOCK_UTIL
#include <net/sock/util.h>
#endif
#ifdef MODULE_PTHREAD
#include <pthread.h>
#endif
#ifdef MODULE_SUIT
#include "suit.h"
#include "suit/conditions.h"
#include "suit/transport/worker.h"
#endif
#ifdef MODULE_SUIT_TRANSPORT_COAP
#include "suit/transport/coap.h"
#endif
#ifdef MODULE_RIOTBOOT_SLOT
#include "riotboot/slot.h"
#endif
#ifdef MODULE_TINY_STRERROR
#include "tiny_strerror.h"
#endif
#ifdef MODULE_UUID
#include "uuid.h"
#endif
#ifdef MODULE_XTIMER
// Uses C11 generics since https://github.com/RIOT-OS/RIOT/pull/20494
#ifndef IS_C2RUST
#include <xtimer.h>
#endif
#endif
#ifdef MODULE_ZTIMER
#include <ztimer.h>
#endif
#ifdef MODULE_ZTIMER64
#ifndef IS_C2RUST
#include <ztimer64.h>
#endif
#endif
#ifdef MODULE_ZTIMER_PERIODIC
#include <ztimer/periodic.h>
#endif
#ifdef MODULE_VFS
// Touches atomics, but we don't need macro expansions or static inlines from this one
#ifndef IS_C2RUST
// Actually using VFS needs constants like O_RDONLY
#include <fcntl.h>
#include <vfs.h>
#endif
#endif
#ifdef MODULE_AUTO_INIT
#include "auto_init_utils.h"
#endif

/* packages */
#ifdef MODULE_NIMBLE_AUTOADV
#  ifndef IS_C2RUST
/* These contain variadic functions ... and besides, let's only use C2Rust where we need it */
#    include "nimble_autoadv.h"
#    include "nimble_autoadv_params.h"
#  endif
#endif
#ifdef MODULE_NIMBLE_HOST
#  include "host/ble_gatt.h"
#  ifndef IS_C2RUST
#    include "host/ble_gap.h"
#  endif
#  include "host/ble_hs_adv.h"
#endif
#ifdef MODULE_NIMBLE_SVC_GAP
#include "services/gap/ble_svc_gap.h"
#endif

/* drivers */
#ifdef MODULE_MICROBIT
#include "microbit.h"
#endif
#ifdef MODULE_WS281X
#include "ws281x_params.h"
#include "ws281x.h"
#endif

/* wolfSSL */
#if defined(MODULE_WOLFSSL) && !defined(IS_C2RUST)
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/ssl.h>
#include <sock_tls.h>
#endif

// Note that while the actual definitions are always in board.h, this defines
// the fallback macros that make sure that in the LED macros' absence,
// fallbacks are there.
#include <led.h>
