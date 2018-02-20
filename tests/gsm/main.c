
#include <string.h>

#include "shell.h"
#include "xtimer.h"
#include "gsm/ublox.h"
#include "board.h"

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

static const ublox_gsm_params_t params = {
    .base.uart = UART_MODEM,
    .base.baudrate = 115200,
    .base.baudrate_to_set = 921600/*460800*/,
    .base.ri_pin = MODEM_RI_PIN,
    .reset_pin = MODEM_RST_PIN_,
    .pwr_on_pin = MODEM_PWR_ON_PIN,
    .dtr_pin = MODEM_DTR_PIN,
};

static ublox_gsm_t modem = {
    .base.driver = &ublox_gsm_driver,
};

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
    return 0;
}

int _modem_init_gprs_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    gsm_gprs_init((gsm_t *)&modem, APN, NULL, NULL);
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

    gsm_receive_sms((gsm_t *)&modem, sms_cb, NULL);
    return 0;
}

int _modem_get_handler(int argc, char **argv)
{
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

    return 1;
}

int _modem_post_handler(int argc, char **argv)
{
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
    return 0;
}

static const shell_command_t commands[] = {
    {"atcmd", "Sends an AT cmd", _at_send_handler},
    {"modem_status", "Print Modem status", _modem_status_handler},
    {"init", "Init modem", _modem_init_handler},
    {"init_gprs", "Init GPRS connection", _modem_init_gprs_handler},
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
