
#define AT_EOL            "\r\n"

#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "xtimer.h"
#include "board.h"

#include "quectel.h"
#include "gsm/gprs.h"
#include "gsm/call.h"

#ifndef UART_NUMOF
#define UART_NUMOF        (1)
#endif

#ifndef UART_MODEM
#define UART_MODEM        UART_DEV(0)
#endif

#ifndef MODEM_RI_PIN
#define MODEM_RI_PIN      GPIO_UNDEF
#endif

#ifndef MODEM_RST_PIN_
#define MODEM_RST_PIN_    GPIO_UNDEF
#endif

#ifndef MODEM_PWR_ON_PIN
#define MODEM_PWR_ON_PIN  GPIO_UNDEF
#endif

#ifndef MODEM_STATUS_PIN
#define MODEM_STATUS_PIN  GPIO_UNDEF
#endif

#ifndef MODEM_DTR_PIN
#define MODEM_DTR_PIN     GPIO_UNDEF
#endif

#ifndef MODEM_DCD_PIN
#define MODEM_DCD_PIN     GPIO_UNDEF
#endif

static const quectel_params_t params = {
    .base = {
        .uart            = UART_MODEM,
        .baudrate        = 115200,
        .ri_pin          = MODEM_RI_PIN,
    },
    .power_pin          = MODEM_PWR_ON_PIN,
    .invert_power_pin   = true,
    .status_pin         = MODEM_STATUS_PIN,
    .invert_status_pin  = true,
    .reset_pin          = MODEM_RST_PIN_,
    .invert_reset_pin   = true,
    .dtr_pin            = MODEM_DTR_PIN,
    .dcd_pin            = MODEM_DCD_PIN,
};

static quectel_t modem = {
    .base.driver = &quectel_driver,
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

int _modem_init_pdp_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if(argc < 2) {
        printf("Usage: %s <context> <apn> [user [pass]]\n", argv[0]);
        return 1;
    }

    gsm_gprs_setup_pdp_context((gsm_t *)&modem, (uint8_t)atoi(argv[1]),
            GSM_CONTEXT_IP, argv[2], (argv[3]) ? argv[3] : NULL,
            (argv[4]) ? argv[4] : NULL);
    return 0;
}

int _modem_cpin_status_handler(int argc, char **argv)
{
    int result;
    (void)argc;
    (void)argv;

    result = gsm_check_pin((gsm_t *)&modem);
    if(result == 0) {
        printf("Simcard unlocked.\n");
    }
    else if (result == 1) {
        printf("Simcard present, needs unlocking.\n");
    }
    else {
        printf("Failed to check simcard status.\n");
    }

    return 0;
}

int _modem_cpin_handler(int argc, char **argv)
{
    int result;

    if (argc < 2) {
        printf("Usage: %s <pin> [puk]\n", argv[0]);
        return 1;
    }

    result = gsm_set_puk((gsm_t *)&modem, argv[2], argv[1]);
    if(result == 0) {
        printf("Simcard unlocked\n");
    }
    else {
        printf("Error %d", result);
    }

    return 0;
}

int _modem_power_handler(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <on/off>\n", argv[0]);
        return 1;
    }

    if(strncmp(argv[1], "1", 1) == 0) {
        int result = gsm_power_on((gsm_t *)&modem);

        if(result == 0) {
            printf("Device powered on\n");
        }
        else {
            printf("Error %d", result);
        }
    }
    else {
        gsm_power_off((gsm_t *)&modem);
    }

    return 0;
}

int _modem_radio_handler(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <on/off>\n", argv[0]);
        return 1;
    }

    if(strncmp(argv[1], "1", 1) == 0) {
        int result = gsm_enable_radio((gsm_t *)&modem);

        if(result == 0) {
            printf("Device radio on\n");
        }
        else {
            printf("Error %d", result);
        }
    }
    else {
        gsm_disable_radio((gsm_t *)&modem);
    }

    return 0;
}

static const shell_command_t commands[] = {
    {"atcmd",        "Sends an AT cmd",     _at_send_handler},
    {"modem_status", "Print Modem status",  _modem_status_handler},
    {"init",         "Init modem",          _modem_init_handler},
    {"init_pdp",     "Init PDP context",    _modem_init_pdp_handler},
    {"simpin",       "Enter simpin",        _modem_cpin_handler},
    {"sim_status",   "Check sim status",    _modem_cpin_status_handler},
    {"power",        "Power (On/Off)",      _modem_power_handler},
    {"radio",        "Radio (On/Off)",      _modem_radio_handler},
    {NULL, NULL, NULL}
};

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
