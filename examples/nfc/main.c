#include "net/nfc/nfct/nfct.h"
#include <stddef.h>
#include <stdio.h>
#include "board.h"
#include "log.h"
#include "ztimer.h"
#include "phydat.h"
#include "saul_reg.h"
#include "fmt.h"

#define BUFFER_SIZE   NFC_T2T_STATIC_MEMORY_SIZE
#define TEXT_BUF_SIZE 20

static uint8_t t2t_mem[BUFFER_SIZE];
static char text_buf[TEXT_BUF_SIZE];
static char temp[TEXT_BUF_SIZE / 2];
static char unit[TEXT_BUF_SIZE / 2];

/**
 * The following function is copied here because including the module doesn't
 * work with our threading layout.
 */
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

static void write_temp_to_text_buf(phydat_t *temperature, char *text_buf)
{
    memset(temp, '\0', 10);
    memset(unit, '\0', 10);
    memset(text_buf, '\0', TEXT_BUF_SIZE);
    fmt_s16_dfp(temp, temperature->val[0], temperature->scale);
    phydat_unit_write(unit, 20, temperature->unit);
    sprintf(text_buf, "%s %s", temp, unit);
    //printf("Text Buffer contains: %s\n", text_buf);
    return;
}

/**
 * @brief Provide current temperature measurement via NFC Type 2 Tag.
 * Updates temperature every minute.
 * 
 * @return int 
 */
int main(void)
{
    /* start by finding a temperature sensor in the system */
    saul_reg_t *temp_sensor = saul_reg_find_type(SAUL_SENSE_TEMP);
    if (!temp_sensor) {
        puts("No temperature sensor present");
        return 1;
    }
    else {
        printf("Found temperature device: %s\n", temp_sensor->name);
    }

    nfc_t2t_t tag;
    phydat_t temperature;
    int dimensions;

    while (1) {
        LED1_OFF;
        /* read a temperature value from the sensor */
        dimensions = saul_reg_read(temp_sensor, &temperature);
        if (dimensions < 1) {
            puts("Error reading a value from the device");
            break;
        }

        /* dump the read value to STDIO */
        phydat_dump(&temperature, dimensions);
        write_temp_to_text_buf(&temperature, text_buf);

        /*Create Tag with temperature as text*/
        nfct_start_type_2_tag_with_text(&DEFAULT_T2T_EMULATOR_DEV, &tag, t2t_mem, BUFFER_SIZE, text_buf, "de", UTF8);

        LED1_ON;
        ztimer_sleep(ZTIMER_SEC, 5);
        test_utils_print_stack_usage();
        t2t_dump_tag_memory(&tag);

        /*Stop tag emulation*/
        nfct_stop_type_2_tag(&DEFAULT_T2T_EMULATOR_DEV);
    }
}
