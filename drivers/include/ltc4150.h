#ifndef __LTC4150_H
#define __LTC4150_H

#include "ltc4150_arch.h"

void ltc4150_init(void);
void ltc4150_start(void);
void ltc4150_stop(void);

double ltc4150_get_current_mA(void);
double ltc4150_get_total_mAh(void);
double ltc4150_get_total_Joule(void);
double ltc4150_get_avg_mA(void);
int ltc4150_get_interval(void);
long ltc4150_get_intcount(void);

#endif /* __LTC4150_H */
