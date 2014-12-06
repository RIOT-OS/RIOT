#ifndef ADC_D_H_
#define ADC_D_H_
#ifdef __cplusplus
extern "C"{
#endif
#include <stdint.h>

void activateRelayCtrl(void);
void deactivateRelayCtrl(void);
void adcSetup(void);
int getAdcValue(void);
uint32_t getWeightValueFromAdcInput(uint32_t);
double getBatteryValueFromAdcInput(int);
void setWeightTara(void);
#ifdef __cplusplus
}
#endif
#endif /* UDP_H_ */
