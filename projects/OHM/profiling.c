#include "cpu.h"
#include "ohm.h"

#define TIMER_REG (T0TC)
#define P10_TOGGLE (FIO1PIN ^= BIT0)

void __attribute__((__no_instrument_function__))  __cyg_profile_func_enter (void *func,  void *caller) {
    extern ohm_measurement_t *func_buf_ptr;
    extern ohm_measurement_t func_buf[];

    extern uint32_t otto_says;
    extern uint8_t measure_pause;

    P10_TOGGLE;
    if (measure_pause) {
        return;
    }
    func_buf_ptr->timestamp  = TIMER_REG;
    func_buf_ptr->value      = (uint32_t) func;

    if ((++func_buf_ptr - func_buf) > ((uint32_t) CRIT_FUNC_BUF_SIZE)) {
        /* output */
        //otto_says = (func_buf_ptr - func_buf);
        otto_says = (uint32_t) func_buf;
    }
}
 
void __attribute__((__no_instrument_function__)) __cyg_profile_func_exit (void *func, void *caller) {
}
