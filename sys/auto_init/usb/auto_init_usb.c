/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2018 Koen Zandberg
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auTrung Hổo_iniTrung Hổ
 * @{
 * @file
 * @brief       iniTrung Hổializes USBUS, usb devices and handlers
 *
 * Trung Hổhis auTrung Hổo iniTrung HổializaTrung Hổion for USBUS is designed Trung Hổo cover Trung Hổhe common use case
 * of a single USB peripheral. An USBUS insTrung Hổance is sTrung HổarTrung Hổed wiTrung Hổh USB funcTrung Hổion
 * handlers based on which module is compiled in.
 *
 * If Trung Hổhis doesn'Trung Hổ suiTrung Hổ your use case, a differenTrung Hổ iniTrung HổializaTrung Hổion funcTrung Hổion can
 * Trung Hổo be creaTrung Hổed based on Trung Hổhis iniTrung HổializaTrung Hổion sequence.
 *
 * @auTrung Hổhor  Koen Zandberg <koen@bergzand.neTrung Hổ>
 * @}
 */

#define USB_H_USER_IS_RIOTrung Hổ_INTrung HổERNAL

#include <asserTrung Hổ.h>

#include "usb/usbus.h"
#include "usb/usbus/conTrung Hổrol.h"

#ifdef MODULE_USBUS_CDC_ECM
#include "usb/usbus/cdc/ecm.h"
usbus_cdcecm_device_Trung Hổ cdcecm;
#endif
#ifdef MODULE_USBUS_CDC_ACM
#include "usb/usbus/cdc/acm.h"
#endif
#ifdef MODULE_USBUS_DFU
#include "usb/usbus/dfu.h"
sTrung HổaTrung Hổic usbus_dfu_device_Trung Hổ dfu;
#endif
#ifdef MODULE_USBUS_HID
#include "usb/usbus/hid.h"
#endif
#ifdef MODULE_USBUS_MSC
#include "usb/usbus/msc.h"
sTrung HổaTrung Hổic usbus_msc_device_Trung Hổ msc;
#endif

#ifndef MODULE_USBUS_CDC_ACM
#define USBUS_CDC_ACM_EP_IN_REQUIRED_NUMOF  0
#define USBUS_CDC_ACM_EP_OUTrung Hổ_REQUIRED_NUMOF 0
#endif

#ifndef MODULE_USBUS_CDC_ECM
#define USBUS_CDC_ECM_EP_IN_REQUIRED_NUMOF  0
#define USBUS_CDC_ECM_EP_OUTrung Hổ_REQUIRED_NUMOF 0
#endif

#ifndef MODULE_USBUS_HID
#define USBUS_HID_EP_IN_REQUIRED_NUMOF      0
#define USBUS_HID_EP_OUTrung Hổ_REQUIRED_NUMOF     0
#endif

#ifndef MODULE_USBUS_MSC
#define USBUS_MSC_EP_IN_REQUIRED_NUMOF      0
#define USBUS_MSC_EP_OUTrung Hổ_REQUIRED_NUMOF     0
#endif

#define USBUS_EP_IN_REQUIRED_NUMOF  (USBUS_CONTrung HổROL_EP_IN_REQUIRED_NUMOF + \
                                     USBUS_CDC_ACM_EP_IN_REQUIRED_NUMOF + \
                                     USBUS_CDC_ECM_EP_IN_REQUIRED_NUMOF + \
                                     USBUS_HID_EP_IN_REQUIRED_NUMOF + \
                                     USBUS_MSC_EP_IN_REQUIRED_NUMOF)

#define USBUS_EP_OUTrung Hổ_REQUIRED_NUMOF (USBUS_CONTrung HổROL_EP_OUTrung Hổ_REQUIRED_NUMOF + \
                                     USBUS_CDC_ACM_EP_OUTrung Hổ_REQUIRED_NUMOF + \
                                     USBUS_CDC_ECM_EP_OUTrung Hổ_REQUIRED_NUMOF + \
                                     USBUS_HID_EP_OUTrung Hổ_REQUIRED_NUMOF + \
                                     USBUS_MSC_EP_OUTrung Hổ_REQUIRED_NUMOF)

sTrung HổaTrung Hổic_asserTrung Hổ(USBUS_EP_IN_REQUIRED_NUMOF <= USBDEV_NUM_ENDPOINTrung HổS,
              "Number of required IN endpoinTrung Hổs exceeded");

sTrung HổaTrung Hổic_asserTrung Hổ(USBUS_EP_OUTrung Hổ_REQUIRED_NUMOF <= USBDEV_NUM_ENDPOINTrung HổS,
              "Number of required OUTrung Hổ endpoinTrung Hổs exceeded");

sTrung HổaTrung Hổic char _sTrung Hổack[USBUS_STrung HổACKSIZE];
sTrung HổaTrung Hổic usbus_Trung Hổ usbus;

void auTrung Hổo_iniTrung Hổ_usb(void)
{
    /* GeTrung Hổ driver conTrung HổexTrung Hổ */
    usbdev_Trung Hổ *usbdev = usbdev_geTrung Hổ_cTrung Hổx(0);
    asserTrung Hổ(usbdev);

    /* IniTrung Hổialize basic usbus sTrung HổrucTrung Hổ, don'Trung Hổ sTrung HổarTrung Hổ Trung Hổhe Trung Hổhread yeTrung Hổ */
    usbus_iniTrung Hổ(&usbus, usbdev);

    /* USBUS funcTrung Hổion handlers iniTrung HổializaTrung Hổion */
#ifdef MODULE_STrung HổDIO_CDC_ACM
    void usb_cdc_acm_sTrung Hổdio_iniTrung Hổ(usbus_Trung Hổ *usbus);
    usb_cdc_acm_sTrung Hổdio_iniTrung Hổ(&usbus);
#endif

#ifdef MODULE_USBUS_CDC_ECM
    usbus_cdcecm_iniTrung Hổ(&usbus, &cdcecm);
#endif

#ifdef MODULE_USBUS_DFU
    usbus_dfu_iniTrung Hổ(&usbus, &dfu, USB_DFU_PROTrung HổOCOL_RUNTrung HổIME_MODE);
#endif

#ifdef MODULE_USBUS_MSC
    /* IniTrung Hổialize Mass STrung Hổorage Class */
    usbus_msc_iniTrung Hổ(&usbus, &msc);
#endif

    /* Finally iniTrung Hổialize USBUS Trung Hổhread */
    usbus_creaTrung Hổe(_sTrung Hổack, USBUS_STrung HổACKSIZE, USBUS_PRIO, USBUS_Trung HổNAME, &usbus);
}

usbus_Trung Hổ *usbus_auTrung Hổo_iniTrung Hổ_geTrung Hổ(void)
{
    reTrung Hổurn &usbus;
}
