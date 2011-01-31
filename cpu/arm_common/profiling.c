#include <stdio.h>
#include <stdint.h>
#include <cpu.h>
#include <ltc4150.h>

#define MAX_TRACED_FUNCTIONS    (32)
#define PROFILING_STACK_SIZE    (16)

typedef struct {
    uint32_t address;
    uint32_t time;
    uint32_t start_time;
    double consumption;
    double consumption_start;
    uint16_t counter;
} profiling_info_t;

static profiling_info_t functions[MAX_TRACED_FUNCTIONS];
static uint8_t profiling_stack[PROFILING_STACK_SIZE];
static uint8_t profiling_sp = 0;

static uint8_t traced_functions = 0;
static uint8_t profiling = 0;

void __attribute__((__no_instrument_function__)) profiling_init(void) {
    uint8_t i;
    for (i = 0; i < MAX_TRACED_FUNCTIONS; i++) {
        functions[i].address = 0;
        functions[i].time = 0;
        functions[i].consumption = 0;
        functions[i].counter = 0;
    }
    for (i = 0; i < PROFILING_STACK_SIZE; i++) {
        profiling_stack[i] = 0;
    }

    ltc4150_start();
    
    profiling = 1;
}

static int16_t __attribute__((__no_instrument_function__)) get_function_index(uint32_t addr) {
    uint8_t i;
    for (i = 0; i < MAX_TRACED_FUNCTIONS; i++) {
        if (functions[i].address == addr) {
            return i;
        }
    }
    return -1;
}

void __attribute__((__no_instrument_function__))  __cyg_profile_func_enter (void *func,  void *caller) {
    if (!profiling) {
        return;
    }
    /*
    uint8_t i;
    int16_t idx = -1;
    for (i = 0; i < MAX_TRACED_FUNCTIONS; i++) {
        if (functions[i].address == (uint32_t) func) {
            idx = i;
            break;
        }
    }
    */
    uint8_t idx = get_function_index((uint32_t) func);
    if ((idx < 0) && (traced_functions < MAX_TRACED_FUNCTIONS)) {
        idx = traced_functions++;
        functions[idx].address = (uint32_t) func;
    }
    else if (idx < 0) {
        return;
    }
    functions[idx].start_time = T0TC;
    functions[idx].counter++;
    functions[idx].consumption_start = ltc4150_get_total_mAh();
    // functions[idx].consumption_start = ltc4150_get_intcount();
}
 
void __attribute__((__no_instrument_function__)) __cyg_profile_func_exit (void *func, void *caller) {
    if (!profiling) {
        return;
    }
    /*
    uint8_t i;
    int16_t idx = -1;
    for (i = 0; i < MAX_TRACED_FUNCTIONS; i++) {
        if (functions[i].address == (uint32_t) func) {
            idx = i;
            break;
        }
    }
    */
    uint8_t idx = get_function_index((uint32_t) func);
    if (idx >= 0) {
        functions[idx].time += T0TC - functions[idx].start_time;
        //functions[idx].consumption += ltc4150_get_intcount() - functions[idx].consumption_start;
        functions[idx].consumption += ltc4150_get_total_mAh() - functions[idx].consumption_start;
    }
}

void profiling_stats(void) {
    uint8_t i;
    for (i = 0; i < traced_functions; i++) {
//            printf("Function @%04lX was running %u times for %lu ticks, consuming %li ltc-ticks\n", functions[i].address, functions[i].counter, functions[i].time, functions[i].consumption);
            printf("Function @%04lX was running %u times for %lu ticks, consuming %lf mAh\n", functions[i].address, functions[i].counter, functions[i].time, functions[i].consumption);
    }
    puts("________________________________________________________");
}
