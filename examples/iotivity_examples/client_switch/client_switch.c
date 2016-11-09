/*
 * Copyright (C) 2016 CREATE-NET
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     Examples
 * @{
 *
 * @file        client_switch.c
 * @brief       This is an Iotivity client. It sends a PUT when the
 *              User button is pressed.
 *
 * @author      Mattia Antonini <mattia.antonini1@studenti.unipr.it>
 *                              <m.antonini@create-net.org>
 *
 * @}
 */

#include "oc_api.h"
#include "pthread_cond.h"
#include "thread.h"
#include "xtimer.h"
#include "periph/gpio.h"

#define MAX_URI_LENGTH (30)

static void observe_light(oc_client_response_t *data);
extern int _netif_config(int argc, char **argv);

static int quit;
static mutex_t mutex;
static pthread_cond_t cv;
static struct timespec ts;
static bool got_discovery_response = false;
static char light_1[MAX_URI_LENGTH];
static oc_server_handle_t light_server;
static bool light_state = false;
static bool led_state = false;

static char _oc_main_stack[THREAD_STACKSIZE_MAIN];

static void
set_led_state(bool led_state)
{
    if (led_state) {
        LED0_ON;
    }
    else {
        LED0_OFF;
    }
}

static void
set_device_custom_property(void *data)
{
    oc_set_custom_device_property(purpose, "operate lamp");
}

static void
app_init(void)
{
    oc_init_platform("RIOT-OS", NULL, NULL);
    oc_add_device("/oic/d", "oic.d.switch", "RIOT switch", "1.0", "1.0",
                  set_device_custom_property, NULL);
}

static oc_event_callback_retval_t
stop_observe(void *data)
{
    PRINT("client_oic: Stopping OBSERVE\n");
    oc_stop_observe(light_1, &light_server);
    return DONE;
}

static void
put_light(oc_client_response_t *data)
{
    PRINT("client_oic: PUT_light:\n");
    if (data->code == OC_STATUS_CHANGED) {
        PRINT("client_oic: PUT response OK\n");
    }
    else {
        PRINT("client_oic: PUT response code %d\n", data->code);
    }
}

static void
observe_light(oc_client_response_t *data)
{
    PRINT("client_oic: OBSERVE_light: ");
    oc_rep_t *rep = data->payload;
    while (rep != NULL) {
        PRINT("key %s, value ", oc_string(rep->name));
        switch (rep->type) {
            case BOOL:
                PRINT("%d\n", rep->value_boolean);
                light_state = rep->value_boolean;
                break;
            default:
                break;
        }
        rep = rep->next;
    }
}

static oc_discovery_flags_t
discovery(const char *di, const char *uri, oc_string_array_t types,
          oc_interface_mask_t interfaces, oc_server_handle_t *server,
          void *user_data)
{
    int i;
    int uri_len = strlen(uri);

    uri_len = (uri_len >= MAX_URI_LENGTH) ? MAX_URI_LENGTH - 1 : uri_len;

    for (i = 0; i < oc_string_array_get_allocated_size(types); i++) {
        char *t = oc_string_array_get_item(types, i);
        if (strlen(t) == 11 && strncmp(t, "oic.r.light", 11) == 0) {
            memcpy(&light_server, server, sizeof(oc_server_handle_t));

            strncpy(light_1, uri, uri_len);
            light_1[uri_len] = '\0';

            got_discovery_response = true;

            return OC_STOP_DISCOVERY;
        }
    }
    return OC_CONTINUE_DISCOVERY;
}

#ifdef BUTTON_GPIO
void button_callback(void)
{
    gpio_irq_disable(BUTTON_GPIO);
    if (oc_init_put(light_1, &light_server, NULL, &put_light, HIGH_QOS, NULL)) {
        oc_rep_start_root_object();
        oc_rep_set_boolean(root, state, !light_state);
        oc_rep_end_root_object();
        if (oc_do_put()) {
            PRINT("client_oic: Sent PUT request\n");
        }
        else {
            PRINT("client_oic: Could not send PUT\n");
        }
    }
    else {
        PRINT("client_oic: Could not init PUT\n");
    }
    xtimer_usleep(140000);
    gpio_irq_enable(BUTTON_GPIO);
}
#endif

static oc_event_callback_retval_t
operate(void *data)
{
    PRINT("client_oic: Ready...\n");
    set_led_state(true);
    oc_do_observe(light_1, &light_server, NULL, &observe_light, LOW_QOS, NULL);
#ifdef BUTTON_GPIO
    gpio_init_int(BUTTON_GPIO, GPIO_IN_PU, GPIO_RISING, (gpio_cb_t)button_callback, NULL);
#endif
    oc_set_delayed_callback(NULL, &stop_observe, 10000);
    return DONE;
}

static oc_event_callback_retval_t
do_discovery(void *data)
{
    if (got_discovery_response) {
        PRINT("client_oic: Discovery done\n");
        oc_set_delayed_callback(NULL, &operate, 1);
        return DONE;
    }
    oc_do_ip_discovery("oic.r.light", &discovery, NULL);
    PRINT("client_oic: continue discovery\n");
    return CONTINUE;
}

static void
issue_requests(void)
{
    oc_set_delayed_callback(NULL, &do_discovery, 1);
}

static void
signal_event_loop_func(void)
{
    mutex_lock(&mutex);
    pthread_cond_signal(&cv);
    mutex_unlock(&mutex);
}

void *
oc_main_thread(void *arg)
{
    (void)arg;

    pthread_cond_init(&cv, NULL);

    static const oc_handler_t handler = { .init = app_init,
                                          .signal_event_loop = signal_event_loop_func,
                                          .requests_entry = issue_requests };
    if (oc_main_init(&handler) < 0) {
        PRINT("client_oic: failed to initialize stack\n");
        return NULL;
    }
    PRINT("client_oic: Configured network interfaces:");
    _netif_config(0, NULL);

    oc_clock_time_t next_event;
    while (quit == 0) {
        next_event = oc_main_poll();
        mutex_lock(&mutex);
        if (next_event == 0) {
            pthread_cond_wait(&cv, &mutex);
        }
        else if (oc_clock_time() < next_event) {
            ts.tv_sec = (next_event / OC_CLOCK_SECOND);
            ts.tv_nsec = (next_event % OC_CLOCK_SECOND) * 1.e09 / OC_CLOCK_SECOND;
            pthread_cond_timedwait(&cv, &mutex, &ts);
        }
        mutex_unlock(&mutex);
    }

    oc_main_shutdown();

    return NULL;
}

int
main(void)
{
    set_led_state(led_state);
    PRINT("client_oic: Waiting for address autoconfiguration...");
    xtimer_sleep(10);
    thread_create(_oc_main_stack, sizeof(_oc_main_stack), 2, 0, oc_main_thread,
                  NULL, "OCF event thread");

    fgetc(stdin);

    quit = 1;
    signal_event_loop_func();

    return 0;
}
