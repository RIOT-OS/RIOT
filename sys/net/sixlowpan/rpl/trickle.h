#include <vtimer.h>
#include <thread.h>

#define TRICKLE_TIMER_STACKSIZE 3072 
#define TRICKLE_INTERVAL_STACKSIZE 3072

void reset_trickletimer(void);
void init_trickle(void);
void start_trickle(uint8_t DIOINtMin, uint8_t DIOIntDoubl, uint8_t DIORedundancyConstatnt);
void trickle_increment_counter(void);
void trickle_timer_over(void);
void trickle_interval_over(void);

