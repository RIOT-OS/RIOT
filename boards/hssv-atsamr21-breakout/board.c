#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* configure the antenna diversity pins of the RF frontend to be controlled
     * by the AT86RF233 */
    gpio_init_mux(GPIO_PIN(PA, 12), GPIO_MUX_F);  //Set ANT_DIV_P to be used as FECTRL[2]
    gpio_init_mux(GPIO_PIN(PA, 9), GPIO_MUX_F);   //Set ANT_DIV_N to be used as FECTRL[1]

    /* enable the peripheral clock mask for the RFCTRL */
    PM->APBCMASK.bit.RFCTRL_ = 1;

    /* configure the RFCTRL peripheral to route the correct AT86RF233 signals to
     * the radio frontend */
    RFCTRL->FECFG.bit.F2CFG = 0;  //Set DIG1 to FECTRL[2]
    RFCTRL->FECFG.bit.F1CFG = 1;  //Set DIG2 to FECTRL[1]

    /* initialize the CPU */
    cpu_init();
}
