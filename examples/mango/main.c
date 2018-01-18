#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#include "mango.h"

#define STACK_SIZE 32
#define MEMORY_SIZE 128
static uint8_t memory[MEMORY_SIZE];

#define MODULE(Fingerprint, Name, Identifier, Length, Image)                   \
    static const uint8_t program_name[12] = Fingerprint;                            \
    static const uint8_t program_code[Length] = Image;
#include "program.inc"
#undef MODULE

int main(void)
{
    mango_vm *vm = mango_initialize(memory, sizeof(memory), STACK_SIZE, NULL);
    if (!vm) {
        printf("Failed to initialize vm\n");
        return 0;
    }

    int result = mango_module_import(vm,
                                     program_name,
                                     program_code,
                                     sizeof(program_code),
                                     NULL);
    if (result) {
        printf("Failed to import module (Errorcode: %d)\n", result);
        return 0;
    }

    int sensor_value = 50;
    while (1) {
        int vm_result = mango_run(vm);
        int call_id;
        switch (vm_result) {
            default:
                printf("VM stopped with unknown returnvalue: %d\n", vm_result);
                return 0;
            case MANGO_E_SUCCESS:
                printf("VM executed succsesfully\n");
                return 0;
            case MANGO_E_SYSTEM_CALL:
                call_id = mango_syscall(vm);
                switch (call_id) {
                    case 100:
                        printf("SYSgetSensorValue: %d\n", sensor_value);
                        *(int32_t *)mango_stack_alloc(vm, 4, 0) = sensor_value;
                        sensor_value = (sensor_value + 80) % 300;
                        break;
                    case 101:
                        printf("SYSsetLight: %" PRIi32 "\n", *(int32_t *)mango_stack_top(vm));
                        mango_stack_free(vm, sizeof(int32_t));
                        break;
                    default:
                        printf("Systemcall %d", call_id);
                        break;
                }
                break;
        }
    }
}
