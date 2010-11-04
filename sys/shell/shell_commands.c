#include <shell.h>
#include <stdlib.h>

#ifdef MODULE_PS
extern void _ps_handler(char* unused);
#endif

#ifdef MODULE_RTC
extern void _gettime_handler(char* unused);
extern void _settime_handler(char* now);
#endif

#ifdef MODULE_SHT11
extern void _get_temperature_handler(char* unused);
extern void _get_humidity_handler(char* unused);
extern void _get_weather_handler(char* unused);
#endif

const shell_command_t _shell_command_list[] = {
#ifdef MODULE_PS
    {"ps", "Prints information about running threads.", _ps_handler},
#endif
#ifdef MODULE_RTC
    {"gettime", "Prints current date and time.", _gettime_handler},
    {"settime", "Sets current time.", _settime_handler},
#endif
#ifdef MODULE_SHT11
    {"gettemp", "Prints measured temperature.", _get_temperature_handler},
    {"gethum", "Prints measured humidity.", _get_humidity_handler},
    {"getweather", "Prints measured humidity and temperature.", _get_weather_handler},
#endif
    {NULL, NULL, NULL}
};

