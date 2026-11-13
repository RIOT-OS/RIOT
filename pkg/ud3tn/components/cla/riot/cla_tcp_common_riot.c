// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
#include "cla/posix/cla_tcp_common.h"

#include "platform/hal_io.h"
#include "platform/hal_task.h"
#include "platform/hal_time.h"


int cla_tcp_rate_limit_connection_attempts(
    struct cla_tcp_rate_limit_config *config)
{
    const uint64_t rt_limit_ms = CLA_TCP_RETRY_INTERVAL_MS;
    const int rt_max_attempts = CLA_TCP_MAX_RETRY_ATTEMPTS;
    const uint64_t now_ms = hal_time_get_timestamp_ms();

    if (config->last_connection_attempt_ms + rt_limit_ms > now_ms) {
        if (rt_max_attempts <= 0) {
            LOGF_WARN(
                "TCP: Last connection attempt was less than %llu ms ago, delaying next attempt.",
                rt_limit_ms
                );
        }
        else {
            if (config->last_connection_attempt_no >=
                rt_max_attempts) {
                LOGF_WARN(
                    "TCP: Final retry %d of %d failed.",
                    config->last_connection_attempt_no,
                    rt_max_attempts
                    );
                return -1;
            }
            LOGF_WARN(
                "TCP: Last connection attempt %d of %d was less than %llu ms ago, delaying next attempt.",
                config->last_connection_attempt_no,
                rt_max_attempts,
                rt_limit_ms
                );
            config->last_connection_attempt_no++;
        }
        hal_task_delay(rt_limit_ms);
        config->last_connection_attempt_ms = (
            hal_time_get_timestamp_ms()
            );
    }
    else {
        config->last_connection_attempt_ms = now_ms;
        config->last_connection_attempt_no = 1;
    }
    return 0;
}
