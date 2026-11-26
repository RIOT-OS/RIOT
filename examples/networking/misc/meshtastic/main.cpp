//Standard Libraries
#include <stdio.h>
#include <string>

//offered by RIOT
#include "ztimer.h"
#include "sx127x_params.h"
#include "radiolib_riotos.h"
#include "radiolib_riotos_params.h"
#include "board.h"

//Offered by RadioLib
#include <RadioLib.h>

//Offered by Libmeshtasticleaf
#include "libmeshtastic_leaf.h"

int main(void) {
	/* Sleep so that we do not miss this message while connecting */
	ztimer_sleep(ZTIMER_SEC, 1);

	/**
	millis() can be defined as ztimer_now(ZTIMER_MSEC), be sure to include "ztimer.h"
	*/
	puts("Starting meshtastic leaf on RIOT-OS");
    printf("You are running RIOT on a(n) %s board. CPU is %s.\n", RIOT_BOARD, RIOT_CPU);
	//or initialize HAL using RIOT's HAL
	spi_t some_spi = SX127X_PARAM_SPI; //Resolve to SPI_DEV(?). For default use CONFIG_RADIOLIB_SPI
	spi_clk_t spi_clock = CONFIG_RADIOLIB_SPI_CLK; //Resolve to SPI_CLK_1MHZ. For default use CONFIG_RADIOLIB_SPI_CLK
	RadioLibHal* hal = new RiotHal(some_spi, spi_clock);
	printf("RadioLib HAL initialized.\n");

	uint32_t cs = CONFIG_RADIOLIB_SPI_NSS;
	uint32_t irq = CONFIG_RADIOLIB_IRQ;
	uint32_t rst = CONFIG_RADIOLIB_RESET;
	uint32_t gpio = CONFIG_RADIOLIB_GPIO;
	Module* module = new Module(hal, cs, irq, rst, gpio);
	printf("Radio Module initialized.\n");
	SX1276* radio = new SX1276(module);
	printf("SX1276 Radio initialized.\n");
	

	//radio band: is application specific
	//spread factor: higher -> range but higher time on air. More infos in loratools.nl
	// patch can be an idea if the version of radio changes

	radio->begin(434.0f, 125.0f, 9, 7, RADIOLIB_SX127X_SYNC_WORD, 10, 8, 1.8f);
	radio->setCRC(2);
	radio->setCurrentLimit(100.0f); //look the radio datasheet
	printf("Radio settings applied.\n");

	//configure meshtastic
	libmeshtastic_leaf::MeshConfig config;
	config.hopLimit = 3;
	config.radio.region = libmeshtastic_leaf::REGION_EU_868;
	config.radio.preset = libmeshtastic_leaf::PRESET_LONG_FAST;
	printf("Meshtastic config is set.\n");

	//setup keys
	uint8_t publicKey[32], privateKey[32];
	libmeshtastic_leaf::libmeshtastic_leaf::generateKeyPair(publicKey, privateKey);
	printf("Meshtastic keys generated.\n");

	//TODO issue here
	libmeshtastic_leaf::libmeshtastic_leaf* mesh = new libmeshtastic_leaf::libmeshtastic_leaf();
	printf("Meshtastic leaf initialized.\n");

	mesh->setMyPublicKey(publicKey);
	mesh->setMyPrivateKey(privateKey);
	printf("Meshtastic keys set.\n");

	//start meshtastic
	mesh->begin(config, radio);
	mesh->setDefaultChannel();
	printf("Meshtastic started!\n");
	LED1_TOGGLE;
	printf("Waiting for next packets...\n");
	while (1) {
		mesh->update();
		if (mesh->available()) {
			libmeshtastic_leaf::MeshPacket packet;
			if (mesh->receive(packet) == libmeshtastic_leaf::ReceiveResult::OK) {
				printf("%s\n", packet.payload);
				LED0_TOGGLE;
				ztimer_sleep(ZTIMER_MSEC, 500);
				LED0_TOGGLE;
				ztimer_sleep(ZTIMER_MSEC, 500);
				LED0_TOGGLE;
				ztimer_sleep(ZTIMER_MSEC, 500);
				LED0_TOGGLE;
				printf("Waiting for next packets...\n");
			}
		}
	}
}
