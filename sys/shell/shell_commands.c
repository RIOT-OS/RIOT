#include <shell.h>
#include <stdlib.h>

#ifdef MODULE_PS
extern void _ps_handler(char* unused);
#endif

#ifdef MODULE_RTC
extern void _date_handler(char* now);
#endif

#ifdef MODULE_SHT11
extern void _get_temperature_handler(char* unused);
extern void _get_humidity_handler(char* unused);
extern void _get_weather_handler(char* unused);
extern void _set_offset_handler(char* offset);
#endif

#ifdef MODULE_LTC4150
extern void _get_current_handler(char* unused);
extern void _reset_current_handler(char* unused);
#endif

#ifdef MODULE_CC110X
extern void _cc1100_get_address_handler(char *unused);
extern void _cc1100_set_address_handler(char *ptr);
#endif

#ifdef MODULE_TRANSCEIVER
#ifdef MODULE_CC110X_NG
extern void _cc1100_ng_get_set_address_handler(char *addr);
extern void _cc1100_ng_get_set_channel_handler(char *chan);
extern void _cc1100_ng_send_handler(char *pkt);
#endif
#endif

const shell_command_t _shell_command_list[] = {
#ifdef MODULE_PS
    {"ps", "Prints information about running threads.", _ps_handler},
#endif
#ifdef MODULE_RTC
    {"date", "Geets or gets current date and time.", _date_handler},
#endif
#ifdef MODULE_SHT11
    {"temp", "Prints measured temperature.", _get_temperature_handler},
    {"hum", "Prints measured humidity.", _get_humidity_handler},
    {"weather", "Prints measured humidity and temperature.", _get_weather_handler},
	{"offset", "Set temperature offset.", _set_offset_handler},
#endif
#ifdef MODULE_LTC4150
	{"cur", "Prints current and average power consumption.", _get_current_handler},
	{"rstcur", "Resets coulomb counter.", _reset_current_handler},
#endif
#ifdef MODULE_CC110X
    {"cc1100_get_address", "", _cc1100_get_address_handler},
    {"cc1100_set_address", "", _cc1100_set_address_handler},
#endif
#ifdef MODULE_TRANSCEIVER
#ifdef MODULE_CC110X_NG
    {"addr", "Gets or sets the address for the CC1100 transceiver", _cc1100_ng_get_set_address_handler},
    {"chan", "Gets or sets the channel for the CC1100 transceiver", _cc1100_ng_get_set_channel_handler},
    {"txtsnd", "Sends a text message to a given node via the CC1100 transceiver", _cc1100_ng_send_handler},
#endif
#endif
    {NULL, NULL, NULL}
};

