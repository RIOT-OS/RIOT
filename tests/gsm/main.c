
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "xtimer.h"
#include "ublox.h"
#include "board.h"

#include "gsm/gprs.h"
#include "gsm/sms.h"

#define APN "apn"

#ifndef UART_MODEM
#define UART_MODEM      UART_DEV(1)
#endif

#ifndef MODEM_RI_PIN
#define MODEM_RI_PIN    GPIO_UNDEF
#endif

#ifndef MODEM_RST_PIN_
#define MODEM_RST_PIN_  GPIO_UNDEF
#endif

#ifndef MODEM_PWR_ON_PIN
#define MODEM_PWR_ON_PIN    GPIO_UNDEF
#endif

#ifndef MODEM_DTR_PIN
#define MODEM_DTR_PIN   GPIO_UNDEF
#endif

static const ublox_params_t params = {
    .base.uart = UART_MODEM,
    .base.baudrate = 115200,
    .base.ri_pin = MODEM_RI_PIN,
    .change_over_baudrate = 921600/*460800*/,
    .reset_pin = MODEM_RST_PIN_,
    .pwr_on_pin = MODEM_PWR_ON_PIN,
    .dtr_pin = MODEM_DTR_PIN,
    .gps_connected = true,
    .gpio1_mode = UBLOX_GPIO_MODE_OUTPUT | UBLOX_GPIO_OUTPUT_HIGH,
    .gpio2_mode = UBLOX_GPIO_MODE_DISABLED,
    .gpio3_mode = UBLOX_GPIO_MODE_DEFAULT,
    .gpio4_mode = UBLOX_GPIO_MODE_GNSS_SUPPLY_EN,
};

static ublox_t modem = {
    .base.driver = &ublox_driver,
};

static gsm_sms_t sms;

#define MAX_CMD_LEN 128
int _at_send_handler(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <command>\n", argv[0]);
        return 1;
    }

    uint8_t resp[MAX_CMD_LEN + 1];
    resp[MAX_CMD_LEN] = '\0';

    gsm_cmd((gsm_t *)&modem, argv[1], resp, sizeof(resp), 20);
    puts((char *)resp);

    return 0;
}

int _modem_status_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    gsm_print_status((gsm_t *)&modem);
    return 0;
}

int _modem_init_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    gsm_init((gsm_t *)&modem, (gsm_params_t *)&params);

    gsm_sms_init(&sms, (gsm_t *)&modem);

    return 0;
}

int _modem_init_pdp_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    uint8_t context = (uint8_t)atoi(argv[1]);

    if(argc < 2) {
        printf("Usage: %s <context> <apn> [user [pass]]\n", argv[0]);
        return 1;
    }

    if(gsm_gprs_setup_pdp_context((gsm_t *)&modem, (context),
            GSM_CONTEXT_IP, argv[2], (argv[3]) ? argv[3] : NULL,
            (argv[4]) ? argv[4] : NULL) == 0) {

        if(gsm_grps_attach((gsm_t *)&modem) != 0) {
            printf("failed to attach gprs");
            return 0;
        }

        if(gsm_grps_activate_context((gsm_t *)&modem, context) != 0) {
            printf("failed to activate gprs context %u", context);
            return 0;
        }
    }

    return 0;
}



static void sms_cb(void *arg, char *sms, char *sender, char *date)
{
    (void)arg;
    printf("New SMS: %s, sender=%s, date=%s\n", sms, sender, date);
}

int _modem_sms_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    gsm_sms_set_format(&sms, GSM_SMS_MESSAGE_FORMAT_TEXT);

    gsm_sms_set_characters(&sms, NULL);

    gsm_sms_enable_reception(&sms, sms_cb, NULL);
    return 0;
}

int _modem_get_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Not implemented yet.");

    /*
    if (argc < 2) {
        printf("Usage: %s <url> [resp_file]\n", argv[0]);
        return 1;
    }

    if (argc == 2) {
        uint8_t buf[256];
        if (gsm_http_get((gsm_t *)&modem, argv[1], buf, sizeof(buf), NULL) > 0) {
            printf("Result:\n%s\n", (char *)buf);
        }
        else {
            puts("Error");
        }
        return 0;
    }
    else if (argc == 3) {
        if (gsm_http_get_file((gsm_t *)&modem, argv[1], argv[2], NULL) > 0) {
            puts("Success");
        }
        else {
            puts("Error");
        }
        return 0;
    }
*/
    return 0;
}

int _modem_post_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Not implemented yet.");

    /*
    if (argc < 3) {
        printf("Usage: %s <url> <req_file> [resp_file]\n", argv[0]);
        return 1;
    }

    if (argc == 3) {
        uint8_t buf[256];
        if (gsm_http_post_file((gsm_t *)&modem, argv[1], argv[2], buf, sizeof(buf), NULL) > 0) {
            printf("Result:\n%s\n", (char *)buf);
        }
        else {
            puts("Error");
        }
        return 0;
    }
    else if (argc == 4) {
        if (gsm_http_post_file_2((gsm_t *)&modem, argv[1], argv[2], argv[3], NULL) > 0) {
            puts("Success");
        }
        else {
            puts("Error");
        }
        return 0;
    }
    */
    return 0;
}

static const shell_command_t commands[] = {
    {"atcmd", "Sends an AT cmd", _at_send_handler},
    {"modem_status", "Print Modem status", _modem_status_handler},
    {"init", "Init modem", _modem_init_handler},
    {"init_gprs", "Init GPRS connection", _modem_init_pdp_handler},
    {"sms", "Enable sms reception", _modem_sms_handler},
    {"get", "Send http get request", _modem_get_handler},
    {"post", "Send http post request", _modem_post_handler},
    {NULL, NULL, NULL}
};

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
