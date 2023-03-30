## About

This is a manual test application for the SX1280 radio driver.

This manual test application was also tested with a custom board (esp32-wroom-32 + lambda80C-24S on Mikrobus_0) :
see https://github.com/thingsat/tinygs_2g4station

If you have other hardware (boards, Semtech based LoRa module), you can adapt
the configuration to your needs by copying an adapted version of
`drivers/sx1280/include/sx1280_params.h` file to your application directory.

## Build and Flash

```shell
cd tests/driver_sx1280
make flash term
```

## Usage

This test application provides low level shell commands to interact with the
SX1280 module.

Once the board is flashed and you are connected via serial to the shell, use the `help`
command to display the available commands:

```
> help
2022-05-09 10:27:13,724 # help
2022-05-09 10:27:13,731 # Command              Description
2022-05-09 10:27:13,731 # ---------------------------------------
2022-05-09 10:27:13,735 # sx1280               Control the SX1280 radio
2022-05-09 10:27:13,738 # reboot               Reboot the node
2022-05-09 10:27:13,742 # version              Prints current RIOT_VERSION
2022-05-09 10:27:13,747 # pm                   interact with layered PM subsystem
```

### Getter

```
2022-04-28 15:00:32,795 # main(): This is RIOT! (Version: 2021.10-devel-3491-g23b57-pr/sx1280_v2)
2022-04-28 15:00:32,806 # Initialization successful - starting the shell now
> sx1280
2022-04-28 15:00:39,408 # sx1280
2022-04-28 15:00:39,409 # Usage: sx1280 <get|set|rx|tx|reset>
> sx1280 get
2022-04-28 15:00:44,808 # sx1280 get
2022-04-28 15:00:44,809 # Usage: sx1280 get <type|freq|bw|sf|cr>
> sx1280 get type
2022-04-28 15:00:48,064 # sx1280 get type
2022-04-28 15:00:48,065 # Device type: lora
> sx1280 get freq
2022-04-28 15:00:52,639 # sx1280 get freq
2022-04-28 15:00:52,639 # Frequency: 2403000000 Hz
> sx1280 get bw
2022-04-28 15:00:57,130 # sx1280 get bw
2022-04-28 15:00:57,131 # Bandwidth: 800 kHz
> sx1280 get sf
2022-04-28 15:01:01,476 # sx1280 get sf
2022-04-28 15:01:01,477 # Spreading factor: 12
2022-04-28 15:01:10,159 # sx1280 get cr
2022-04-28 15:01:10,165 # Coding rate: 6
2022-04-28 15:01:10,165 # Usage: use
2022-04-28 15:01:10,166 #      LORA_CR_4_5 = 1
2022-04-28 15:01:10,166 #      LORA_CR_4_6 = 2
2022-04-28 15:01:10,167 #      LORA_CR_4_7 = 3
2022-04-28 15:01:10,167 #      LORA_CR_4_8 = 4
2022-04-28 15:01:10,169 #      LORA_CR_LI_4_5 = 5
2022-04-28 15:01:10,170 #      LORA_CR_LI_4_6 = 6
2022-04-28 15:01:10,171 #      LORA_CR_LI_4_8 = 7
```

### Setter

```
2022-04-28 16:30:53,635 # sx1280
2022-04-28 16:30:53,642 # Usage: sx1280 <get|set|rx|tx>
> sx1280 set
2022-04-28 16:30:56,504 # sx1280 set
2022-04-28 16:30:56,505 # Usage: sx1280 set <freq|bw|sf|cr|> <value>
> sx1280 freq
2022-04-28 16:31:02,802 # sx1280 freq
> sx1280 set freq
2022-04-28 16:31:12,282 # sx1280 set freq
2022-04-28 16:31:12,287 # Usage: use freq between 2400000000 + (bw/2) and 2500000000 - (bw/2) (Hz) !
2022-04-28 16:31:12,290 # Usage: sx1280 set <freq|bw|sf|cr|> <value>
> sx1280 set bw
2022-04-28 16:31:18,924 # sx1280 set bw
2022-04-28 16:31:18,925 # Usage: use 200, 400, 800, 1600 (kHz)
2022-04-28 16:31:18,929 # Usage: sx1280 set <freq|bw|sf|cr|> <value>
> sx1280 set sf
2022-04-28 16:31:23,232 # sx1280 set sf
2022-04-28 16:31:23,233 # Usage: use SF between 5 and 12
2022-04-28 16:31:23,236 # Usage: sx1280 set <freq|bw|sf|cr|> <value>
> sx1280 set cr
2022-04-28 15:01:10,159 # sx1280 set cr
2022-04-28 15:01:10,165 # Usage: use
2022-04-28 15:01:10,166 #      LORA_CR_4_5 = 1
2022-04-28 15:01:10,166 #      LORA_CR_4_6 = 2
2022-04-28 15:01:10,167 #      LORA_CR_4_7 = 3
2022-04-28 15:01:10,167 #      LORA_CR_4_8 = 4
2022-04-28 15:01:10,169 #      LORA_CR_LI_4_5 = 5
2022-04-28 15:01:10,170 #      LORA_CR_LI_4_6 = 6
2022-04-28 15:01:10,171 #      LORA_CR_LI_4_8 = 7
> sx1280 set cr 6
2022-04-28 15:01:12,359 # sx1280 set cr 6
2022-04-28 15:01:12,362 # cr set

2022-04-28 16:31:27,001 # Usage: sx1280 set <freq|bw|sf|cr|> <value>
> sx1280 set freq 2455555555
2022-04-28 16:32:20,235 # sx1280 set freq 2455555555
2022-04-28 16:32:20,236 # freq set
> sx1280 set bw 200
2022-04-28 16:32:29,468 # sx1280 set bw 200
2022-04-28 16:32:29,468 # setup: setting 200KHz bandwidth
2022-04-28 16:32:29,469 # bw set
> sx1280 set sf 5
2022-04-28 16:32:35,138 # sx1280 set sf 5
2022-04-28 16:32:35,141 # sf set
> sx1280 set cr 4
2022-04-28 16:32:40,970 # sx1280 set cr 4
2022-04-28 16:32:40,973 # cr set

sx1280 reset
2022-05-17 13:36:54,836 # sx1280 reset
2022-05-17 13:36:54,836 # resetting sx1280...
```

### TX/RX

 Once we have a shell on both card we can use the shell cmd :

```
2022-04-28 16:26:48,034 # main(): This is RIOT! (Version: 2021.10-devel-3491-g23b57-pr/sx1280_v2)
2022-04-28 16:26:48,046 # Initialization successful - starting the shell now
help
2022-04-28 16:27:23,060 # help
2022-04-28 16:27:23,061 # Command              Description
2022-04-28 16:27:23,066 # ---------------------------------------
2022-04-28 16:27:23,067 # sx1280               Control the SX1280 radio
2022-04-28 16:27:23,072 # reboot               Reboot the node
2022-04-28 16:27:23,077 # version              Prints current RIOT_VERSION
2022-04-28 16:27:23,082 # pm                   interact with layered PM subsystem
2022-04-28 16:27:23,085 # ps                   Prints information about running threads.
> sx1280
2022-04-28 16:27:27,574 # sx1280
2022-04-28 16:27:27,575 # Usage: sx1280 <get|set|rx|tx>
> sx1280 tx
2022-04-28 16:27:30,710 # sx1280 tx
2022-04-28 16:27:30,711 # Usage: sx1280 tx <payload>
> sx1280 tx test_default_params
2022-04-28 16:28:00,452 # sx1280 tx test_default_params
2022-04-28 16:28:00,455 # sending "test_default_params" payload (20 bytes)
> 2022-04-28 16:28:00,718 # Transmission completed
```

```
2022-04-28 16:26:43,538 # Connect to serial port /dev/tty.usbserial-14620
Welcome to pyterm!
Type '/exit' to exit.
sx1280
2022-04-28 16:27:49,686 # sx1280
2022-04-28 16:27:49,687 # Usage: sx1280 <get|set|rx|tx>
> sx1280 rx
2022-04-28 16:27:53,657 # sx1280 rx
2022-04-28 16:27:53,657 # Usage: sx1280 rx <start|stop>
> sx1280 rx start
2022-04-28 16:27:56,596 # sx1280 rx start
2022-04-28 16:27:56,600 # Listen mode started
> 2022-04-28 16:28:00,562 # Data reception started
2022-04-28 16:28:00,726 # Received: "test_default_params" (20 bytes) - [RSSI: -43, SNR: 8]
```
