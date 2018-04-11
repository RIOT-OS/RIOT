
#define AT_EOL            "\r\n"

#include <string.h>

#include "shell.h"
#include "xtimer.h"
#include "gsm.h"
#include "board.h"

#ifndef APN
#define APN               "apn"
#endif

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

#ifndef MODEM_DTR_PIN
#define MODEM_DTR_PIN     GPIO_UNDEF
#endif

static gsm_driver_t fake_driver;

static const gsm_params_t params = {
    .uart            = UART_MODEM,
    .baudrate        = 115200,
    .ri_pin          = MODEM_RI_PIN,
};

static gsm_t modem = {
    .driver = &fake_driver,
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

    gsm_setup_pdp_context((gsm_t *)&modem, 0, GSM_CTX_IP, argv[1],
            (argv[2]) ? argv[2] : NULL, (argv[3]) ? argv[3] : NULL);
    return 0;
}

int _modem_ppp_dialout_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int result = gsm_dial((gsm_t *)&modem, GSM_PPP_NUMBER, false);

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
        printf("Usage: %s <pin>\n", argv[0]);
        return 1;
    }


    result = gsm_set_pin((gsm_t *)&modem, argv[1]);
    if(result) {
        printf("Simcard unlocked\n");
    }

    return 0;
}

static const shell_command_t commands[] = {
    {"atcmd",        "Sends an AT cmd",     _at_send_handler},
    {"modem_status", "Print Modem status",  _modem_status_handler},
    {"init",         "Init modem",          _modem_init_handler},
    {"init_pdp",     "Init GPRS connection",_modem_init_pdp_handler},
    {"simpin",       "Enter simpin",        _modem_cpin_handler},
    {"sim_status",   "Check sim status",    _modem_cpin_status_handler},
    {"ppp_dial",     "PPP Dial out",        _modem_ppp_dialout_handler},
    {NULL, NULL, NULL}
};

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
