/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for nx_at86rf2xx network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#if defined(MODULE_AT86RFR2)

	#include "board.h"
	#include "net/gnrc/netdev2.h"
	#include "net/gnrc/netdev2/ieee802154.h"
	#include "net/gnrc.h"

	#include "at86rf2xx.h"

	#include "at86rfr2_params.h"



	#define ENABLE_DEBUG (0)
	#include "debug.h"

	/**
	 * @brief   Define stack parameters for the MAC layer thread
	 * @{
	 */
	#define AT86RF2XX_MAC_STACKSIZE     (512) // (THREAD_STACKSIZE_DEFAULT)
	#ifndef AT86RF2XX_MAC_PRIO
		#define AT86RF2XX_MAC_PRIO          (GNRC_NETDEV2_MAC_PRIO)
	#endif

	#define AT86RFRF2_NUM (1)

	static at86rf2xx_t at86rfr2_devs[AT86RFRF2_NUM];
	static gnrc_netdev2_t gnrc_adpt[AT86RFRF2_NUM];
	static char _at86rf2xx_stacks[AT86RF2XX_MAC_STACKSIZE][AT86RFRF2_NUM];

	void auto_init_at86rfr2(void)
	{
		for (int i = 0; i < AT86RFRF2_NUM; i++) {
	        const at86rf2xx_params_t *p = &at86rfr2_params[i];
	        int res;

	        DEBUG("Initializing AT86RFR2 with SOC radio no SPI needed.\n");
	        at86rf2xx_setup(&at86rfr2_devs[i], (at86rf2xx_params_t*) p);
			res = gnrc_netdev2_ieee802154_init(&gnrc_adpt[i],
											   (netdev2_ieee802154_t *)&at86rfr2_devs[i] );

			if (res < 0) {
				DEBUG("Error initializing AT86RFR2 radio device!");
			}
			else {
				gnrc_netdev2_init(_at86rf2xx_stacks[i],
								  AT86RF2XX_MAC_STACKSIZE,
								  AT86RF2XX_MAC_PRIO,
								  "at86rfr2",
								  &gnrc_adpt[i]);
			}
		}
	}

#else /* defined(MODULE_AT86RFR2) */
		typedef int dont_be_pedantic;
#endif /* MODULE_AT86RF2XX */

/** @} */
