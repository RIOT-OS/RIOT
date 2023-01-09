/**
 * @defgroup pkg_uwb_core Device driver model for the Decawave Impulse Radio-Ultra Wideband (IR-UWB) transceiver(s)
 * @ingroup  pkg
 * @brief    Hardware and architecture agnostic platform for IoT
 *           Location Based Services (LBS)
 * @see      https://github.com/Decawave/uwb-core
 */

# Decawave uwb-core RIOT Port

The distribution https://github.com/decawave/uwb-core contains the
device driver model for the Decawave Impulse Radio-Ultra Wideband
(IR-UWB) transceiver(s). The driver includes hardware abstraction
layers (HAL), media access control (MAC) layer, Ranging Services (RNG).
The uwb-core driver and RIOT combine to create a hardware and
architecture agnostic platform for IoT Location Based Services (LBS).

## Abstraction details

uwb-core is meant as a hardware and architecture agnostic library. It
was developed with MyNewt as its default OS, but its abstractions are
well defined which makes it easy to use with another OS.

A porting layer DPL (Decawave Porting Layer) has been implemented that
wraps around OS functionalities and modules: mutex, semaphores, threads,
etc.. In most cases the mapping is direct although some specific
functionalities might not be supported.

Since the library was used on top of mynewt most configuration values
are prefixed with `MYNEWT_VAL_%`, all configurations can be found under
`pkg/uwb-core/include/syscfg`.

In MyNewt there is always a default event thread, since we don't have
this in RIOT when using this pkg an event thread is started which will
run as this default event thread.

To work this library needs to be built on top of an UWB device
implementing the `uwb` api (see [uwb](https://github.com/Decawave/uwb-core/tree/master/hw/drivers/uwb).
This port uses [uwb-dw1000](https://github.com/Decawave/uwb-dw1000) as
device driver for dw1000 modules.

The library can be used by directly including the different module headers.

## Current support

uwb-core comes with many utility libraries, only ranging related libraries
are described here. For more info refer to [uwb-core](https://github.com/Decawave/uwb-core).

- uwb-core_uwb: The uwb-core driver implements the MAC layers and
  exports a MAC extension interface for additional services.
  In the case of this port the api implementation is provided mainly by
  the uwb-dw1000 pkg.

- uwb-core_rng: ranging base class, provides the api to perform ranging.
  It can use any of the twr_% ranging algorithms mentioned below,
  for details on each algorithm refer to the pertinent literature.

- uwb-core_twr_ss: single side two way ranging

- uwb-core_twr_ss_ack: : single side two way ranging using hardware
  generated ACK as the response.

- uwb-core_twr_ss_ext: single side two way ranging with extended frames.

- uwb-core_twr_ds: double side two-way ranging

- uwb-core_twr_ds_ext: double side wo way ranging with extended frames.

## Usage example

The most simple examples would be a tdoa blinking tag, here the device
is considered awake.

```c
    // IEEE 802.15.4e standard blink
    ieee_blink_frame_t tdoa_blink_frame = {
        .fctrl = 0xC5,  /* frame type (0xC5 for a blink) using 64-bit addressing */
        .seq_num = 0,   /* sequence number, incremented for each new frame. */
        .long_address = 0,  /* device ID */
    };
    // Set device address
    tdoa_blink_frame.long_address = udev->my_long_address;
    // Write tx data and start transmission
    uwb_write_tx(udev, tdoa_blink_frame.array, 0, sizeof(ieee_blink_frame_t));
    uwb_start_tx(udev);
    // Increase sequence number
    tdoa_blink_frame.seq_num++;
```

The following can be wrapped into a timer callback to setup a blinking tag.

For more examples check the [uwb-apps](https://github.com/Decawave/uwb-apps)
repository as well as [examples/twr_aloha](https://github.com/RIOT-OS/RIOT/tree/master/examples/twr-aloha)

## Watchout!

uwb-core uses anonymous structures, therefore any application using this
pkg must also set at application level:

  CFLAGS += -fms-extensions

If building with llvm then also `CFLAGS += -Wno-microsoft-anon-tag` must
be added, although it is currently blacklisted because of missing stdatomic
definitions.

## Todos

The uwb-core pkg comes with a wide range of feature that are currently
untested and not supported in this port, these will be incrementally
included:

- [ ] `uwb_cpp`: Clock Calibration Packet (CCP) Service
- [ ] `tdma`: uwb-core offers TDMA support
- [ ] `uwb_wcs`: Wireless Clock Synchronization (WCS) Service
- [ ] `rtdoa`: Reverse Time Difference of Arrival profiles
- [ ] `cir`: Circular impulse response
- [ ] `nrng`: N-ranges support
- [ ] other modules in `lib`

uwb-core uses dynamic allocation for some of its internal modules. It
would be nice to make this optional when possible.
