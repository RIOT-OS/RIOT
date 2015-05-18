//*****************************************************************************
//
// usbdhid.h - Definitions used by HID class devices.
//
// Copyright (c) 2008-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the Stellaris USB Library.
//
//*****************************************************************************

#ifndef __USBDHID_H__
#define __USBDHID_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! \addtogroup hid_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! The size of the memory that should be allocated to create a configuration
//! descriptor for a single instance of the USB HID Device.
//! This does not include the configuration descriptor which is automatically
//! ignored by the composite device class.
//
// For reference this is
// sizeof(g_sHIDInterfaceSection) +  sizeof(g_sHIDDescriptorSection) +
// sizeof(g_sHIDInEndpointSection) + sizeof(g_sHIDOutEndpointSection)
//
//*****************************************************************************
#define COMPOSITE_DHID_SIZE     (32)

//*****************************************************************************
//
// Macros used to create the static Report Descriptors.
//
//*****************************************************************************

//*****************************************************************************
//
//! This is a macro to assist adding Usage Page entries in HID report
//! descriptors.
//!
//! \param ucValue is the Usage Page value.
//!
//! This macro takes a value and prepares it to be placed as a Usage Page entry
//! into a HID report structure.  These are defined by the USB HID
//! specification.
//!
//! \return Not a function.
//
//*****************************************************************************
#define UsagePage(ucValue)      0x05, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Usage entries in HID report descriptors.
//!
//! \param ucValue is the Usage value.
//!
//! This macro takes a value and prepares it to be placed as a Usage entry into
//! a HID report structure.  These are defined by the USB HID specification.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Usage(ucValue)          0x09, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Usage Minimum entries in HID report
//! descriptors.
//!
//! \param ucValue is the Usage Minimum value.
//!
//! This macro takes a value and prepares it to be placed as a Usage Minimum
//! entry into a HID report structure.  This is the first or minimum value
//! associated with a usage value.
//!
//! \return Not a function.
//
//*****************************************************************************
#define UsageMinimum(ucValue)   0x19, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Usage Maximum entries in HID report
//! descriptors.
//!
//! \param ucValue is the Usage Maximum value.
//!
//! This macro takes a value and prepares it to be placed as a Usage Maximum
//! entry into a HID report structure.  This is the last or maximum value
//! associated with a usage value.
//!
//! \return Not a function.
//
//*****************************************************************************
#define UsageMaximum(ucValue)   0x29, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Logical Minimum entries in HID report
//! descriptors.
//!
//! \param cValue is the Logical Minimum value.
//!
//! This macro takes a value and prepares it to be placed as a Logical Minimum
//! entry into a HID report structure.  This is the actual minimum value for a
//! range of values associated with a field.
//!
//! \return Not a function.
//
//*****************************************************************************
#define LogicalMinimum(cValue)  0x15, ((cValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Logical Maximum entries in HID report
//! descriptors.
//!
//! \param cValue is the Logical Maximum value.
//!
//! This macro takes a value and prepares it to be placed as a Logical Maximum
//! entry into a HID report structure.  This is the actual maximum value for a
//! range of values associated with a field.
//!
//! \return Not a function.
//
//*****************************************************************************
#define LogicalMaximum(cValue)  0x25, ((cValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Physical Minimum entries in HID report
//! descriptors.
//!
//! \param sValue is the Physical Minimum value.  It is a signed, 16 bit
//! number.
//!
//! This macro takes a value and prepares it to be placed as a Physical Minimum
//! entry into a HID report structure.  This is value is used in conversion of
//! the control logical value, as returned to the host in the relevant report,
//! to a physical measurement in the appropriate units.
//!
//! \return Not a function.
//
//*****************************************************************************
#define PhysicalMinimum(sValue) 0x36, ((sValue) & 0xFF),                      \
                                (((sValue) >> 8) & 0xFF)

//*****************************************************************************
//
//! This is a macro to assist adding Physical Maximum entries in HID report
//! descriptors.
//!
//! \param sValue is the Physical Maximum value.  It is a signed, 16 bit
//! number.
//!
//! This macro takes a value and prepares it to be placed as a Physical Maximum
//! entry into a HID report structure.  This is value is used in conversion of
//! the control logical value, as returned to the host in the relevant report,
//! to a physical measurement in the appropriate units.
//!
//! \return Not a function.
//
//*****************************************************************************
#define PhysicalMaximum(sValue) 0x46, ((sValue) & 0xFF),                      \
                                (((sValue) >> 8) & 0xFF)

//*****************************************************************************
//
//! This is a macro to assist adding Collection entries in HID report
//! descriptors.
//!
//! \param ucValue is the type of Collection.
//!
//! This macro takes a value and prepares it to be placed as a Collection
//! entry into a HID report structure.  This is the type of values that are
//! being grouped together, for instance input, output or features can be
//! grouped together as a collection.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Collection(ucValue)     0xa1, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding End Collection entries in HID report
//! descriptors.
//!
//! This macro can be used to place an End Collection entry into a HID report
//! structure.  This is a tag to indicate that a collection of entries has
//! ended in the HID report structure.  This terminates a previous Collection()
//! entry.
//!
//! \return Not a function.
//
//*****************************************************************************
#define EndCollection           0xc0

//*****************************************************************************
//
//! This is a macro to assist adding Report Count entries in HID report
//! descriptors.
//!
//! \param ucValue is the number of items in a report item.
//!
//! This macro takes a value and prepares it to be placed as a Report Count
//! entry into a HID report structure.  This is number of entries of Report
//! Size for a given item.
//!
//! \return Not a function.
//
//*****************************************************************************
#define ReportCount(ucValue)    0x95, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Report ID entries in HID report
//! descriptors.
//!
//! \param ucValue is the identifier prefix for the current report.
//!
//! This macro takes a value and prepares it to be placed as a Report ID
//! entry into a HID report structure.  This value is used as a 1 byte prefix
//! for the report it is contained within.
//!
//! \return Not a function.
//
//*****************************************************************************
#define ReportID(ucValue)       0x85, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Report Size entries in HID report
//! descriptors.
//!
//! \param ucValue is the size, in bits, of items in a report item.
//!
//! This macro takes a value and prepares it to be placed as a Report Size
//! entry into a HID report structure.  This is size in bits of the entries of
//! of a report entry.  The Report Count specifies how many entries of Report
//! Size are in a given item.  These can be individual bits or bit fields.
//!
//! \return Not a function.
//
//*****************************************************************************
#define ReportSize(ucValue)     0x75, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Input entries in HID report descriptors.
//!
//! \param ucValue is bit mask to specify the type of a set of input report
//! items.  Note that if the USB_HID_INPUT_BITF flag is required, the Input2
//! macro (which uses a 2 byte version of the Input item tag) must be used
//! instead of this macro.
//!
//! This macro takes a value and prepares it to be placed as an Input entry
//! into a HID report structure.  This specifies the type of an input item in
//! a report structure.  These refer to a bit mask of flags that indicate the
//! type of input for a set of items.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Input(ucValue)          0x81, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Input entries in HID report descriptors.
//!
//! \param usValue is bit mask to specify the type of a set of input report
//! items.  Note that this macro uses a version of the Input item tag with a
//! two byte payload and allows any of the 8 possible data bits for the tag to
//! be used.  If USB_HID_INPUT_BITF (bit 8) is not required, the Input macro
//! may be used instead.
//!
//! This macro takes a value and prepares it to be placed as an Input entry
//! into a HID report structure.  This specifies the type of an input item in
//! a report structure.  These refer to a bit mask of flags that indicate the
//! type of input for a set of items.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Input2(usValue)         0x82, ((usValue) & 0xff),                     \
                                (((usValue) >> 8) & 0xFF)

//*****************************************************************************
//
//! This is a macro to assist adding Feature entries in HID report descriptors.
//!
//! \param ucValue is bit mask to specify the type of a set of feature report
//! items.  Note that if the USB_HID_FEATURE_BITF flag is required, the
//! Feature2 macro (which uses a 2 byte version of the Feature item tag) must
//! be used instead of this macro.
//!
//! This macro takes a value and prepares it to be placed as a Feature entry
//! into a HID report structure.  This specifies the type of a feature item in
//! a report structure.  These refer to a bit mask of flags that indicate the
//! type of feature for a set of items.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Feature(ucValue)        0xB1, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Feature entries in HID report descriptors.
//!
//! \param usValue is bit mask to specify the type of a set of feature report
//! items.  Note that this macro uses a version of the Feature item tag with a
//! two byte payload and allows any of the 8 possible data bits for the tag to
//! be used.  If USB_HID_FEATURE_BITF (bit 8) is not required, the Feature
//! macro may be used instead.
//!
//! This macro takes a value and prepares it to be placed as a Feature entry
//! into a HID report structure.  This specifies the type of a feature item in
//! a report structure.  These refer to a bit mask of flags that indicate the
//! type of feature for a set of items.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Feature2(usValue)       0xB2, ((usValue) & 0xff),                     \
                                (((usValue) >> 8) & 0xFF)

//*****************************************************************************
//
//! This is a macro to assist adding Output entries in HID report descriptors.
//!
//! \param ucValue is bit mask to specify the type of a set of output report
//! items.  Note that if the USB_HID_OUTPUT_BITF flag is required, the Output2
//! macro (which uses a 2 byte version of the Output item tag) must be used
//! instead of this macro.
//!
//! This macro takes a value and prepares it to be placed as an Output entry
//! into a HID report structure.  This specifies the type of an output item in
//! a report structure.  These refer to a bit mask of flags that indicate the
//! type of output for a set of items.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Output(ucValue)         0x91, ((ucValue) & 0xff)

//*****************************************************************************
//
//! This is a macro to assist adding Output entries in HID report descriptors.
//!
//! \param usValue is bit mask to specify the type of a set of output report
//! items.  Note that this macro uses a version of the Output item tag with a
//! two byte payload and allows any of the 8 possible data bits for the tag to
//! be used.  If USB_HID_OUTPUT_BITF (bit 8) is not required, the Output macro
//! may be used instead.
//!
//! This macro takes a value and prepares it to be placed as an Output entry
//! into a HID report structure.  This specifies the type of an output item in
//! a report structure.  These refer to a bit mask of flags that indicate the
//! type of output for a set of items.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Output2(usValue)        0x92, ((usValue) & 0xff),                     \
                                (((usValue) >> 8) & 0xFF)

//*****************************************************************************
//
//! This is a macro to assist adding Unit Exponent entries in HID report
//! descriptors.
//!
//! \param cValue is the required exponent in the range [-8, 7].
//!
//! This macro takes a value and prepares it to be placed as a Unit Exponent
//! entry into a HID report structure.  This is the exponent applied to
//! PhysicalMinimum and PhysicalMaximum when scaling and converting control
//! values to "real" units.
//!
//! \return Not a function.
//
//*****************************************************************************
#define UnitExponent(cValue)    0x55, ((cValue) & 0x0f)

//*****************************************************************************
//
//! This is a macro to assist adding Unit entries for uncommon units in HID
//! report descriptors.
//!
//! \param ulValue is the definition of the unit required as defined in section
//! 6.2.2.7 of the USB HID device class definition document.
//!
//! This macro takes a value and prepares it to be placed as a Unit entry into
//! a HID report structure.  Note that individual macros are defined for common
//! units and this macro is intended for use when a complex or uncommon unit
//! is needed.  It allows entry of a 5 nibble unit definition into the report
//! descriptor.
//!
//! \return Not a function.
//
//*****************************************************************************
#define Unit(ulValue)           0x67, (ulValue) & 0x0f),                      \
                                (((ulValue) >> 8) & 0xFF),                    \
                                (((ulValue) >> 16) & 0xFF),                   \
                                (((ulValue) >> 24) & 0xFF)

//*****************************************************************************
//
//! This macro inserts a Unit entry for centimeters into a report descriptor.
//!
//*****************************************************************************
#define UnitDistance_cm         0x66, 0x11, 0x00

//*****************************************************************************
//
//! This macro inserts a Unit entry for inches into a report descriptor.
//!
//*****************************************************************************
#define UnitDistance_i          0x66, 0x13, 0x00

//*****************************************************************************
//
//! This macro inserts a Unit entry for degrees into a report descriptor.
//!
//*****************************************************************************
#define UnitRotation_deg        0x66, 0x14, 0x00

//*****************************************************************************
//
//! This macro inserts a Unit entry for radians into a report descriptor.
//!
//*****************************************************************************
#define UnitRotation_rad        0x66, 0x12, 0x00

//*****************************************************************************
//
//! This macro inserts a Unit entry for grams into a report descriptor.
//!
//*****************************************************************************
#define UnitMass_g              0x66, 0x01, 0x01

//*****************************************************************************
//
//! This macro inserts a Unit entry for seconds into a report descriptor.
//!
//*****************************************************************************
#define UnitTime_s              0x66, 0x01, 0x10

//*****************************************************************************
//
//! This macro inserts a Unit entry for temperature in Kelvin into a report
//! descriptor.
//!
//*****************************************************************************
#define UnitTemp_K              0x67, 0x01, 0x00, 0x01, 0x00

//*****************************************************************************
//
//! This macro inserts a Unit entry for temperature in Fahrenheit into a report
//! descriptor.
//!
//*****************************************************************************
#define UnitTemp_F              0x67, 0x03, 0x00, 0x01, 0x00

//*****************************************************************************
//
//! This macro inserts a Unit entry for velocity in cm/s into a report
//! descriptor.
//!
//*****************************************************************************
#define UnitVelocitySI          0x66, 0x11, 0xF0

//*****************************************************************************
//
//! This macro inserts a Unit entry for momentum in (grams * cm)/s into a
//! report descriptor.
//!
//*****************************************************************************
#define UnitMomentumSI          0x66, 0x11, 0xF1

//*****************************************************************************
//
//! This macro inserts a Unit entry for acceleration in cm/s**2 into a
//! report descriptor.
//!
//*****************************************************************************
#define UnitAccelerationSI      0x66, 0x11, 0xE0

//*****************************************************************************
//
//! This macro inserts a Unit entry for force in (cm * grams)/s**2 into a
//! report descriptor.
//!
//*****************************************************************************
#define UnitForceSI             0x66, 0x11, 0xE1

//*****************************************************************************
//
//! This macro inserts a Unit entry for energy in (grams * cm**2)/s**2 into a
//! report descriptor.
//!
//*****************************************************************************
#define UnitEnergySI            0x66, 0x21, 0xE1

//*****************************************************************************
//
//! This macro inserts a Unit entry for angular acceleration in degrees/s**2
//! into a report descriptor.
//!
//*****************************************************************************
#define UnitAngAccelerationSI   0x66, 0x12, 0xE0

//*****************************************************************************
//
//! This macro inserts a Unit entry for voltage into a a report descriptor.
//!
//*****************************************************************************
#define UnitVoltage             0x67, 0x21, 0xD1, 0xF0, 0x00

//*****************************************************************************
//
//! This macro inserts a Unit entry for voltage into a a report descriptor.
//!
//*****************************************************************************
#define UnitCurrent_A           0x67, 0x01, 0x00, 0x10, 0x00

//*****************************************************************************
//
// PRIVATE
//
// The first few sections of this header are private defines that are used by
// the USB HID code and are here only to help with the application
// allocating the correct amount of memory for the HID device code.
//
//*****************************************************************************

//*****************************************************************************
//
// PRIVATE
//
// This enumeration holds the various states that the device can be in during
// normal operation.
//
//*****************************************************************************
typedef enum
{
    //
    // Unconfigured.
    //
    HID_STATE_UNCONFIGURED,

    //
    // No outstanding transaction remains to be completed.
    //
    HID_STATE_IDLE,

    //
    // Waiting on completion of a send or receive transaction.
    //
    HID_STATE_WAIT_DATA
}
tHIDState;

//*****************************************************************************
//
// PRIVATE
//
// This structure defines the private instance data and state variables for
// HID devices.  The memory for this structure is pointed to by the
// psPrivateHIDData field in the tUSBDHIDDevice structure passed in the
// USBDHIDInit() function.
//
//*****************************************************************************
typedef struct
{
    unsigned long ulUSBBase;
    tDeviceInfo *psDevInfo;
    tConfigDescriptor *psConfDescriptor;
    volatile tHIDState eHIDRxState;
    volatile tHIDState eHIDTxState;
    volatile unsigned short usDeferredOpFlags;
    unsigned short usInReportSize;
    unsigned short usInReportIndex;
    unsigned short usOutReportSize;
    unsigned char *pucInReportData;
    unsigned char *pucOutReportData;
    volatile tBoolean bConnected;
    volatile tBoolean bSendInProgress;
    tBoolean bGetRequestPending;
    unsigned char ucINEndpoint;
    unsigned char ucOUTEndpoint;
    unsigned char ucInterface;
}
tHIDInstance;

#ifndef DEPRECATED
//*****************************************************************************
//
// The number of bytes of workspace required by the HID device class driver.
// The client must provide a block of RAM of at least this size in the
// pvWorkspace field of the tUSBDHIDDevice structure passed on USBDHIDInit.
//
//*****************************************************************************
#define USB_HID_WORKSPACE_SIZE  sizeof(tHIDInstance)
#endif

//*****************************************************************************
//
//! The structure used to track idle time for reports.  An array of these
//! structures is passed to the HID device class driver during USBDHIDInit and
//! is used to track automatic resending of each report (if not disabled by
//! the host).
//
//*****************************************************************************
typedef struct
{
    //
    //! The idle duration for the report expressed in units of 4mS.  0
    //! indicates infinite and informs the class driver not to send the report
    //! unless a state change occurs.
    //
    unsigned char ucDuration4mS;

    //
    //! The ID of the report which this structure applies to.  This is the
    //! report ID as specified using a ReportID tag in the report descriptor
    //! rather than the index of the report in the HID class descriptor array.
    //! If only a single Input report is supported and, thus, no ReportID tag
    //! is present, this field should be set to 0.
    //
    unsigned char ucReportID;

    //
    //! The number of milliseconds before we need to send a copy of a given
    //! report back to the host.  This field is updated by the HID driver and
    //! used to time sending of USBD_HID_EVENT_IDLE_TIMEOUT.
    //
    unsigned short usTimeTillNextmS;

    //
    //! The number of milliseconds that have passed since the last time this
    //! report was sent.  The HID class driver needs to track this since
    //! Set_Idle requests are required to take effect as if issued immediately
    //! after the last transmission of the report to which they refer.
    //
    unsigned long ulTimeSinceReportmS;
}
tHIDReportIdle;

//*****************************************************************************
//
//! The structure used by the application to define operating parameters for
//! the HID device.
//
//*****************************************************************************
typedef struct
{
    //
    //! The vendor ID that this device is to present in the device descriptor.
    //
    unsigned short usVID;

    //
    //! The product ID that this device is to present in the device descriptor.
    //
    unsigned short usPID;

    //
    //! The maximum power consumption of the device, expressed in milliamps.
    //
    unsigned short usMaxPowermA;

    //
    //! Indicates whether the device is self- or bus-powered and whether or not
    //! it supports remote wakeup.  Valid values are USB_CONF_ATTR_SELF_PWR or
    //! USB_CONF_ATTR_BUS_PWR, optionally ORed with USB_CONF_ATTR_RWAKE.
    //
    unsigned char ucPwrAttributes;

    //
    //! The interface subclass to publish to the server for this HID device.
    //
    unsigned char ucSubclass;

    //
    //! The interface protocol to publish to the server for this HID device.
    //
    unsigned char ucProtocol;

    //
    //! The number of Input reports that this device supports.  This field
    //! must equal the number of reports published in the HID class descriptors
    //! for the device and also the number of entries in the array whose first
    //! element is pointed to by field psReportIdle below.
    //
    unsigned char ucNumInputReports;

    //
    //! A pointer to the first element in an array of structures used to track
    //! idle time for each Input report.  When USBDHIDInit is called, the
    //! ucDuration4mS and ucReportID fields of each of these array members
    //! should be initialized to indicate the default idle timeout for each
    //! input report.  This array must be in RAM since the HID device class
    //! driver will update values in it in response to requests from the host
    //! and to track elapsed time.  The number of elements in the array must
    //! match the number supplied in the ucNumInputReports field above.
    //
    tHIDReportIdle *psReportIdle;

    //! A pointer to the callback function which will be called to notify
    //! the application of general events, events related to report transfers
    //! on endpoint zero and events related to reception of Output and Feature
    //! reports via the (optional) interrupt OUT endpoint.
    //
    tUSBCallback pfnRxCallback;

    //
    //! A client-supplied pointer which will be sent as the first
    //! parameter in all calls made to the receive channel callback,
    //! pfnRxCallback.
    //
    void *pvRxCBData;

    //
    //! A pointer to the callback function which will be called to notify
    //! the application of events related to transmission of Input reports
    //! via the interrupt IN endpoint.
    //
    tUSBCallback pfnTxCallback;

    //
    //! A client-supplied pointer which will be sent as the first
    //! parameter in all calls made to the transmit channel callback,
    //! pfnTxCallback.
    //
    void *pvTxCBData;

    //
    //! If set to true, this field indicates that the device should use a
    //! dedicated interrupt OUT endpoint to receive reports from the host.  In
    //! this case, reports from the host are passed to the application via the
    //! receive callback using USB_EVENT_RX_AVAILABLE events.  If false,
    //! reports from the host are received via endpoint zero and passed to the
    //! application via USBD_HID_EVENT_REPORT_SENT events.
    //
    tBoolean bUseOutEndpoint;

    //
    //! The HID descriptor that the device is to publish (following the
    //! standard interface descriptor and prior to the endpoint descriptors for
    //! the interface).
    //
    const tHIDDescriptor *psHIDDescriptor;

    //
    //! The HID class descriptors offered by the device are defined in an
    //! array of byte pointers and this field points to that array.  The
    //! order and number of elements in the array must match the associated
    //! information provided in the HID descriptor in field by psHIDDescriptor.
    //
    const unsigned char * const *ppClassDescriptors;

    //
    //! A pointer to the string descriptor array for this device.  This array
    //! must contain the following string descriptor pointers in this order.
    //! Language descriptor, Manufacturer name string (language 1), Product
    //! name string (language 1), Serial number string (language 1),HID
    //! Interface description string (language 1), Configuration description
    //! string (language 1), (optionally) First HID device-specific string
    //! (language 1), (optionally) Second HID device-specific string (language
    //! 1), etc.
    //!
    //! If supporting more than 1 language, the descriptor block (except for
    //! string descriptor 0) must be repeated for each language defined in the
    //! language descriptor.
    //!
    //! The number of HID device-specific strings is dependent upon the content
    //! of the report descriptor passed to the interface and is, thus,
    //! application controlled.
    //
    const unsigned char * const *ppStringDescriptors;

    //
    //! The number of descriptors provided in the ppStringDescriptors
    //! array.  This must be 1 + ((5 + (num HID strings)) * (num languages)).
    //
    unsigned long ulNumStringDescriptors;

    //
    //! A pointer to private instance data for this device instance.  This
    //! memory must remain accessible for as long as the HID device is in use
    //! and must not be modified by any code outside the HID class driver.
    //
    tHIDInstance *psPrivateHIDData;
}
tUSBDHIDDevice;

//*****************************************************************************
//
// HID-specific device class driver events
//
//*****************************************************************************

//*****************************************************************************
//
//! This event indicates that the host is requesting a particular report be
//! returned via endpoint 0, the control endpoint.  The ulMsgValue parameter
//! contains the requested report type in the high byte and report ID in the
//! low byte (as passed in the wValue field of the USB request structure).
//! The pvMsgData parameter contains a pointer which must be written with the
//! address of the first byte of the requested report.  The callback must
//! return the size in bytes of the report pointed to by *pvMsgData.  The
//! memory returned in response to this event must remain unaltered until
//! USBD_HID_EVENT_REPORT_SENT is sent.
//
//*****************************************************************************
#define USBD_HID_EVENT_GET_REPORT                                             \
                                (USBD_HID_EVENT_BASE + 0)

//*****************************************************************************
//
//! This event indicates that a report previously requested via a
//! USBD_HID_EVENT_GET_REPORT has been successfully transmitted to the host.
//! The application may now free or reuse the report memory passed on the
//! previous event.  Although this would seem to be an event that would be
//! passed to the transmit channel callback, it is actually passed to the
//! receive channel callback.  This ensures that all events related to the
//! request and transmission of reports via endpoint zero can be handled in
//! a single function.
//
//*****************************************************************************
#define USBD_HID_EVENT_REPORT_SENT                                            \
                                (USBD_HID_EVENT_BASE + 1)

//*****************************************************************************
//
//! This event indicates that the host has sent a Set_Report request to
//! the device and requests that the device provide a buffer into which the
//! report can be written.  The ulMsgValue parameter contains the received
//! report type in the high byte and report ID in the low byte (as passed in
//! the wValue field of the USB request structure).  The pvMsgData parameter
//! contains the length of buffer requested.  Note that this is the actual
//! length value cast to a "void *" type and not a pointer in this case.
//! The callback must return a pointer to a suitable buffer (cast to the
//! standard "unsigned long" return type for the callback).
//
//*****************************************************************************
#define USBD_HID_EVENT_GET_REPORT_BUFFER                                      \
                                (USBD_HID_EVENT_BASE + 2)

//*****************************************************************************
//
//! This event indicates that the host has sent the device a report via
//! endpoint 0, the control endpoint.  The ulMsgValue field indicates the
//! size of the report and pvMsgData points to the first byte of the report.
//! The report buffer will previously have been returned in response to an
//! earlier USBD_HID_EVENT_GET_REPORT_BUFFER callback.  The HID device class
//! driver will not access the memory pointed to by pvMsgData after this
//! callback is made so the application is free to reuse or free it at this
//! point.
//
//*****************************************************************************
#define USBD_HID_EVENT_SET_REPORT                                             \
                                (USBD_HID_EVENT_BASE + 3)

//*****************************************************************************
//
//! This event is sent in response to a Get_Protocol request from the host.
//! The callback should provide the current protocol via the return code,
//! USB_HID_PROTOCOL_BOOT or USB_HID_PROTOCOL_REPORT.
//
//*****************************************************************************
#define USBD_HID_EVENT_GET_PROTOCOL                                           \
                                (USBD_HID_EVENT_BASE + 4)

//*****************************************************************************
//
//! This event is sent in response to a Set_Protocol request from the host.
//! The ulMsgData value will contain the requested protocol,
//! USB_HID_PROTOCOL_BOOT or USB_HID_PROTOCOL_REPORT.
//
//*****************************************************************************
#define USBD_HID_EVENT_SET_PROTOCOL                                           \
                                (USBD_HID_EVENT_BASE + 5)

//*****************************************************************************
//
//! This event indicates to an application that a report idle timeout has
//! occurred and requests a pointer to the report that must be sent back to
//! the host.  The ulMsgData value will contain the requested report ID and
//! pvMsgData contains a pointer that must be written with a pointer to the
//! report data that is to be sent.  The callback must return the number of
//! bytes in the report pointed to by *pvMsgData.
//
//*****************************************************************************
#define USBD_HID_EVENT_IDLE_TIMEOUT                                           \
                                (USBD_HID_EVENT_BASE + 6)

extern tDeviceInfo g_sHIDDeviceInfo;

//*****************************************************************************
//
// API Function Prototypes
//
//*****************************************************************************
extern void *USBDHIDInit(unsigned long ulIndex,
                         const tUSBDHIDDevice *psHIDDevice);
extern void *USBDHIDCompositeInit(unsigned long ulIndex,
                                  const tUSBDHIDDevice *psDevice);

extern void USBDHIDTerm(void *pvInstance);
extern void *USBDHIDSetRxCBData(void *pvInstance, void *pvCBData);
extern void *USBDHIDSetTxCBData(void *pvInstance, void *pvCBData);
extern unsigned long USBDHIDReportWrite(void *pvInstance,
                                        unsigned char *pcData,
                                        unsigned long ulLength,
                                        tBoolean bLast);
extern unsigned long USBDHIDPacketRead(void *pvInstance,
                                       unsigned char *pcData,
                                       unsigned long ulLength,
                                       tBoolean bLast);
extern unsigned long USBDHIDTxPacketAvailable(void *pvInstance);
extern unsigned long USBDHIDRxPacketAvailable(void *pvInstance);
extern void USBDHIDPowerStatusSet(void *pvInstance, unsigned char ucPower);
extern tBoolean USBDHIDRemoteWakeupRequest(void *pvInstance);

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __USBDHID_H__
