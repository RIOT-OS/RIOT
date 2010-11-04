#include <stdio.h>
#include <stdint.h>
#include <sht11.h>

void _get_humidity_handler(char* unused) {
    uint8_t success;
    sht11_val_t sht11_val;
    success = sht11_read_sensor(&sht11_val, HUMIDITY|TEMPERATURE);
    if (!success) {
        printf("Error reading SHT11\n");
    }
    else {
        printf("Relative humidity: %5.2f%% / Temperature compensated humidity; %5.2f%%\n",
                sht11_val.relhum, sht11_val.relhum_temp);
    }
}
void _get_temperature_handler(char* unused) {
    uint8_t success;
    sht11_val_t sht11_val;
    success = sht11_read_sensor(&sht11_val, TEMPERATURE);
    if (!success) {
        printf("Error reading SHT11\n");
    }
    else {
        printf("Temperature: %-6.2f°C\n", sht11_val.temperature);
    }
}
void _get_weather_handler(char* unused) {
    uint8_t success;
    sht11_val_t sht11_val;
    success = sht11_read_sensor(&sht11_val, HUMIDITY|TEMPERATURE);
    if (!success) {
        printf("Error reading SHT11\n");
    }
    else {
        printf("Relative humidity: %5.2f%% / Temperature compensated humidity; %5.2f%% ",
                sht11_val.relhum, sht11_val.relhum_temp);
        printf("Temperature: %-6.2f°C\n", sht11_val.temperature);
    }
}

