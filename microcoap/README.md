Microcoap example
============

This is a small microcoap example application. It provides a server which only 
answers GET requests to the resource /foo/bar. 

## Setup

You can use [marz](https://github.com/sgso/marz) to tunnel CoAP messages into the RIOT native thread. This is a bit tricky, so maybe this walkthrough will help:

0. Build this application.
1. Install the copper plugin in your Firefox browser.
2. Run `sudo apt-get install bridge-utils`
3. In your RIOT directury, run

    ./cpu/native/tapsetup.sh create 2

This will set up two tap devices connected by a bridge. our RIOT application and 
marz will each listen at one of these devices, and communicate over the bridge.

3. Open two terminal windows.  

**In window #1**, start the microcoap-example application:

    cd applications/microcoap
    sudo ./bin/native/microcoap-example.elf tap1 -i 1

*Make sure to bind it to ``tap1``, since marz will be bound to ``tap0`!*  
``-i 1`` forces your RIOT instance to match its id to the one specified in marz.config. You should **only** specify this for the **one** RIOT that marz tunnels to. This is sufficient for this example; if you need help running more than one RIOT with marz, please contact the author of this example.

You should see output similar to this.

    RIOT native uart0 initialized.
    RIOT native interrupts/signals initialized.
    LED_GREEN_OFF
    LED_RED_ON
    RIOT native board initialized.
    RIOT native hardware initialization complete.

    kernel_init(): This is RIOT! (Version: 400e-microcoap)
    kernel_init(): jumping into first task...
    UART0 thread started.
    uart0_init() [OK]
    Initializing transport layer protocol: udp
    Starting example microcoap server...
    initializing 6LoWPAN...
    initializing receive socket...
    Ready to receive requests.

                Welcome to RIOT

    >

**In window #2**, first install Python development headers by running

    sudo apt-get install python-dev

Afterwards you can install and run marz:
    
    pip install --user Twisted && 
    pip install --user bidict && 
    git clone https://github.com/sgso/marz &&
    cd marz &&
    ./setup.sh 

    ./marz.py

You should see output similar to this.

    WARNING: No route found for IPv6 destination :: (no default route?)
    Listening on UDP ports: [5683, 2222]
    Listening on tap interface tap0 with MAC address 9a:80:a3:fc:93:18

## Testing

The "Copper" firefox plugin is a convenient way to test CoAP endpoints. In the absence of a GUI you can also use Python to send test requests.

### Using python(3)

First, make sure Python 3 is installed, clone `aiocoap` into a directory of your choice and then change into it:

    git clone git@github.com:chrysn/aiocoap.git &&
        cd aiocoap

Open the `clientGET.py` file and change the line that reads

    request.set_request_uri(<some url>)

to

    request.set_request_uri('coap://[::1]/foo/bar')

Then run `clientGET.py`, which should print the following:

    $ ./clientGET.py
    Result: 2.05 Content
    b'1337'


### Using the Firefox Copper plugin

Open Firefox and enter

    coap://[::1]:5683/foo/bar

Into the browser window. Then, click the big gren ``GET`` button. This should
trigger a GET request to our microcoap-example application. Shortly after you've
clicked GET, **window #2** should read

    make new connection
    [UDP6 5683] Received 14 data bytes from ('::1', 54685): Relaying through 54685 to RiotEndpoint(hwaddr=1, ipv6='fe80::ff:fe00:1', port=5683)
    [TAP] Received 12 data bytes on port 54685: Relaying through 5683 to IP6Endpoint(ipv6='::1', port=54685)

**window #1** should supply you with detailed information about the received 
request and the reply our microcoap-example is sending:

    > Received packet: 40 01 0B EC B3 66 6F 6F 03 62 61 72 C1 02
    content:
    Header:
      ver  0x01
      t    0x01
      tkl  0x00
      code 0x01
      id   0x0BEC
     Options:
      0x0B [ 66 6F 6F ]
      0x0B [ 62 61 72 ]
      0x17 [ 02 ]
    Payload:
    Sending packet: 60 45 0B EC C2 00 00 FF 31 33 33 37
    content:
    Header:
      ver  0x01
      t    0x01
      tkl  0x00
      code 0x45
      id   0x0BEC
     Options:
      0x0C [ 00 00 ]
    Payload: 31 33 33 37

And finally, the big grey ``Payload`` box in your Firefox window should read:

    1337

If this all works, you're good to go! :)
