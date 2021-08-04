/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    test_utils_benchmark_udp    UDP benchmark
 * @ingroup     sys
 *
 * @{
 * @file
 * @brief       Continuously send UDP packets with configurable size and interval.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef TEST_UTILS_BENCHMARK_UDP_H
#define TEST_UTILS_BENCHMARK_UDP_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum size of a benchmark packet
 */
#ifndef BENCH_PAYLOAD_SIZE_MAX
#define BENCH_PAYLOAD_SIZE_MAX  (1024)
#endif

/**
 * @brief   Default address of the benchmark server
 */
#ifndef BENCH_SERVER_DEFAULT
#define BENCH_SERVER_DEFAULT    "fd00:dead:beef::1"
#endif

/**
 * @brief   Default port of the benchmark server
 */
#ifndef BENCH_PORT_DEFAULT
#define BENCH_PORT_DEFAULT      (12345)
#endif

/**
 * @brief   Flag indicating the benchmark packet is a configuration command.
 */
#define BENCH_FLAG_CMD_PKT      (1 << 0)

/**
 * @brief   Configuration Cookie mask.
 */
#define BENCH_MASK_COOKIE       (0xFFFFFF00)

/**
 * @brief   Benchmark message to the server
 * @note    Both server and client are assumed to be little endian machines
 * @{
 */
typedef struct {
    uint32_t flags;     /**< must include config cookie                */
    uint32_t seq_no;    /**< number of packets sent sind config update */
    uint32_t replies;   /**< number of replies received from server    */
    uint32_t rtt_last;  /**< round trip time of the last packet        */
    uint8_t payload[];  /**< variable length payload                   */
} benchmark_msg_ping_t;
/** @} */

/**
 * @brief   Command response from the server
 * @note    Both server and client are assumed to be little endian machines
 * @{
 */
typedef struct {
    uint32_t flags;         /**< contains new config cookie             */
    uint32_t delay_us;      /**< delay between benchmark messages in µs */
    uint16_t payload_len;   /**< payload of benchmark messages          */
} benchmark_msg_cmd_t;
/** @} */

/**
 * @brief   This will start the benchmark process.
 *          Two threads will be spawned, one to send packets to the server
 *          and one to handle the response.
 *
 * @param[in]   server  benchmark server (address or hostname)
 * @param[in]   port    benchmark server port
 *
 * @return      0 on success
 *              error otherwise
 */
int benchmark_udp_start(const char *server, uint16_t port);

/**
 * @brief   Stop the benchmark process
 *
 * @return      true  if the benchmark process was stopped
 *              false if no benchmark process was running
 */
bool benchmark_udp_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_UTILS_BENCHMARK_UDP_H */
/** @} */
