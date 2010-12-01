#include <stdio.h>
#include <config.h>

void _id_handler(char *id) {
    uint16_t newid;

    if (sscanf(id, "id %hu", &newid) == 1) {
        printf("Setting new id %u\n", newid);
        sysconfig.id = newid;
        if (!config_save()) {
            puts("ERROR setting new id");
        }
    }
    else {
        printf("Current id: %u\n", sysconfig.id);
    }
}
