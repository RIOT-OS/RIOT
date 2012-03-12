#include <vtimer.h>
#include <thread.h>

#define TRICKLE_TIMER_STACKSIZE 3072 
//#define TRICKLE_TIMER_STACKSIZE 4096 
#define TRICKLE_INTERVAL_STACKSIZE 3072
//#define DAO_DELAY_STACKSIZE 2048
#define DAO_DELAY_STACKSIZE 3072
//#define DAO_DELAY_STACKSIZE 4096
#define RT_STACKSIZE 512

void reset_trickletimer(void);
void init_trickle(void);
void start_trickle(uint8_t DIOINtMin, uint8_t DIOIntDoubl, uint8_t DIORedundancyConstatnt);
void trickle_increment_counter(void);
void trickle_timer_over(void);
void trickle_interval_over(void);
void delay_dao(void);
void dao_delay_over(void);
void rt_timer_over(void);
