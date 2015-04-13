#ifndef ADC_D_H_
#define ADC_D_H_
#ifdef __cplusplus
extern "C"{
#endif
#include <stdint.h>

void activateRelayCtrl(void);
void deactivateRelayCtrl(void);
void adcSetup(void);
void i2cSetup(void);
int getAdcValue(void);
int getI2cValue(uint8_t);
uint32_t getWeightValueFromAdcInput(uint32_t);
double getBatteryValueFromAdcInput(int);
void setWeightTara(void);
#ifdef __cplusplus
}
#endif
#endif /* UDP_H_ */
