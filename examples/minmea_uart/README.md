# MINMEA on UART : reading and parse NMEA sentences for a GNSS module

simple application for reading and parse NMEA sentences for a GNSS module connected to one UART

> The source code of this application is mainly based on `tests/periph_uart` test.


```bash
make BOARD=lora-e5-dev
cp bin/lora-e5-dev/tests_minmea_uart.bin  /Volumes/NUCLEO/
```

The `lora-e5-dev` board had 3 UARTs. For testing this application without a GNSS module, you can connect `RX1` pin to `TX2` pin and `RX2` pin to `TX1` pin.

```
UARD_DEV(0): test uart_poweron() and uart_poweroff()  ->  [OK]

UART INFO:
Available devices:               3
UART used for STDIO (the shell): UART_DEV(0)

> help
Command              Description                                                
---------------------------------------                                         
init                 Initialize a UART device with a given baudrate             
mode                 Setup data bits, stop bits and parity for a given UART deve
send                 Send a string through given UART device                    
test                 Run an automated test on a UART with RX and TX connected   
> init 1 9600                                                                   
Success: Initialized UART_DEV(1) at BAUD 9600                                   
UARD_DEV(1): test uart_poweron() and uart_poweroff()  ->  [OK]                  
> init 2 9600                                                                   
Success: Initialized UART_DEV(2) at BAUD 9600                                   
UARD_DEV(2): test uart_poweron() and uart_poweroff()  ->  [OK]                  
> send 2 TEST                                                                   
UART_DEV(2) TX: TEST                                                            
Success: UART_DEV(1) RX: [TEST]\n                                               
> send 1 $GNGLL,5229.0178,N,01326.7605,E,114350.000,A,A*45
UART_DEV(1) TX: $GNGLL,5229.0178,N,01326.7605,E,114350.000,A,A*45               
parsed coordinates: lat=52.483631 lon=13.446008                                 
SUCCESS                                                                         
Success: UART_DEV(2) RX: [$GNGLL,5229.0178,N,01326.7605,E,114350.000,A,A*45]\n  
> send 1 $GPZDA,024611.08,25,03,2002,00,00*6A
UART_DEV(1) TX: $GPZDA,024611.08,25,03,2002,00,00*6A
parsed date: 2002-3-25 2:46:11.80000 offset=0:0
SUCCESS
Success: UART_DEV(2) RX: [$GPZDA,024611.08,25,03,2002,00,00*6A]\n

> send 1 $GPGGA,161229.487,3723.2475,N,12158.3416,W,1,07,1.0,9.0,M,,,,0000*18
UART_DEV(1) TX: $GPGGA,161229.487,3723.2475,N,12158.3416,W,1,07,1.0,9.0,M,,,,0000*18
TODO: parse NMEA sentence 2.
Success: UART_DEV(2) RX: [$GPGGA,161229.487,3723.2475,N,12158.3416,W,1,07,1.0,9.0,M,,,,0000*18]\n

> send 1 $GPRMC,092204.999,A,4250.5589,S,14718.5084,E,0.00,89.68,211200,,*25
UART_DEV(1) TX: $GPRMC,092204.999,A,4250.5589,S,14718.5084,E,0.00,89.68,211200,,*25
parsed Position and time: valid=1 date=0-12-21 9:22:4.999000
 lat=-42.842647 lng=147.308471 speed=0 course=1.494666 variation=0
SUCCESS
Success: UART_DEV(2) RX: [$GPRMC,092204.999,A,4250.5589,S,14718.5084,E,0.00,89.68,211200,,*25]\n

> send 1 $GPGSV,1,1,01,21,00,000,*4B
UART_DEV(1) TX: $GPGSV,1,1,01,21,00,000,*4B
TODO: parse NMEA sentence 6.
Success: UART_DEV(2) RX: [$GPGSV,1,1,01,21,00,000,*4B]\n

> send 1 $GPGSV,3,1,10,20,78,331,45,01,59,235,47,22,41,069,,13,32,252,45*70
UART_DEV(1) TX: $GPGSV,3,1,10,20,78,331,45,01,59,235,47,22,41,069,,13,32,252,45*70
TODO: parse NMEA sentence 6.
Success: UART_DEV(2) RX: [$GPGSV,3,1,10,20,78,331,45,01,59,235,47,22,41,069,,13,32,252,45*70]\n
```

More NMEA sentences are available [here](https://www.satsleuth.com/GPS_NMEA_sentences.aspx).


## Test

| BOARD         | GNSS Module           | UART (RX,TX)    | Params   | Comment                                   |
|---------------|-----------------------|-----------------|----------|-------------------------------------------|
| lora-e5-dev   | Grove GPS model SIM28 | 1 (RX1, TX1)    | 9600 8N1 | STDIO_UART_BAUDRATE should be set to 9600 |
| p-nucleo-wb55 |                       | 1 (PA3, PA2)    | 9600 8N1 |                                           |
| nucleo-f446re |                       | 1 (???, ???)    | 9600 8N1 |                                           |



## Misc

* [GNSS modules](https://github.com/CampusIoT/orbimote/blob/master/gnss_modules.md)
