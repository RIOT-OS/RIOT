/**
 * @defgroup pkg_tinyusb TinyUSB package
 * @ingroup  pkg
 * @brief    Provides the tinyUSB stack as package
 * @author   Gunar Schorcht <gunar@schorcht.net>
 * @see https://github.com/hathach/tinyusb
 *
 * # TinyUSB
 *
 * tinyUSB is an open-source cross-platform USB Host/Device stack for
 * embedded systems.
 *
 * @note This package is distinct from (and incompatible with) the USB stack
 * provided around USBUS in RIOT (see @ref usb). They differ in the level of
 * abstraction and in the set of supported devices.
 *
 * # Usage
 *
 * Add the following entries to your application makefile:
 * - Enable tinyUSB package
 *   ```makefile
 *   USEPKG += tinyusb
 *   ```
 * - Select whether to use the tinyUSB device stack or the tinyUSB host stack or both:
 *   ```makefile
 *   USEMODULE += tinyusb_device
 *   ```
 * - Select the device/host classes to be used, for example:
 *   ```makefile
 *   USEMODULE += tinyusb_class_cdc tinyusb_class_msc
 *   ```
 *
 * Either add `tinyusb_setup()` to your main function to explicitly initialize
 * the tinyUSB stack including the used peripherals and start the tinyUSB
 * thread, or use the `auto_init` module (**default**).
 *
 * ```c
 * int main(void)
 * {
 *     ...
 *     // If auto-initialization is not used (module `auto_init`),
 *     // initialize the tinyUSB stack including used peripherals and
 *     // start the tinyUSB thread. Auto-initialization is used by default.
 *     tinyusb_setup();
 *
 *     while (1) {
 *         ...
 *     }
 *
 *     return 0;
 * }
 * ```
 *
 * If it is necessary to override the default configuration defined in
 * `tusb_config.h`, create a file `tinyusb_app_config.h` in your application
 * directory and override the configuration to be changed.
 * ```c
 * #define CONFIG_TUSBD_CDC_NUMOF   2
 * ```
 *
 * By default, the number of `CONFIG_TUSBD_*_NUMOF` device class and
 * `CONFIG_TUSBH_*_NUMOF` host class interfaces are defined to 1 if the
 * corresponding `tinyusb_class_*` and `tinyusb_device`/`tinyusb_host`
 * module are enabled, and 0 otherwise.
 * That is, there is one interface of each class.
 *
 * For example, if the `tinyusb_device` and `tinyusb_class_cdc` modules are
 * enabled, `CONFIG_TUSBD_CDC_NUMOF` is defined to 1 by default. The number of
 * all other `CONFIG_TUSBD_*_NUMOF` device class interfaces are 0.
 *
 * If you add the `tinyusb_app_config.h` file in your application directory
 * to override the default configuration, add the application path to the
 * include paths at the end of your
 * application's Makefile. This is necessary so that the tinyUSB stack
 * uses the file `tinyusb_app_config.h` from your application directory
 * and thus the file `tusb_config.h` from the tinyUSB package.
 * ```makefile
 * USEPKG += tinyusb
 * USEMODULE += tinyusb_class_cdc
 * USEMODULE += tinyusb_class_msc
 * USEMODULE += tinyusb_device
 *
 * include $(RIOTBASE)/Makefile.include
 *
 * INCLUDES += -I$(APPDIR)
 * ```
 *
 * Implement the callbacks of the enabled classes.
 *
 * For most common device classes and their configuration, the tinyUSB
 * package automatically generates the required descriptors and descriptor
 * callbacks for any combination of
 * - up to two interfaces of the class CDC ACM,
 * - up to two interfaces of the Generic In/Out HID class,
 * - up to one MSC device interface and,
 * - up to one interface of the Vendor device class.
 *
 * Any other combination, either a different number of these device class
 * interfaces or the use of a different device class interface, requires the
 * implementation of custom descriptors and the callbacks.
 *
 * All symbols of the generated descriptors and their callback functions are
 * defined as weak symbols so that the application can override parts of the
 * descriptors or the callback functions that handle them. For example, the
 * array `tusb_desc_hid_0_report`, which defines the HID report descriptor for
 * HID device 0, making the device a generic in/out HID device
 *
 * ```c
 * __attribute__((weak))
 * uint8_t const tusb_desc_hid_0_report[] =
 * {
 *     TUD_HID_REPORT_DESC_GENERIC_INOUT(CONFIG_TUSBD_HID_EP_SIZE),
 * };
 * ```
 *
 * could be overridden by the application with
 *
 * ```c
 * uint8_t const tusb_desc_hid_0_report[] =
 * {
 *     TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(REPORT_ID_KEYBOARD)),
 *     TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(REPORT_ID_MOUSE)),
 * };
 * ```
 *
 * to make the device to be a composite keyboard/mouse device.
 *
 * Please refer `$RIOTBASE/tests/pkg_tinyusb_cdc_msc` and the
 * [tinyUSB documentation](https://docs.tinyusb.org/en/latest/reference/getting_started.html)
 * for details.
 */
