#ifdef MODULE_CC26X2_CC13X2_RF
/*
 *  =============================== Power ===============================
 */
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>

const PowerCC26X2_Config PowerCC26X2_config = {
    .policyInitFxn      = NULL,
    .policyFxn          = &PowerCC26XX_standbyPolicy,
    .calibrateFxn       = &PowerCC26XX_calibrate,
    .enablePolicy       = true,
    .calibrateRCOSC_LF  = true,
    .calibrateRCOSC_HF  = true,
};

/*
 *  =============================== RF Driver ===============================
 */
#include <ti/drivers/rf/RF.h>

const RFCC26XX_HWAttrsV2 RFCC26XX_hwAttrs = {
    .hwiPriority        = ~0,       /* Lowest HWI priority */
    .swiPriority        = 0,        /* Lowest SWI priority */
    .xoscHfAlwaysNeeded = true,     /* Keep XOSC dependency while in standby */
    .globalCallback     = NULL,     /* No board specific callback */
    .globalEventMask    = 0         /* No events subscribed to */
};

#endif
