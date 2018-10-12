Integration of The Things Network and openSenseMap
=======

Description
-----------

This application show how to send data to the openSenseMap platform by
integrating it with The Things Network.

This application is using the Over-The-Air Activation procedure.

Preparation
-----------
### The Things Network
Go to [the website](https://www.thethingsnetwork.org/) and create an account.
On the [console section](https://console.thethingsnetwork.org/) you will be
able to create a new application.Once the application has been created register
a new device.

Finally on the _Integrations_ section of your application add a new
'HTTP Integration'. **Make sure that URL points to
https://ttn.opensensemap.org/v1.1 and that the Method is POST**.

### openSenseMap
First, you need to register a new box. For that go to the
[openSenseMap website](https://opensensemap.org/), register and create a new
senseBox.

You will need to choose the station name, the exposure, location and type of
hardware (you can choose either of them). Under _Manual configuration_ you can
add sensors to your box. For this application we added 3 sensors:

1. **Temperature**
    - Phenomenon: 'Temperature'
    - Unit: 'C'
    - Type: 'temperature'
2. **Battery**
    - Phenomenon: 'Battery'
    - Unit: '%'
    - Type: 'uint8'
3. **Humidity**
    - Phenomenon: 'Humidity'
    - Unit: '%'
    - Type: 'humidity'

Phenomenon can be any representative string, but type needs to be one of the
accepted types
[documented here](https://github.com/sensebox/ttn-osem-integration).

Once the box is created go to the dashboard and click the 'Edit' button. On the
'TheThingsNetwork' section choose:

- **Decoding Profile**: LoRa serialization
- **TTN application ID**: (found on the 'Application Overview' section)
- **TTN device ID**: (found on the 'Device Overview' section as Device ID)
- **Port**: (LoRa port where the data is being sent to The Things Network. Found
in Application -> [your application] -> Devices -> [your device] -> Data -> 
Port column)
- **Decoder Options**:
```json
[
    {
        "sensor_title":"Temperature",
        "decoder":"temperature",
        "sensor_unit":"°C"
    },
    {
        "sensor_title":"Battery",
        "decoder":"uint8",
        "sensor_unit":"%"
    },
    {
        "sensor_title":"Humidity",
        "decoder":"humidity",
        "sensor_unit":"%"
    }
]
```

**Make sure you save your changes before leaving**.

Usage
-----

### LoRaWAN keys
To join the LoRaWAN network using OTAA, edit the application `Makefile` with the
information found on the 'Device Overview' section at The Things Network
website.

    DEVEUI ?= FFFFFFFFFFFFFFFF
    APPEUI ?= FFFFFFFFFFFFFFFF
    APPKEY ?= FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF

### LoRa serialization
To encode the measurements on the payload 'LoRa serialization' needs to be used.

**It is very important that the writings to the serialization buffer are of the
same type and in the same order as the decoder has been defined on the
SenseBox configuration!**

```C
    /* Reset serialization descriptor */
    lora_serialization_reset(&serialization);

    /* Write data to serialization. Replace with your sensors measurements. */
    lora_serialization_write_temperature(&serialization, 25.3);
    lora_serialization_write_uint8(&serialization, 82);
    lora_serialization_write_humidity(&serialization, 50.2);
```

### Build and Flash
Simply build and flash the application:

    make flash term

Use the `BOARD`, `DRIVER` and `REGION` make variables to adapt the application
to your hardware setup and region of use:

- `BOARD` can be one of the nucleo-64 boards
- `DRIVER` can be either `sx1276` or `sx1272`
- `REGION` can be `EU868`, `US915`, etc (see LoRaWAN regional parameters for
  details).

ST Nucleo-64 can be used with mbed LoRa shields: there's one based on
[the sx1276 radio](https://os.mbed.com/components/SX1276MB1xAS/) and one based
on the [the sx1272 radio](https://os.mbed.com/components/SX1272MB2xAS/).

**You should see the reported data on the
[openSenseMap](https://opensensemap.org/).**