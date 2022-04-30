### About

This example demonstrates how to communicate with the Kafka REST proxy

### Compilation

To compile and flash the application, use e.g. the following command

```bash
$ BOARD=nucleo-f207zg IPV4=0 IPV6=1 TRANSPORT_COAP=1 QOS=0 make flash term
```

Available configuration options are:

    TRANSPORT_COAP=1 # for CoAP transport
    TRANSPORT_COAP_SECURE=1 # for CoAP Secure transport
    TRANSPORT_MQTT=1 # for MQTT transport
    TRANSPORT_MQTT_SN=1 # for MQTT-SN transport

    QOS=0 # quality of service 0, 1 or 2

    IPV4=1 # enable IPv4 support
    IPV6=1 # enable IPv6 support

### Note

The application may not succeed on the first attempt due to unknown reasons. Try resetting the
board a few times.

Do not forget to set `CONFIG_CALL_KAFKA = True` in `mqtt2http` in the docker-compose project. 
