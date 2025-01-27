#include "net/nfc/nfct/nfct.h"
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include "board.h"
#include "log.h"
#include "ztimer.h"

#define BUFFER_SIZE 1024

static uint8_t ndef_mem[BUFFER_SIZE];
static uint8_t t2t_mem[BUFFER_SIZE];

#if MODULE_FMT
#  define MIN_SIZE (THREAD_STACKSIZE_TINY)
#else
#  define MIN_SIZE (THREAD_STACKSIZE_TINY + THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

void print_stack_usage_metric(const char *name, void *stack, unsigned max_size)
{
    unsigned free = measure_stack_free_internal(stack, max_size);

    if ((LOG_LEVEL >= LOG_INFO) &&
        (thread_get_stacksize(thread_get_active()) >= MIN_SIZE)) {
#if MODULE_FMT
        print_str("{ \"threads\": [{ \"name\": \"");
        print_str(name);
        print_str("\", \"stack_size\": ");
        print_u32_dec(max_size);
        print_str(", \"stack_used\": ");
        print_u32_dec(max_size - free);
        print_str("}]}\n");
#else
        printf(
            "{ \"threads\": [{ \"name\": \"%s\", \"stack_size\": %u, \"stack_used\": %u }]}\n",
            name, max_size, max_size - free);
#endif
    }
}

#ifdef DEVELHELP
void test_utils_print_stack_usage(void)
{
    for (kernel_pid_t i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; i++) {
        thread_t *p = thread_get(i);

        if (p == NULL) {
            continue;
        }
        print_stack_usage_metric(thread_get_name(p), thread_get_stackstart(p), thread_get_stacksize(p));
    }
}
#endif

static bool test_nfct(void)
{
    puts("Starting NFC T2T test");
    ndef_t ndef_message;

    ndef_init(&ndef_message, ndef_mem, 1024);
    ndef_add_text_record(&ndef_message, "Hello World", 11, "en", 2, UTF8);
    ndef_add_uri_record(&ndef_message, NDEF_URI_HTTPS_WWW, "riot-os.org", 11);

    nfc_t2t_t t2t;
    t2t_create_type_2_tag(&t2t, NULL, NULL, NULL, NFC_T2T_STATIC_MEMORY_SIZE, t2t_mem);
    nfct_start_type_2_tag_with_ndef(&DEFAULT_T2T_EMULATOR_DEV, &t2t, &ndef_message);
    /* sleep for 10 seconds, then disable the tag */
    ztimer_sleep(ZTIMER_SEC, 10);
    nfct_stop_type_2_tag(&DEFAULT_T2T_EMULATOR_DEV);
    return true;
}

static bool test_convenience_functions(void)
{
    puts("Starting convenience tests");
    nfc_t2t_t t2t;
    nfct_start_type_2_tag_with_text(&DEFAULT_T2T_EMULATOR_DEV, &t2t, t2t_mem, BUFFER_SIZE, "Hallo Welt", "de", UTF8);
    /* sleep for 10 seconds, then disable the tag */
    ztimer_sleep(ZTIMER_SEC, 10);
    nfct_stop_type_2_tag(&DEFAULT_T2T_EMULATOR_DEV);

    nfct_start_type_2_tag_with_uri(&DEFAULT_T2T_EMULATOR_DEV, &t2t, t2t_mem, BUFFER_SIZE, NDEF_URI_HTTPS_WWW, "riot-os.org");
    test_utils_print_stack_usage();
    /* sleep for 10 seconds, then disable the tag */
    ztimer_sleep(ZTIMER_SEC, 10);
    nfct_stop_type_2_tag(&DEFAULT_T2T_EMULATOR_DEV);
    return 0;
}

int main(void)
{
    puts("Starting NFC tests");
    test_nfct();
    test_convenience_functions();
    puts("Ending NFC tests");
    return 0;
}
