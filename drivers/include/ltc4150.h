#ifndef __LTC4150_H
#define __LTC4150_H 

#include <ltc4150_arch.h>

void ltc4150_init();
void ltc4150_start();
void ltc4150_stop();

double ltc4150_get_current_mA();
double ltc4150_get_total_mAh();
double ltc4150_get_total_Joule(void);
double ltc4150_get_avg_mA();
int ltc4150_get_interval();
long ltc4150_get_intcount(void);

#endif /* __LTC4150_H */
