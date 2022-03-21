# Shell command for testing the Semtech SX1280 Raw Driver


## Description
This shell command acts as a LoRa 2.4 GHz modem for receiving/sending LoRa frames.

The `rangetest` command can be used for long-distance range testing such as commnication from [sounding balloons](https://gricad-gitlab.univ-grenoble-alpes.fr/thingsat/public/-/blob/master/balloons/2021-04-15/README.md#balloon-3).

## Usage

The code is based on [SX1280Lib](https://os.mbed.com/teams/Semtech/code/SX1280Lib/) provided by Semtech:  SPI function calls have been adapted for RIOT SPI API.

Driver of LoRa 2.4GHz transceiver [SX1280](https://www.semtech.com/products/wireless-rf/24-ghz-transceivers/sx1280) for [RIOTOS](https://github.com/RIOT-OS/RIOT).

## Build

### Shell mode

```bash
cd tests/driver_sx1280
make BOARD=nucleo-f411re DEVADDR=0xfc00ac20
make BOARD=nucleo-f411re DEVADDR=0xfc00ac20 flash-only
```


### Range test mode

```bash
cd tests/driver_sx1280
make BOARD=nucleo-f411re RANGETEST=1 DEVADDR=0xfc00ac20 DELAY_BETWEEN_TX=10000000 
make BOARD=nucleo-f411re RANGETEST=1 DEVADDR=0xfc00ac20 DELAY_BETWEEN_TX=10000000 flash-only
```

## Commands related to SX1280

		> help                                                                 
		Command              Description                                       
		---------------------------------------                                
		echo                 Prints the input command                          
		setup                Setup the sx1280 parameters for rx/tx             
		channel              Get/Set channel frequency (in Hz)                 
		preamble             Get/Set the preamble length                       
		syncword             Get/Set the sync word                             
		invert_iq            Get/Set IQ swapping                               
		crc                  Get/Set CRC on/off                                
		payload              Get/Set Payload fix/var                           
		txpower              Get/Set the TX power                              
		invert_iq            Get/Set IQ swapping                               
		send                 Sends a message with sx1280                       
		listen               Waits for a message with sx1280                   
		rangetest            Run range test


## Test Nucleo+Lamba80 to Nucleo Nucleo+Lamba80

Node 1 : Transmitter (Nucleo+Lamba80)
```
> channel set 2422000000                                               
New channel set to 2422000000 Hz                                       
> setup 800 12 5 li                                                    
setup: setting 800KHz bandwidth                                        
[Info] setup: configuration set with success                           
> send 3 
sending "DMGGLDPHBMBFSAAU" payload (17 bytes)   (1/3)
SX1280 OnTxDone
sending "ZNPLCAQIFFCEUOWI" payload (17 bytes)   (2/3)
SX1280 OnTxDone
sending "KPERTMSPJFWGNPRK" payload (17 bytes)   (3/3)
> SX1280 OnTxDone

> invert_iq get                                                        
InvertIQ: normal                                                       
> invert_iq set inverted                                               
New InvertIQ: inverted                                                 
> send 1                                                                   
SX1280 OnTxDone                                                      
> send 1                                                                     
SX1280 OnTxDone                                                      
> send 1                                                                     
SX1280 OnTxDone                                                      
> syncword get                                                         
syncword : 0x12                                                        
> syncword set 52                                                      
New syncword set to 0x34                                               
> listen                                                               
SX1280 OnRxDone                                                      
Rx(4): 0P                                                              
 SNR=5 dBm / RSSI=-40 dBm                                              
>                                                

```

Node 2 : Receiver (Nucleo+Lamba80)
```
> channel set 2422000000                                               
New channel set to 2422000000 Hz                                       
> setup 800 12 5 li                                                    
setup: setting 800KHz bandwidth                                        
[Info] setup: configuration set with success                           
> listen                                                               
SX1280 OnRxDone                                                      
Rx(4): P                                                               
 SNR=3 dBm / RSSI=-44 dBm                                              
> listen                                                                 
SX1280 OnRxDone                                                      
Rx(4): P                                                               
 SNR=3 dBm / RSSI=-44 dBm                                              
invert_iq get                                                          
InvertIQ: normal                                                       
> listen                                                               
> invert_iq set inverted                                               
New InvertIQ: inverted                                                 
> listen                                                               
SX1280 OnRxDone                                                      
Rx(4): P                                                               
 SNR=2 dBm / RSSI=-37 dBm                                              
> listen 10                                                                                                                                
SX1280 OnRxTimeout
> syncword get                                                         
syncword : 0x12                                                        
> syncword set 52                                                      
New syncword set to 0x34                                               
> listen                                                               
SX1280 OnRxDone                                                      
Rx(4): 0P
 SNR=5 dBm / RSSI=-40 dBm                                              
>                                                

```


## TX Test to Multitech MTCDT 2G4 mCard

The module and the driver are designed to work with Multitech standard channels:

| Channel | Frequency (Hz) | Bandwidth (Hz) | Spread Factor |
|---------|----------------|----------------|---------------|
| 1       | 2403000000     | 812000         | 12            |
| 2       | 2479000000     | 812000         | 12            |
| 3       | 2425000000     | 812000         | 12            |


Warning : On MTCDT @ Lab, channel are : 2422000000, 2479000000, 2425000000

```c
// https://www.multitech.net/developer/software/lora/mtac-lora-2g4-3/
// http://www.multitech.net/developer/wp-content/uploads/2020/06/global_conf.json_.2g4.txt
#define MULTITECH_ISM2400_CHANNEL_1			2422000000
#define MULTITECH_ISM2400_CHANNEL_2			2425000000
#define MULTITECH_ISM2400_CHANNEL_3			2479000000
#define MULTITECH_ISM2400_BANDWITH			RAL_LORA_BW_800_KHZ
#define MULTITECH_ISM2400_SF				12
#define MULTITECH_ISM2400_CR				RAL_LORA_CR_LI_4_8
#define MULTITECH_ISM2400_SYNCWORD_PUBLIC	(0x12)
#define MULTITECH_ISM2400_LEN_IN_SYMB		(8)
```


```
channel set 2422000000
send 1 
channel set 2425000000
send 1
channel set 2479000000
send 1
```


## Range test

### Range with Multitech 2G4

The `rangetest` command builds frames that are compliant with the LoRaWAN-like DataUp frames.

The ABP parameters are
* Default `DEVADDR` is `fc00ac10`
* Default `APPSKEY` is `01020304cafebabe01020304cafebabe`
* Default `NWKSKEY` is `cafebabe01020304cafebabe01020304`
* Default `FCNT` is `0U`

> The `DEVADDR`, `FCTRL`, `FCNT`, `APPSKEY` and `NWKSKEY` are defined into the sx1280_utils.c file.

> They should be set a future `lorawan_context` struct that can be persisted in EEPROM.

The `rangetest` command sends periodically messages on several channels 
```
rangetest <time> <size> <sleep before next tx> <channel1 in kHz> <channel2 in kHz>  ...
```

```
setup                                                                                                                     
Setup: sf=12 bw=800000Hz cr=6/8                                                                                           
syncword get                                                                                                              
syncword : 0x12                                                                                                           
preamble get                                                                                                              
Preamble length: 8                                                                                                        
payload get                                                                                                               
Payload: var                                                                                                              
invert_iq get                                                                                                             
InvertIQ: normal                                                                                                          
crc get                                                                                                                   
CRC: on                                                                                                                   
txpower get                                                                                                               
txpower: 13                                                                                                               
txpower set 0                                                                                                             
New TX power set to 0                                                                                                     
```


The Multtech 24G channels are `2422000` kHz, `2425000` kHz and `2479000` kHz.

```
rangetest 1000 24 5 2422000 2425000 2479000                                                                                     
starting range test                                                                                                        
devaddr : 0xfc00ac10                                                                                                      
appskey : 01020304cafebabe01020304cafebabe                                                                                
nwkskey : cafebabe01020304cafebabe01020304                                                                                
New channel set to 2422000000 Hz                                                                                          
sending : channel=2422000000 iq=0 fcnt=0 frame=8010ac00fc0000000ab84e756050475142474b58bc1999ce                           
WDT started                                                                                                               
SX1280 OnTxDone                                                                                                           
listen : channel=2422000000 iq=1 duration=5 seconds ...                                                                   
sleep 5 seconds before next frame                                                                                         
SX1280 OnRxTimeout                                                                                                        
New channel set to 2425000000 Hz                                                                                          
sending : channel=2425000000 iq=0 fcnt=1 frame=8010ac00fc0001000ac74e7560484650454b4b59ae8b7873                           
SX1280 OnTxDone                                                                                                           
listen : channel=2425000000 iq=1 duration=5 seconds ...                                                                   
sleep 5 seconds before next frame                                                                                         
SX1280 OnRxTimeout                                                                                                        
New channel set to 2479000000 Hz                                                                                          
sending : channel=2479000000 iq=0 fcnt=2 frame=8010ac00fc0002000ad64e75604344454e4a545167ea7ccd                           
SX1280 OnTxDone                                                                                                           
```

## LoRaWAN Network Servers with ISM2400 region

[Chirpstack](https://www.chirpstack.io/network-server/features/regions/) and [The Things Network's lorawan-stack](https://github.com/TheThingsNetwork/lorawan-stack) are supporting the ISM2400 region. Chripstack has been tested with the SX1280 driver on Nucleo+Lambda80 module and a Multitech station with the MCard 2G4.

The LoRa ism2400 endpoint should registered as an ABP endpoint.

## Wiring

### Nucleo + Lambda80 Wiring

For the [Lambda80 pinout](./media/lambda80_pins.png) board, you need at least 8 wires, or 9 with the RESET pin:  

| Nucleo | Lambda80   | Function                     |
|--------|------------|------------------------------|
| GND    | GND    (2) | 0V                           |
| 3V3    | Vcc    (3) | +3.3V                        |
| D6     | DIO0   (6) | Busy pin                     |
| D9     | DIO1   (7) | IRQ (TX/RX done for example) |
| D10    | NSS   (16) | SPI CS                       |
| D11    | SDI   (15) | SPI MOSI                     |
| D12    | SDO   (14) | SPI MISO                     |
| D13    | SCLK  (13) | SPI CLK                      |
| D7     | RESET (12) | Optional RESET               |

This is the default pinout but you can customize it by providing another `sx1280_params_t` during `sx1280_init`.

### Nucleo + Mikrobus Lambda80 Wiring

TODO

### TinyGS 2G4 (ESP32 Wroom 32 + Lambda80 Mikrobus)

```
make BUILD_IN_DOCKER=1 BOARD=esp32-wroom-32 SX1280_MODULE=lambda80_mikrobus0 RANGETEST=1 DEVADDR=0xfc00ac40 DELAY_BETWEEN_TX=10000000 
make BOARD=esp32-wroom-32 PORT=$PORT flash-only
make BOARD=esp32-wroom-32 PORT=$PORT term
```

> [See TinyGS 2G4 station](https://github.com/thingsat/tinygs_2g4station/blob/main/README.md)

### TinyGS 2G4 (ESP32 Wroom 32 + EByte E28 Mikrobus)

```
make BUILD_IN_DOCKER=1 BOARD=esp32-wroom-32 SX1280_MODULE=e28_mikrobus0 RANGETEST=1 DEVADDR=0xfc00ac40 DELAY_BETWEEN_TX=10000000 
make BOARD=esp32-wroom-32 PORT=$PORT flash-only
make BOARD=esp32-wroom-32 PORT=$PORT term
```

> [See TinyGS 2G4 station](https://github.com/thingsat/tinygs_2g4station/blob/main/README.md)


## Tested boards

This is the list of tested boards:

* [x] nucleo-f446re
* [x] nucleo-f411re
* [x] nucleo-f401re
* [x] nucleo-f103rb
* [x] nucleo-l053r8 
* [x] nucleo-l073rz 
* [ ] nucleo-f446ze
* [ ] lora-e5-dev (Dev board and Mini)
* [ ] bluepill
* [ ] bluepill-128kib
* [x] thingsat-l0 ([external board](https://gricad-gitlab.univ-grenoble-alpes.fr/thingsat/public/-/blob/master/cubesat_mission/README.md))
* [x] olimexino-stm32 + Mikrobus Lambda80 (Nicolas Albarel)
* [x] esp32-wroom-32 + Mikrobus Lambda80 ([TinyGS 2G4 gateway](https://github.com/thingsat/tinygs_2g4station/blob/main/README.md)
* [ ] esp32-wroom-32 + Mikrobus EByte E28 ([TinyGS 2G4 gateway](https://github.com/thingsat/tinygs_2g4station/blob/main/README.md)
* [ ] esp32-wroom-32 + Mikrobus NiceRF ([TinyGS 2G4 gateway](https://github.com/thingsat/tinygs_2g4station/blob/main/README.md)

This is the list of tested gateways :
[x] Multitech MTCDT 2G4 mCard

## SX1280 firmware version

Remark: the firmware value is not equal to `5853`, check the quality of the jumpers wiring the MCU board and the SX1280 module.

```
SX1280 firmware version = 1d1d                                      
sx1280_init_status ERROR 2                                          
```

## Remarks
* Add stm32l051% to CPU_MODELS_WITHOUT_HWRNG into cpu/stm32/Makefile.features


## Frequencies plans

### LoRaWAN ISM2400 frequencies plan
* https://github.com/TheThingsNetwork/lorawan-frequency-plans/blob/master/ISM_2400_3CH_DRAFT2.yml

### Multitech 2G4 frequencies plan
* http://www.multitech.net/developer/wp-content/uploads/2020/06/global_conf.json_.2g4.txt

## Missing SX1280 features

This is the list of the task required to finish the driver:

* Set the sx1280 to sleep when not in use
* Use the right settings for power consumption, like DCDC or LDO and RC or XOSC
* Figure out the Time-On-Air according to the spec
* Finish other utility functions for LoRa
* Configure the driver for more MODEM type like GFSK, BLE, FLRC or Ranging.

## Missing SX1280 shell commands

* `sendhex`
* `forwarder` : for `lora-e5-dev` boards, forward the LoRa ism2400 messages to a SubGhZ LoRaWAN network (eu868, us915, cn470 ...). Reeceived frames are embedded into the subGhz frames payload.
* process the fcnt of the downlink frame into `rangetest` command. The fcnt of the last downlink frame can be sent into the payload of the uplink frame.

## TODO

## License
TBD

The files `.h` and `.c` are largely imported from [SX1280Lib](https://os.mbed.com/teams/Semtech/code/SX1280Lib/),  
licensed under [Apache license](https://os.mbed.com/handbook/Apache-Licence)


