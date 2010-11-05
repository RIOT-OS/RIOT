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
#endif

#ifdef MODULE_LTC4150
extern void _get_current_handler(char* unused);
extern void _reset_current_handler(char* unused);
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
#endif
#ifdef MODULE_LTC4150
	{"cur", "Prints current and average power consumption.", _get_current_handler},
	{"rstcur", "Resets coulomb counter.", _reset_current_handler},
#endif
    {NULL, NULL, NULL}
};

