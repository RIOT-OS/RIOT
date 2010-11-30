#include <stdio.h>
#include <ltc4150.h>

void _get_current_handler(char* unused) {
	printf("Power usage: %.4f mA (%.4f mA avg/ %.4f mAh total / %i usec)\n", ltc4150_get_current_mA(), ltc4150_get_avg_mA(), ltc4150_get_total_mAh(), ltc4150_get_interval());
}

void _reset_current_handler(char* unused) {
	ltc4150_start();
}
