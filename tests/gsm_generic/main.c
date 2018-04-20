
#define AT_EOL            "\r\n"

#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "xtimer.h"
#include "board.h"

#include "generic.h"
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

static const gsm_params_t params = {
    .uart            = UART_MODEM,
    .baudrate        = 115200,
    .ri_pin          = MODEM_RI_PIN,
};

static gsm_t modem = {
        .driver = &generic_driver,
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

int _modem_ppp_dialout_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int result = gsm_call_dial((gsm_t *)&modem, GSM_PPP_NUMBER, false);

    if(result >= 0) {
        printf("PPP dialout success");
    }
    else {
        printf("Failed to dialout PPP");
    }

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

int _modem_local_time_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    char buffer[64];
    int result = gsm_get_local_time((gsm_t *)&modem, buffer, 32);
    if(result > 0) {
        printf("Local Time: %s\n", buffer);
    }
    else {
        printf("Error %d", result);
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
    {"ppp_dial",     "PPP Dial out",        _modem_ppp_dialout_handler},
    {"time",         "Local Time",          _modem_local_time_handler},
    {NULL, NULL, NULL}
};

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
