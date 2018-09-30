#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "at.h"
#include "fmt.h"
#include "board.h"
#include "shell.h"
#include "xtimer.h"

#include "gsm/generic.h"
#include "gsm/gprs.h"
#include "gsm/call.h"

#include "net/ipv4/addr.h"

#ifndef MODEM_RI_PIN
#define MODEM_RI_PIN    GPIO_UNDEF
#endif

#define MAX_CMD_LEN     (256)

static const gsm_params_t params = {
    .uart            = MODEM_UART,
    .baudrate        = MODEM_BAUDRATE,
    .ri_pin          = MODEM_RI_PIN,
};

static gsm_generic_t _modem;

int _at_send_handler(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <command>, [timeout]\n", argv[0]);
        return 1;
    }

    uint8_t timeout = 20;

    if (argc > 2) {
        timeout = (uint8_t)atoi(argv[2]);
    }

    uint8_t resp[MAX_CMD_LEN + 1];
    resp[MAX_CMD_LEN] = '\0';

    gsm_cmd((gsm_t *)&_modem, argv[1], resp, MAX_CMD_LEN, timeout);
    puts((char *)resp);

    return 0;
}

int _modem_status_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    gsm_print_status((gsm_t *)&_modem);
    return 0;
}

int _modem_init_pdp_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (argc < 2) {
        printf("Usage: %s <context> <apn> [user [pass]]\n", argv[0]);
        return 1;
    }

    gsm_gprs_setup_pdp_context((gsm_t *)&_modem, (uint8_t)atoi(argv[1]),
                    GSM_CONTEXT_IP, argv[2], (argv[3]) ? argv[3] : NULL,
                    (argv[4]) ? argv[4] : NULL);
    return 0;
}

int _modem_sim_status_handler(int argc, char **argv)
{
    int result;
    (void)argc;
    (void)argv;

    result = gsm_check_pin((gsm_t *)&_modem);
    if (result == 0) {
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

    result = gsm_set_puk((gsm_t *)&_modem, argv[2], argv[1]);
    if (result == 0) {
        printf("Simcard unlocked\n");
    }
    else {
        printf("Error %d", result);
    }

    return 0;
}

int _modem_sim_handler(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <status|pin>\n", argv[0]);
    }
    else {
        if (strcmp(argv[1], "status") == 0) {
            return _modem_sim_status_handler(argc - 1, &argv[1]);
        }
        else if (strcmp(argv[1], "pin") == 0) {
            return _modem_cpin_handler(argc - 1, &argv[1]);
        }
    }
    return 0;
}

int _modem_power_handler(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <on(1)/off(0)>\n", argv[0]);
        return 1;
    }

    if (strncmp(argv[1], "1", 1) == 0) {
        int result = gsm_poweron((gsm_t *)&_modem);

        if (result == 0) {
            printf("Device powered on\n");
        }
        else {
            printf("Error %d", result);
        }
    }
    else {
        gsm_poweroff((gsm_t *)&_modem);
    }

    return 0;
}

int _modem_radio_handler(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <on(1)/off(0)>\n", argv[0]);
        return 1;
    }

    if (strncmp(argv[1], "1", 1) == 0) {
        int result = gsm_enable_radio((gsm_t *)&_modem);

        if (result == 0) {
            printf("Device radio on\n");
        }
        else {
            printf("Error %d", result);
        }
    }
    else {
        gsm_disable_radio((gsm_t *)&_modem);
    }

    return 0;
}

int _modem_gprs_attach(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <attach(1)/detach(0)>\n", argv[0]);
        return 1;
    }

    if (strncmp(argv[1], "1", 1) == 0) {
        int result = gsm_grps_attach((gsm_t *)&_modem);

        if (result == 0) {
            printf("Attached\n");
        }
        else {
            printf("Error %d", result);
        }
    }
    else {
        gsm_grps_detach((gsm_t *)&_modem);
    }

    return 0;
}

int _modem_gprs_activate(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <activate(1)/deactivate(0)>\n", argv[0]);
        return 1;
    }

    if (strncmp(argv[1], "1", 1) == 0) {
        int result = gsm_grps_activate_context((gsm_t *)&_modem, (uint8_t)atoi(argv[2]));

        if (result == 0) {
            printf("Activated\n");
        }
        else {
            printf("Error %d\n", result);
        }
    }
    else {
        gsm_grps_deactivate_context((gsm_t *)&_modem, (uint8_t)atoi(argv[2]));
    }

    return 0;
}


int _modem_datamode_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (gsm_call_switch_to_data_mode((gsm_t *)&_modem) != 0) {
        printf("Failed to switch to data mode\n");
    }

    return 0;
}

int _modem_cmdmode_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (gsm_call_switch_to_command_mode((gsm_t *)&_modem) != 0) {
        printf("Failed to switch to command mode\n");
    }

    return 0;
}

int _modem_ip_handler(int argc, char **argv)
{
    char buf[IPV4_ADDR_MAX_STR_LEN];

    if (argc < 2) {
        printf("Usage: %s <context>\n", argv[0]);
        return 1;
    }

    uint32_t ip = gsm_gprs_get_address((gsm_t *)&_modem,
                    (uint8_t)atoi(argv[1]));

    printf("Address (ipv4) %s\n",
                    ipv4_addr_to_str(buf, (ipv4_addr_t *)&ip,
                                    IPV4_ADDR_MAX_STR_LEN));

    return 0;
}

int _modem_rssi_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int rssi;
    unsigned ber;

    if (gsm_get_signal((gsm_t *)&_modem, &rssi, &ber) == 0) {
        printf("RSSI= %ddBm ber=%u%%\n", rssi, ber);
    }
    else {
        puts("Failed to get signal strength");
    }

    return 0;
}

int _modem_time_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    char buf[32];

    if (gsm_get_local_time((gsm_t *)&_modem, buf, 32) >= 0) {
        printf("%s\n", buf);
    }
    else {
        puts("Failed to get time");
    }

    return 0;
}

int _modem_gprs_handler(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <attach|activate|setup|ip>\n", argv[0]);
    }
    else {

        if (strcmp(argv[1], "attach") == 0) {
            return _modem_gprs_attach(argc - 1, &argv[1]);
        }
        else if (strcmp(argv[1], "setup") == 0) {
            return _modem_init_pdp_handler(argc - 1, &argv[1]);
        }
        else if (strcmp(argv[1], "activate") == 0) {
            return _modem_gprs_activate(argc - 1, &argv[1]);
        }
        else if (strcmp(argv[1], "ip") == 0) {
            return _modem_ip_handler(argc - 1, &argv[1]);
        }
    }
    return 0;
}


static const shell_command_t commands[] = {
    {"atcmd",        "Sends an AT cmd",     _at_send_handler},
    {"status",       "Print Modem status",  _modem_status_handler},
    {"gprs",         "GPRS operations",    _modem_gprs_handler},
    {"sim",          "Sim operations",      _modem_sim_handler},
    {"power",        "Power (On/Off)",      _modem_power_handler},
    {"radio",        "Radio (On/Off)",      _modem_radio_handler},
    {"datamode",     "Switch to datamode",  _modem_datamode_handler },
    {"cmdmode",      "Switch to commandmode",_modem_cmdmode_handler },
    {"rssi",         "Get rssi",             _modem_rssi_handler },
    {"time",         "Get time",            _modem_time_handler },
    {NULL, NULL, NULL}
};

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    int err = gsm_init((gsm_t *)&_modem, (gsm_params_t *)&params, &gsm_generic_driver);

    if (err < 0) {
        printf("Failed to initialize gsm driver with error %d\n", err);
        return -1;
    }

    /* start the shell */
    puts("Initialization OK, starting shell now");

    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
