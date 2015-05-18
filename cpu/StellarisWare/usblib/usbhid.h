//*****************************************************************************
//
// usbhid.h - Definitions used by HID class devices and hosts.
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

#ifndef __USBHID_H__
#define __USBHID_H__

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
// HID Interface descriptor Subclasses.
//
//*****************************************************************************
#define USB_HID_SCLASS_NONE     0x00
#define USB_HID_SCLASS_BOOT     0x01

//*****************************************************************************
//
// USB Interface descriptor HID protocols.
//
//*****************************************************************************
#define USB_HID_PROTOCOL_NONE   0
#define USB_HID_PROTOCOL_KEYB   1
#define USB_HID_PROTOCOL_MOUSE  2

//*****************************************************************************
//
// HID Class descriptor types.
//
//*****************************************************************************
#define USB_HID_DTYPE_HID       0x21
#define USB_HID_DTYPE_REPORT    0x22
#define USB_HID_DTYPE_PHYSICAL  0x23

//*****************************************************************************
//
// HID USB requests.
//
//*****************************************************************************
#define USBREQ_GET_REPORT       0x01
#define USBREQ_GET_IDLE         0x02
#define USBREQ_GET_PROTOCOL     0x03
#define USBREQ_SET_REPORT       0x09
#define USBREQ_SET_IDLE         0x0a
#define USBREQ_SET_PROTOCOL     0x0b

//*****************************************************************************
//
// GET_REPORT or SET_REPORT Definitions.
//
//*****************************************************************************
#define USB_HID_REPORT_IN       0x01
#define USB_HID_REPORT_OUTPUT   0x02
#define USB_HID_REPORT_FEATURE  0x03

//*****************************************************************************
//
// GET_PROTOCOL or SET_PROTOCOL Definitions.
//
//*****************************************************************************
#define USB_HID_PROTOCOL_BOOT   0
#define USB_HID_PROTOCOL_REPORT 1

//*****************************************************************************
//
// Report Values used with the Report macros.
//
//*****************************************************************************
#define USB_HID_GENERIC_DESKTOP 0x01
#define USB_HID_BUTTONS         0x09
#define USB_HID_X               0x30
#define USB_HID_Y               0x31

#define USB_HID_POINTER         0x01
#define USB_HID_MOUSE           0x02
#define USB_HID_KEYBOARD        0x06

#define USB_HID_PHYSICAL        0x00
#define USB_HID_APPLICATION     0x01

#define USB_HID_USAGE_POINTER   0x0109
#define USB_HID_USAGE_BUTTONS   0x0509
#define USB_HID_USAGE_LEDS      0x0508
#define USB_HID_USAGE_KEYCODES  0x0507

//*****************************************************************************
//
// HID mouse button definitions as used in the first byte of the output report
// used in the BIOS mouse protocol.
//
//*****************************************************************************
#define HID_MOUSE_BUTTON_1      0x01
#define HID_MOUSE_BUTTON_2      0x02
#define HID_MOUSE_BUTTON_3      0x04

//*****************************************************************************
//
// HID Keyboard LED definitions as used in the first byte of the output report
// used in the BIOS keyboard protocol.
//
//*****************************************************************************
#define HID_KEYB_NUM_LOCK       0x01
#define HID_KEYB_CAPS_LOCK      0x02
#define HID_KEYB_SCROLL_LOCK    0x04
#define HID_KEYB_COMPOSE        0x08
#define HID_KEYB_KANA           0x10

//*****************************************************************************
//
// HID Keyboard key modifiers as provided in the first byte of the input report
// used in the BIOS keyboard protocol.
//
//*****************************************************************************
#define HID_KEYB_LEFT_CTRL      0x01
#define HID_KEYB_LEFT_SHIFT     0x02
#define HID_KEYB_LEFT_ALT       0x04
#define HID_KEYB_LEFT_GUI       0x08
#define HID_KEYB_RIGHT_CTRL     0x10
#define HID_KEYB_RIGHT_SHIFT    0x20
#define HID_KEYB_RIGHT_ALT      0x40
#define HID_KEYB_RIGHT_GUI      0x80

//*****************************************************************************
//
// A subset of the HID keyboard usage IDs.
//
//*****************************************************************************
#define HID_KEYB_USAGE_RESERVED 0x00
#define HID_KEYB_USAGE_ROLLOVER 0x01
#define HID_KEYB_USAGE_A        0x04
#define HID_KEYB_USAGE_B        0x05
#define HID_KEYB_USAGE_C        0x06
#define HID_KEYB_USAGE_D        0x07
#define HID_KEYB_USAGE_E        0x08
#define HID_KEYB_USAGE_F        0x09
#define HID_KEYB_USAGE_G        0x0A
#define HID_KEYB_USAGE_H        0x0B
#define HID_KEYB_USAGE_I        0x0C
#define HID_KEYB_USAGE_J        0x0D
#define HID_KEYB_USAGE_K        0x0E
#define HID_KEYB_USAGE_L        0x0F
#define HID_KEYB_USAGE_M        0x10
#define HID_KEYB_USAGE_N        0x11
#define HID_KEYB_USAGE_O        0x12
#define HID_KEYB_USAGE_P        0x13
#define HID_KEYB_USAGE_Q        0x14
#define HID_KEYB_USAGE_R        0x15
#define HID_KEYB_USAGE_S        0x16
#define HID_KEYB_USAGE_T        0x17
#define HID_KEYB_USAGE_U        0x18
#define HID_KEYB_USAGE_V        0x19
#define HID_KEYB_USAGE_W        0x1A
#define HID_KEYB_USAGE_X        0x1B
#define HID_KEYB_USAGE_Y        0x1C
#define HID_KEYB_USAGE_Z        0x1D
#define HID_KEYB_USAGE_1        0x1E
#define HID_KEYB_USAGE_2        0x1F
#define HID_KEYB_USAGE_3        0x20
#define HID_KEYB_USAGE_4        0x21
#define HID_KEYB_USAGE_5        0x22
#define HID_KEYB_USAGE_6        0x23
#define HID_KEYB_USAGE_7        0x24
#define HID_KEYB_USAGE_8        0x25
#define HID_KEYB_USAGE_9        0x26
#define HID_KEYB_USAGE_0        0x27
#define HID_KEYB_USAGE_ENTER    0x28
#define HID_KEYB_USAGE_ESCAPE   0x29
#define HID_KEYB_USAGE_BACKSPACE \
                                0x2A
#define HID_KEYB_USAGE_TAB      0x2B
#define HID_KEYB_USAGE_SPACE    0x2C
#define HID_KEYB_USAGE_MINUS    0x2D
#define HID_KEYB_USAGE_EQUAL    0x2E
#define HID_KEYB_USAGE_LBRACKET 0x2F
#define HID_KEYB_USAGE_RBRACKET 0x30
#define HID_KEYB_USAGE_BSLASH   0x31
#define HID_KEYB_USAGE_SEMICOLON \
                                0x33
#define HID_KEYB_USAGE_FQUOTE   0x34
#define HID_KEYB_USAGE_BQUOTE   0x35
#define HID_KEYB_USAGE_COMMA    0x36
#define HID_KEYB_USAGE_PERIOD   0x37
#define HID_KEYB_USAGE_FSLASH   0x38
#define HID_KEYB_USAGE_CAPSLOCK 0x39
#define HID_KEYB_USAGE_F1       0x3A
#define HID_KEYB_USAGE_F2       0x3B
#define HID_KEYB_USAGE_F3       0x3C
#define HID_KEYB_USAGE_F4       0x3D
#define HID_KEYB_USAGE_F5       0x3E
#define HID_KEYB_USAGE_F6       0x3F
#define HID_KEYB_USAGE_F7       0x40
#define HID_KEYB_USAGE_F8       0x41
#define HID_KEYB_USAGE_F9       0x42
#define HID_KEYB_USAGE_F10      0x43
#define HID_KEYB_USAGE_F11      0x44
#define HID_KEYB_USAGE_F12      0x45
#define HID_KEYB_USAGE_SCROLLOCK   \
                                0x47
#define HID_KEYB_USAGE_RIGHT_ARROW \
                                0x4F
#define HID_KEYB_USAGE_LEFT_ARROW \
                                0x50
#define HID_KEYB_USAGE_DOWN_ARROW \
                                0x51
#define HID_KEYB_USAGE_UP_ARROW 0x52
#define HID_KEYB_USAGE_NUMLOCK  0x53
#define HID_KEYB_USAGE_KEYPAD_SLASH \
                                0x54
#define HID_KEYB_USAGE_KEYPAD_STAR \
                                0x55
#define HID_KEYB_USAGE_KEYPAD_MINUS \
                                0x56
#define HID_KEYB_USAGE_KEYPAD_PLUS \
                                0x57
#define HID_KEYB_USAGE_KEPAD_ENTER \
                                0x58
#define HID_KEYB_USAGE_KEYPAD_1 0x59
#define HID_KEYB_USAGE_KEYPAD_2 0x5A
#define HID_KEYB_USAGE_KEYPAD_3 0x5B
#define HID_KEYB_USAGE_KEYPAD_4 0x5C
#define HID_KEYB_USAGE_KEYPAD_5 0x5D
#define HID_KEYB_USAGE_KEYPAD_6 0x5E
#define HID_KEYB_USAGE_KEYPAD_7 0x5F
#define HID_KEYB_USAGE_KEYPAD_8 0x60
#define HID_KEYB_USAGE_KEYPAD_9 0x61
#define HID_KEYB_USAGE_KEYPAD_0 0x62
#define HID_KEYB_USAGE_KEPAD_PERIOD \
                                0x63

//*****************************************************************************
//
// HID descriptor country codes (most of these are described as "countries" in
// the HID specification even though they are really languages).
//
//*****************************************************************************
#define USB_HID_COUNTRY_NONE    0x00
#define USB_HID_COUNTRY_ARABIC  0x01
#define USB_HID_COUNTRY_BELGIAN 0x02
#define USB_HID_COUNTRY_CANADA_BI \
                                0x03
#define USB_HID_COUNTRY_CANADA_FR \
                                0x04
#define USB_HID_COUNTRY_CZECH_REPUBLIC \
                                0x05
#define USB_HID_COUNTRY_DANISH  0x06
#define USB_HID_COUNTRY_FINNISH 0x07
#define USB_HID_COUNTRY_FRENCH  0x08
#define USB_HID_COUNTRY_GERMAN  0x09
#define USB_HID_COUNTRY_GREEK   0x0A
#define USB_HID_COUNTRY_HEBREW  0x0B
#define USB_HID_COUNTRY_HUNGARY 0x0C
#define USB_HID_COUNTRY_INTERNATIONAL_ISO \
                                0x0D
#define USB_HID_COUNTRY_ITALIAN 0x0E
#define USB_HID_COUNTRY_JAPAN_KATAKANA \
                                0x0F
#define USB_HID_COUNTRY_KOREAN  0x10
#define USB_HID_COUNTRY_LATIN_AMERICAN \
                                0x11
#define USB_HID_COUNTRY_NETHERLANDS \
                                0x12
#define USB_HID_COUNTRY_NORWEGIAN \
                                0x13
#define USB_HID_COUNTRY_PERSIAN 0x14
#define USB_HID_COUNTRY_POLAND  0x15
#define USB_HID_COUNTRY_PORTUGUESE \
                                0x16
#define USB_HID_COUNTRY_RUSSIA  0x17
#define USB_HID_COUNTRY_SLOVAKIA \
                                0x18
#define USB_HID_COUNTRY_SPANISH 0x19
#define USB_HID_COUNTRY_SWEDISH 0x1A
#define USB_HID_COUNTRY_SWISS_FRENCH \
                                0x1B
#define USB_HID_COUNTRY_SWISS_GERMAN \
                                0x1C
#define USB_HID_COUNTRY_SWITZERLAND \
                                0x1D
#define USB_HID_COUNTRY_TAIWAN  0x1E
#define USB_HID_COUNTRY_TURKISH_Q \
                                0x1F
#define USB_HID_COUNTRY_UK      0x20
#define USB_HID_COUNTRY_US      0x21
#define USB_HID_COUNTRY_YUGOSLAVIA \
                                0x22
#define USB_HID_COUNTRY_TURKISH_F \
                                0x23

//*****************************************************************************
//
// Data flags used in Input item tags within report descriptors.
//
//*****************************************************************************
#define USB_HID_INPUT_DATA      0x0000
#define USB_HID_INPUT_CONSTANT  0x0001
#define USB_HID_INPUT_ARRAY     0x0000
#define USB_HID_INPUT_VARIABLE  0x0002
#define USB_HID_INPUT_ABS       0x0000
#define USB_HID_INPUT_RELATIVE  0x0004
#define USB_HID_INPUT_NOWRAP    0x0000
#define USB_HID_INPUT_WRAP      0x0008
#define USB_HID_INPUT_LINEAR    0x0000
#define USB_HID_INPUT_NONLINEAR 0x0010
#define USB_HID_INPUT_PREFER    0x0000
#define USB_HID_INPUT_NONPREFER 0x0020
#define USB_HID_INPUT_NONULL    0x0000
#define USB_HID_INPUT_NULL      0x0040
#define USB_HID_INPUT_BITF      0x0100
#define USB_HID_INPUT_BYTES     0x0000

//*****************************************************************************
//
// Data flags used in Feature item tags within report descriptors.
//
//*****************************************************************************
#define USB_HID_FEATURE_DATA    0x0000
#define USB_HID_FEATURE_CONSTANT \
                                0x0001
#define USB_HID_FEATURE_ARRAY   0x0000
#define USB_HID_FEATURE_VARIABLE \
                                0x0002
#define USB_HID_FEATURE_ABS     0x0000
#define USB_HID_FEATURE_RELATIVE \
                                0x0004
#define USB_HID_FEATURE_NOWRAP  0x0000
#define USB_HID_FEATURE_WRAP    0x0008
#define USB_HID_FEATURE_LINEAR  0x0000
#define USB_HID_FEATURE_NONLINEAR \
                                0x0010
#define USB_HID_FEATURE_PREFER  0x0000
#define USB_HID_FEATURE_NONPREFER \
                                0x0020
#define USB_HID_FEATURE_NONULL  0x0000
#define USB_HID_FEATURE_NULL    0x0040
#define USB_HID_FEATURE_BITF    0x0100
#define USB_HID_FEATURE_BYTES   0x0000

//*****************************************************************************
//
// Data flags used in Output item tags within report descriptors.
//
//*****************************************************************************
#define USB_HID_OUTPUT_DATA     0x0000
#define USB_HID_OUTPUT_CONSTANT 0x0001
#define USB_HID_OUTPUT_ARRAY    0x0000
#define USB_HID_OUTPUT_VARIABLE 0x0002
#define USB_HID_OUTPUT_ABS      0x0000
#define USB_HID_OUTPUT_RELATIVE 0x0004
#define USB_HID_OUTPUT_NOWRAP   0x0000
#define USB_HID_OUTPUT_WRAP     0x0008
#define USB_HID_OUTPUT_LINEAR   0x0000
#define USB_HID_OUTPUT_NONLINEAR \
                                0x0010
#define USB_HID_OUTPUT_PREFER   0x0000
#define USB_HID_OUTPUT_NONPREFER \
                                0x0020
#define USB_HID_OUTPUT_NONULL   0x0000
#define USB_HID_OUTPUT_NULL     0x0040
#define USB_HID_OUTPUT_BITF     0x0100
#define USB_HID_OUTPUT_BYTES    0x0000

//*****************************************************************************
//
// Physical descriptor bias values.
//
//*****************************************************************************
#define USB_HID_BIAS_NOT_APPLICABLE \
                                0x00
#define USB_HID_BIAS_RIGHT_HAND 0x01
#define USB_HID_BIAS_LEFT_HAND  0x02
#define USB_HID_BIAS_BOTH_HANDS 0x03
#define USB_HID_BIAS_EITHER_HAND \
                                0x04

//*****************************************************************************
//
// Physical descriptor designator values.
//
//*****************************************************************************
#define USB_HID_DESIGNATOR_NONE 0x00
#define USB_HID_DESIGNATOR_HAND 0x01
#define USB_HID_DESIGNATOR_EYEBALL \
                                0x02
#define USB_HID_DESIGNATOR_EYEBROW \
                                0x03
#define USB_HID_DESIGNATOR_EYELID \
                                0x04
#define USB_HID_DESIGNATOR_EAR  0x05
#define USB_HID_DESIGNATOR_NOSE 0x06
#define USB_HID_DESIGNATOR_MOUTH \
                                0x07
#define USB_HID_DESIGNATOR_UPPER_LIP \
                                0x08
#define USB_HID_DESIGNATOR_LOWER_LIP \
                                0x09
#define USB_HID_DESIGNATOR_JAW  0x0A
#define USB_HID_DESIGNATOR_NECK 0x0B
#define USB_HID_DESIGNATOR_UPPER_ARM \
                                0x0C
#define USB_HID_DESIGNATOR_ELBOW \
                                0x0D
#define USB_HID_DESIGNATOR_FOREARM \
                                0x0E
#define USB_HID_DESIGNATOR_WRIST \
                                0x0F
#define USB_HID_DESIGNATOR_PALM 0x10
#define USB_HID_DESIGNATOR_THUMB \
                                0x11
#define USB_HID_DESIGNATOR_INDEX_FINGER \
                                0x12
#define USB_HID_DESIGNATOR_MIDDLE_FINGER \
                                0x13
#define USB_HID_DESIGNATOR_RING_FINGER \
                                0x14
#define USB_HID_DESIGNATOR_LITTLE_FINGER \
                                0x15
#define USB_HID_DESIGNATOR_HEAD 0x16
#define USB_HID_DESIGNATOR_SHOULDER \
                                0x17
#define USB_HID_DESIGNATOR_HIP  0x18
#define USB_HID_DESIGNATOR_WAIST \
                                0x19
#define USB_HID_DESIGNATOR_THIGH \
                                0x1A
#define USB_HID_DESIGNATOR_KNEE 0x1B
#define USB_HID_DESIGNATOR_CALF 0x1C
#define USB_HID_DESIGNATOR_ANKLE \
                                0x1D
#define USB_HID_DESIGNATOR_FOOT 0x1E
#define USB_HID_DESIGNATOR_HEEL 0x1F
#define USB_HID_DESIGNATOR_BALL_OF_FOOT \
                                0x20
#define USB_HID_DESIGNATOR_BIG_TOE \
                                0x21
#define USB_HID_DESIGNATOR_SECOND_TOE \
                                0x22
#define USB_HID_DESIGNATOR_THIRD_TOE \
                                0x23
#define USB_HID_DESIGNATOR_FOURTH_TOE \
                                0x24
#define USB_HID_DESIGNATOR_LITTLE_TOE \
                                0x25
#define USB_HID_DESIGNATOR_BROW 0x26
#define USB_HID_DESIGNATOR_CHEEK \
                                0x27

//*****************************************************************************
//
// Physical descriptor qualifier values.
//
//*****************************************************************************
#define USB_HID_QUALIFIER_NOT_APPLICABLE    \
                                (0x00 << 5)
#define USB_HID_QUALIFIER_RIGHT (0x01 << 5)
#define USB_HID_QUALIFIER_LEFT  (0x02 << 5)
#define USB_HID_QUALIFIER_BOTH  (0x03 << 5)
#define USB_HID_QUALIFIER_EITHER \
                                (0x04 << 5)
#define USB_HID_QUALIFIER_CENTER \
                                (0x05 << 5)

//*****************************************************************************
//
// This is the maximum value for a usage code.
//
//*****************************************************************************
#define USBH_HID_MAX_USAGE      256
#define USBH_HID_CAPS_ARRAY_SZ  (USBH_HID_MAX_USAGE/sizeof(unsigned long))

//*****************************************************************************
//
// All structures defined in this section of the header require byte packing of
// fields.  This is usually a ccomplished using the PACKED macro but, for IAR
// Embedded Workbench, this requires a pragma.
//
//*****************************************************************************
#ifdef ewarm
#pragma pack(1)
#endif

//*****************************************************************************
//
//! The class descriptor information structure is used to announce the presence
//! of HID-specific class descriptors within the HID descriptor.
//
//*****************************************************************************
typedef struct
{
    //
    //! The type of HID class descriptor.  This will be USB_HID_DTYPE_REPORT or
    //! USB_HID_DTYPE_PHYSICAL.
    //
    unsigned char bDescriptorType;

    //
    //! The total length of the HID class descriptor.
    //
    unsigned short wDescriptorLength;
}
PACKED tHIDClassDescriptorInfo;

//*****************************************************************************
//
//! The HID descriptor is inserted following the interface descriptor and
//! before the endpoint descriptors for a HID class device.
//
//*****************************************************************************
typedef struct
{
    //
    //! The length of this descriptor in bytes.
    //
    unsigned char bLength;

    //
    //! The type of the descriptor.  For a HID descriptor, this will be
    //! USB_HID_DTYPE_HID (0x21).
    //
    unsigned char bDescriptorType;

    //
    //! A BCD value identifying the HID Class specification release supported
    //! by the device.  For version 1.11, for example, this value would be
    //! 0x0111.
    //
    unsigned short bcdHID;

    //
    //! The country code for which this hardware is localized or 0 if no
    //! localization has been performed.  Valid country (or language) codes are
    //! in labels of the form USB_HID_COUNTRY_xxx.
    unsigned char bCountryCode;

    //
    //! The number of class-specific descriptors that exist for this device.
    //! This indicates the number of class descriptor information structures
    //! that are appended to this structure and must be at least 1 (since all
    //! HID devices must publish at least 1 report descriptor).
    //
    unsigned char bNumDescriptors;

    //
    //! A table announcing each of the class-specific descriptors that this
    //! device publishes.  The actual number of entries in the array is given
    //! by the bNumDescriptors field.
    //
    tHIDClassDescriptorInfo sClassDescriptor[1];
}
PACKED tHIDDescriptor;

//*****************************************************************************
//
// Return to default packing when using the IAR Embedded Workbench compiler.
//
//*****************************************************************************
#ifdef ewarm
#pragma pack()
#endif

//*****************************************************************************
//
//! This structure defines the mapping of USB usage identifiers to printable
//! characters.  The structure has three members that hold this information.
//! The ucBytesPerChar, indicates the number of bytes per character in
//! the table.  The pulCapsLock array holds a packed bit array of usage
//! identifiers that can be modified by the Caps Lock key.  The pCharMapping
//! array is treated as a double indexed array with two "columns".  In the case
//! of a single byte character it is treated as pairs of 8 bit values for
//! unshifted and shifted values.  In the case of a double byte characters it is
//! treated as pairs of 16 bit values.
//
//*****************************************************************************
typedef struct
{
    //
    //! Number of bytes per character in the pCharMapping table of this
    //! structure.
    //
    unsigned char ucBytesPerChar;

    //
    //! This is a packed bitmasked structure with a one bit flags that
    //! indicates if the corresponding Usage ID is affected by the Caps Lock
    //! key.
    //
    unsigned long pulCapsLock[USBH_HID_CAPS_ARRAY_SZ];

    //
    //! This is the indexed table of Usage ID to character value.  It must be
    //! at least ucBytesPerChar * 2 * USBH_HID_MAX_USAGE bytes in size as it
    //! is treated as a double indexed array.
    //
    void *pCharMapping;
}
tHIDKeyboardUsageTable;

//*****************************************************************************
//
// The US Keyboard mapping used by USB keyboard usage ID to character mapping.
//
//*****************************************************************************
extern const tHIDKeyboardUsageTable g_sUSKeyboardMap;

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

#endif // __USBHID_H__
