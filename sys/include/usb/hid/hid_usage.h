/*
 * Copyright C 2021 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */
/**
 * @defgroup    usb_hid_usage   USB HID usage tables
 * @ingroup     usb_hid
 * @brief       USB HID usage tables
 * @{
 *
 * @file
 * @brief       Definition for USB HID usage tables
 *
 * See the HID Usage tables document for explanations on the different usage
 * types
 *
 * @see Based on HID Usage Tables 1.21, https://www.usb.org/sites/default/files/hut1_21.pdf
 */

#ifndef USB_HID_HID_USAGE_H
#define USB_HID_HID_USAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Usage page definitions
 * @{
 */
#define USB_HID_USAGE_GD (0x01)             /**< Usage page  Generic Desktop */
#define USB_HID_USAGE_SIM (0x02)            /**< Usage page  Simulation Controls */
#define USB_HID_USAGE_VR (0x03)             /**< Usage page  VR Controls */
#define USB_HID_USAGE_SPORT (0x04)          /**< Usage page  Sport Controls */
#define USB_HID_USAGE_GAME (0x05)           /**< Usage page  Game Controls */
#define USB_HID_USAGE_GDV (0x06)            /**< Usage page  Generic Device Controls */
#define USB_HID_USAGE_KEY (0x07)            /**< Usage page  Keyboard/Keypad */
#define USB_HID_USAGE_LED (0x08)            /**< Usage page  LED */
#define USB_HID_USAGE_TELEPHONY (0x0B)      /**< Usage page  Telephony Device */
#define USB_HID_USAGE_CONSUMER (0x0C)       /**< Usage page  Consumer */
#define USB_HID_USAGE_DIGITIZERS (0x0D)     /**< Usage page  Digitizers */
#define USB_HID_USAGE_HAPTICS (0x0E)        /**< Usage page  Haptics */
#define USB_HID_USAGE_PID (0x0F)            /**< Usage page  PID */
#define USB_HID_USAGE_EHT (0x12)            /**< Usage page  Eye and Head Trackers */
#define USB_HID_USAGE_AUXDISP (0x14)        /**< Usage page  Auxiliary Display */
#define USB_HID_USAGE_SENSORS (0x20)        /**< Usage page  Sensors */
#define USB_HID_USAGE_MEDICAL (0x40)        /**< Usage page  Medical Instrument */
#define USB_HID_USAGE_BRAILLE (0x41)        /**< Usage page  Braille Display */
#define USB_HID_USAGE_LIGHT (0x59)          /**< Usage page  Lighting And Illumination */
#define USB_HID_USAGE_MONITOR (0x80)        /**< Usage page  USB Monitor */
#define USB_HID_USAGE_MONITOR_VALUES (0x81) /**< Usage page  Monitor Enumerated Values */
#define USB_HID_USAGE_MONITOR_VESA (0x82)   /**< Usage page  VESA Virtual Control */
#define USB_HID_USAGE_POWER (0x84)          /**< Usage page  Power */
#define USB_HID_USAGE_POS_BARCODE (0x8C)    /**< Usage page  Bar Code Scanner */
#define USB_HID_USAGE_POS_SCALE (0x8D)      /**< Usage page  Scale */
#define USB_HID_USAGE_POS_MSR (0x8E)        /**< Usage page  Magnetic Stripe Reading (MSR) Devices */
#define USB_HID_USAGE_POS_RESV (0x8F)       /**< Usage page  Reserved Point of Sale */
#define USB_HID_USAGE_CAMERA (0x90)         /**< Usage page  Camera Control */
#define USB_HID_USAGE_ARCADE (0x91)         /**< Usage page  Arcade */
#define USB_HID_USAGE_GAMING (0x92)         /**< Usage page  Gaming Device */
#define USB_HID_USAGE_FIDO (0xF1D0)         /**< Usage page  FIDO Alliance */
/** @} */

#ifndef DOXYGEN /* No sense in adding these to doxygen */
/**
 * @name Page 0x01: Generic Desktop
 * @{
 */
#define USB_HID_USAGE_GD_UNDEFINED 0x00
#define USB_HID_USAGE_GD_POINTER 0x01                                       /* Usage type:  CP */
#define USB_HID_USAGE_GD_MOUSE 0x02                                         /* Usage type:  CA */
#define USB_HID_USAGE_GD_JOYSTICK 0x04                                      /* Usage type:  CA */
#define USB_HID_USAGE_GD_GAMEPAD 0x05                                       /* Usage type:  CA */
#define USB_HID_USAGE_GD_KEYBOARD 0x06                                      /* Usage type:  CA */
#define USB_HID_USAGE_GD_KEYPAD 0x07                                        /* Usage type:  CA */
#define USB_HID_USAGE_GD_MULTI_AXIS_CONTROLLER 0x08                         /* Usage type:  CA */
#define USB_HID_USAGE_GD_TABLET_PC_SYSTEM_CONTROLS 0x09                     /* Usage type:  CA */
#define USB_HID_USAGE_GD_WATER_COOLING_DEVICE 0x0A                          /* Usage type:  CA */
#define USB_HID_USAGE_GD_COMPUTER_CHASSIS_DEVICE 0x0B                       /* Usage type:  CA */
#define USB_HID_USAGE_GD_WIRELESS_RADIO_CONTROLS 0x0C                       /* Usage type:  CA */
#define USB_HID_USAGE_GD_PORTABLE_DEVICE_CONTROL 0x0D                       /* Usage type:  CA */
#define USB_HID_USAGE_GD_SYSTEM_MULTI_AXIS_CONTROLLER 0x0E                  /* Usage type:  CA */
#define USB_HID_USAGE_GD_SPATIAL_CONTROLLER 0x0F                            /* Usage type:  CA */
#define USB_HID_USAGE_GD_ASSISTIVE_CONTROL 0x10                             /* Usage type:  CA */
#define USB_HID_USAGE_GD_DEVICE_DOCK 0x11                                   /* Usage type:  CA */
#define USB_HID_USAGE_GD_DOCKABLE_DEVICE 0x12                               /* Usage type:  CA */
#define USB_HID_USAGE_GD_X 0x30                                             /* Usage type:  DV */
#define USB_HID_USAGE_GD_Y 0x31                                             /* Usage type:  DV */
#define USB_HID_USAGE_GD_Z 0x32                                             /* Usage type:  DV */
#define USB_HID_USAGE_GD_RX 0x33                                            /* Usage type:  DV */
#define USB_HID_USAGE_GD_RY 0x34                                            /* Usage type:  DV */
#define USB_HID_USAGE_GD_RZ 0x35                                            /* Usage type:  DV */
#define USB_HID_USAGE_GD_SLIDER 0x36                                        /* Usage type:  DV */
#define USB_HID_USAGE_GD_DIAL 0x37                                          /* Usage type:  DV */
#define USB_HID_USAGE_GD_WHEEL 0x38                                         /* Usage type:  DV */
#define USB_HID_USAGE_GD_HAT_SWITCH 0x39                                    /* Usage type:  DV */
#define USB_HID_USAGE_GD_COUNTED_BUFFER 0x3A                                /* Usage type:  CL */
#define USB_HID_USAGE_GD_BYTE_COUNT 0x3B                                    /* Usage type:  DV */
#define USB_HID_USAGE_GD_MOTION_WAKEUP 0x3C                                 /* Usage type:  OSC, DF */
#define USB_HID_USAGE_GD_START 0x3D                                         /* Usage type:  OOC */
#define USB_HID_USAGE_GD_SELECT 0x3E                                        /* Usage type:  OOC */
#define USB_HID_USAGE_GD_VX 0x40                                            /* Usage type:  DV */
#define USB_HID_USAGE_GD_VY 0x41                                            /* Usage type:  DV */
#define USB_HID_USAGE_GD_VZ 0x42                                            /* Usage type:  DV */
#define USB_HID_USAGE_GD_VBRX 0x43                                          /* Usage type:  DV */
#define USB_HID_USAGE_GD_VBRY 0x44                                          /* Usage type:  DV */
#define USB_HID_USAGE_GD_VBRZ 0x45                                          /* Usage type:  DV */
#define USB_HID_USAGE_GD_VNO 0x46                                           /* Usage type:  DV */
#define USB_HID_USAGE_GD_FEATURE_NOTIFICATION 0x47                          /* Usage type:  DV, DF */
#define USB_HID_USAGE_GD_RESOLUTION_MULTIPLIER 0x48                         /* Usage type:  DV */
#define USB_HID_USAGE_GD_QX 0x49                                            /* Usage type:  DV */
#define USB_HID_USAGE_GD_QY 0x4A                                            /* Usage type:  DV */
#define USB_HID_USAGE_GD_QZ 0x4B                                            /* Usage type:  DV */
#define USB_HID_USAGE_GD_QW 0x4C                                            /* Usage type:  DV */
#define USB_HID_USAGE_GD_SYSTEM_CONTROL 0x80                                /* Usage type:  CA */
#define USB_HID_USAGE_GD_SYSTEM_POWER_DOWN 0x81                             /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_SLEEP 0x82                                  /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_WAKE_UP 0x83                                /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_CONTEXT_MENU 0x84                           /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_MAIN_MENU 0x85                              /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_APP_MENU 0x86                               /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_MENU_HELP 0x87                              /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_MENU_EXIT 0x88                              /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_MENU_SELECT 0x89                            /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_MENU_RIGHT 0x8A                             /* Usage type:  RTC */
#define USB_HID_USAGE_GD_SYSTEM_MENU_LEFT 0x8B                              /* Usage type:  RTC */
#define USB_HID_USAGE_GD_SYSTEM_MENU_UP 0x8C                                /* Usage type:  RTC */
#define USB_HID_USAGE_GD_SYSTEM_MENU_DOWN 0x8D                              /* Usage type:  RTC */
#define USB_HID_USAGE_GD_SYSTEM_COLD_RESTART 0x8E                           /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_WARM_RESTART 0x8F                           /* Usage type:  OSC */
#define USB_HID_USAGE_GD_D_PAD_UP 0x90                                      /* Usage type:  OOC */
#define USB_HID_USAGE_GD_D_PAD_DOWN 0x91                                    /* Usage type:  OOC */
#define USB_HID_USAGE_GD_D_PAD_RIGHT 0x92                                   /* Usage type:  OOC */
#define USB_HID_USAGE_GD_D_PAD_LEFT 0x93                                    /* Usage type:  OOC */
#define USB_HID_USAGE_GD_INDEX_TRIGGER 0x94                                 /* Usage type:  MC, DV */
#define USB_HID_USAGE_GD_PALM_TRIGGER 0x95                                  /* Usage type:  MC, DV */
#define USB_HID_USAGE_GD_THUMBSTICK 0x96                                    /* Usage type:  CP */
#define USB_HID_USAGE_GD_SYSTEM_FUNCTION_SHIFT 0x97                         /* Usage type:  MC */
#define USB_HID_USAGE_GD_SYSTEM_FUNCTION_SHIFT_LOCK 0x98                    /* Usage type:  OOC */
#define USB_HID_USAGE_GD_SYSTEM_FUNCTION_SHIFT_LOCK_INDICATOR 0x99          /* Usage type:  DV */
#define USB_HID_USAGE_GD_SYSTEM_DISMISS_NOTIFICATION 0x9A                   /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_DO_NOT_DISTURB 0x9B                         /* Usage type:  OOC */
#define USB_HID_USAGE_GD_SYSTEM_DOCK 0xA0                                   /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_UNDOCK 0xA1                                 /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_SETUP 0xA2                                  /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_BREAK 0xA3                                  /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_DEBUGGER_BREAK 0xA4                         /* Usage type:  OSC */
#define USB_HID_USAGE_GD_APPLICATION_BREAK 0xA5                             /* Usage type:  OSC */
#define USB_HID_USAGE_GD_APPLICATION_DEBUGGER_BREAK 0xA6                    /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_SPEAKER_MUTE 0xA7                           /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_HIBERNATE 0xA8                              /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_DISPLAY_INVERT 0xB0                         /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_DISPLAY_INTERNAL 0xB1                       /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_DISPLAY_EXTERNAL 0xB2                       /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_DISPLAY_BOTH 0xB3                           /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_DISPLAY_DUAL 0xB4                           /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_DISPLAY_TOGGLE_INT_EXT_MODE 0xB5            /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_DISPLAY_SWAP_PRIMARY_SECONDARY 0xB6         /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SYSTEM_DISPLAY_TOGGLE_LCD_AUTOSCALE 0xB7           /* Usage type:  OSC */
#define USB_HID_USAGE_GD_SENSOR_ZONE 0xC0                                   /* Usage type:  CL */
#define USB_HID_USAGE_GD_RPM 0xC1                                           /* Usage type:  DV */
#define USB_HID_USAGE_GD_COOLANT_LEVEL 0xC2                                 /* Usage type:  DV */
#define USB_HID_USAGE_GD_COOLANT_CRITICAL_LEVEL 0xC3                        /* Usage type:  SV */
#define USB_HID_USAGE_GD_COOLANT_PUMP 0xC4                                  /* Usage type:  US */
#define USB_HID_USAGE_GD_CHASSIS_ENCLOSURE 0xC5                             /* Usage type:  CL */
#define USB_HID_USAGE_GD_WIRELESS_RADIO_BUTTON 0xC6                         /* Usage type:  OOC */
#define USB_HID_USAGE_GD_WIRELESS_RADIO_LED 0xC7                            /* Usage type:  OOC */
#define USB_HID_USAGE_GD_WIRELESS_RADIO_SLIDER_SWITCH 0xC8                  /* Usage type:  OOC */
#define USB_HID_USAGE_GD_SYSTEM_DISPLAY_ROTATION_LOCK_BUTTON 0xC9           /* Usage type:  OOC */
#define USB_HID_USAGE_GD_SYSTEM_DISPLAY_ROTATION_LOCK_SLIDER_SWITCH 0xCA    /* Usage type:  OOC */
#define USB_HID_USAGE_GD_CONTROL_ENABLE 0xCB                                /* Usage type:  DF */
#define USB_HID_USAGE_GD_DOCKABLE_DEVICE_UNIQUE_ID 0xD0                     /* Usage type:  DV */
#define USB_HID_USAGE_GD_DOCKABLE_DEVICE_VENDOR_ID 0xD1                     /* Usage type:  DV */
#define USB_HID_USAGE_GD_DOCKABLE_DEVICE_PRIMARY_USAGE_PAGE 0xD2            /* Usage type:  DV */
#define USB_HID_USAGE_GD_DOCKABLE_DEVICE_PRIMARY_USAGE_ID 0xD3              /* Usage type:  DV */
#define USB_HID_USAGE_GD_DOCKABLE_DEVICE_DOCKING_STATE 0xD4                 /* Usage type:  DF */
#define USB_HID_USAGE_GD_DOCKABLE_DEVICE_DISPLAY_OCCLUSION 0xD5             /* Usage type:  CL */
#define USB_HID_USAGE_GD_DOCKABLE_DEVICE_OBJECT_TYPE 0xD6                   /* Usage type:  DV */
/** @} */

/**
 * @name Page 0x02: Simulation Controls
 * @{
 */
#define USB_HID_USAGE_SIM_UNDEFINED 0x00
#define USB_HID_USAGE_SIM_FLIGHT_SIMULATION_DEVICE 0x01         /* Usage type:  CA */
#define USB_HID_USAGE_SIM_AUTOMOBILE_SIMULATION_DEVICE 0x02     /* Usage type:  CA */
#define USB_HID_USAGE_SIM_TANK_SIMULATION_DEVICE 0x03           /* Usage type:  CA */
#define USB_HID_USAGE_SIM_SPACESHIP_SIMULATION_DEVICE 0x04      /* Usage type:  CA */
#define USB_HID_USAGE_SIM_SUBMARINE_SIMULATION_DEVICE 0x05      /* Usage type:  CA */
#define USB_HID_USAGE_SIM_SAILING_SIMULATION_DEVICE 0x06        /* Usage type:  CA */
#define USB_HID_USAGE_SIM_MOTORCYCLE_SIMULATION_DEVICE 0x07     /* Usage type:  CA */
#define USB_HID_USAGE_SIM_SPORTS_SIMULATION_DEVICE 0x08         /* Usage type:  CA */
#define USB_HID_USAGE_SIM_AIRPLANE_SIMULATION_DEVICE 0x09       /* Usage type:  CA */
#define USB_HID_USAGE_SIM_HELICOPTER_SIMULATION_DEVICE 0x0A     /* Usage type:  CA */
#define USB_HID_USAGE_SIM_MAGIC_CARPET_SIMULATION_DEVICE 0x0B   /* Usage type:  CA */
#define USB_HID_USAGE_SIM_BICYCLE_SIMULATION_DEVICE 0x0C        /* Usage type:  CA */
#define USB_HID_USAGE_SIM_FLIGHT_CONTROL_STICK 0x20             /* Usage type:  CA */
#define USB_HID_USAGE_SIM_FLIGHT_STICK 0x21                     /* Usage type:  CA */
#define USB_HID_USAGE_SIM_CYCLIC_CONTROL 0x22                   /* Usage type:  CP */
#define USB_HID_USAGE_SIM_CYCLIC_TRIM 0x23                      /* Usage type:  CP */
#define USB_HID_USAGE_SIM_FLIGHT_YOKE 0x24                      /* Usage type:  CA */
#define USB_HID_USAGE_SIM_TRACK_CONTROL 0x25                    /* Usage type:  CP */
#define USB_HID_USAGE_SIM_AILERON 0xB0                          /* Usage type:  DV */
#define USB_HID_USAGE_SIM_AILERON_TRIM 0xB1                     /* Usage type:  DV */
#define USB_HID_USAGE_SIM_ANTI_TORQUE_CONTROL 0xB2              /* Usage type:  DV */
#define USB_HID_USAGE_SIM_AUTOPILOT_ENABLE 0xB3                 /* Usage type:  OOC */
#define USB_HID_USAGE_SIM_CHAFF_RELEASE 0xB4                    /* Usage type:  OSC */
#define USB_HID_USAGE_SIM_COLLECTIVE_CONTROL 0xB5               /* Usage type:  DV */
#define USB_HID_USAGE_SIM_DIVE_BRAKE 0xB6                       /* Usage type:  DV */
#define USB_HID_USAGE_SIM_ELECTRONIC_COUNTERMEASURES 0xB7       /* Usage type:  OOC */
#define USB_HID_USAGE_SIM_ELEVATOR 0xB8                         /* Usage type:  DV */
#define USB_HID_USAGE_SIM_ELEVATOR_TRIM 0xB9                    /* Usage type:  DV */
#define USB_HID_USAGE_SIM_RUDDER 0xBA                           /* Usage type:  DV */
#define USB_HID_USAGE_SIM_THROTTLE 0xBB                         /* Usage type:  DV */
#define USB_HID_USAGE_SIM_FLIGHT_COMMUNICATIONS 0xBC            /* Usage type:  OOC */
#define USB_HID_USAGE_SIM_FLARE_RELEASE 0xBD                    /* Usage type:  OSC */
#define USB_HID_USAGE_SIM_LANDING_GEAR 0xBE                     /* Usage type:  OOC */
#define USB_HID_USAGE_SIM_TOE_BRAKE 0xBF                        /* Usage type:  DV */
#define USB_HID_USAGE_SIM_TRIGGER 0xC0                          /* Usage type:  MC */
#define USB_HID_USAGE_SIM_WEAPONS_ARM 0xC1                      /* Usage type:  OOC */
#define USB_HID_USAGE_SIM_WEAPONS_SELECT 0xC2                   /* Usage type:  OSC */
#define USB_HID_USAGE_SIM_WING_FLAPS 0xC3                       /* Usage type:  DV */
#define USB_HID_USAGE_SIM_ACCELERATOR 0xC4                      /* Usage type:  DV */
#define USB_HID_USAGE_SIM_BRAKE 0xC5                            /* Usage type:  DV */
#define USB_HID_USAGE_SIM_CLUTCH 0xC6                           /* Usage type:  DV */
#define USB_HID_USAGE_SIM_SHIFTER 0xC7                          /* Usage type:  DV */
#define USB_HID_USAGE_SIM_STEERING 0xC8                         /* Usage type:  DV */
#define USB_HID_USAGE_SIM_TURRET_DIRECTION 0xC9                 /* Usage type:  DV */
#define USB_HID_USAGE_SIM_BARREL_ELEVATION 0xCA                 /* Usage type:  DV */
#define USB_HID_USAGE_SIM_DIVE_PLANE 0xCB                       /* Usage type:  DV */
#define USB_HID_USAGE_SIM_BALLAST 0xCC                          /* Usage type:  DV */
#define USB_HID_USAGE_SIM_BICYCLE_CRANK 0xCD                    /* Usage type:  DV */
#define USB_HID_USAGE_SIM_HANDLE_BARS 0xCE                      /* Usage type:  DV */
#define USB_HID_USAGE_SIM_FRONT_BRAKE 0xCF                      /* Usage type:  DV */
#define USB_HID_USAGE_SIM_REAR_BRAKE 0xD0                       /* Usage type:  DV */
/** @} */

/**
 * @name Page 0x03: VR Controls
 * @{
 */
#define USB_HID_USAGE_VR_UNDEFINED 0x00
#define USB_HID_USAGE_VR_BELT 0x01                      /* Usage type:  CA */
#define USB_HID_USAGE_VR_BODY_SUIT 0x02                 /* Usage type:  CA */
#define USB_HID_USAGE_VR_FLEXOR 0x03                    /* Usage type:  CP */
#define USB_HID_USAGE_VR_GLOVE 0x04                     /* Usage type:  CA */
#define USB_HID_USAGE_VR_HEAD_TRACKER 0x05              /* Usage type:  CP */
#define USB_HID_USAGE_VR_HEAD_MOUNTED_DISPLAY 0x06      /* Usage type:  CA */
#define USB_HID_USAGE_VR_HAND_TRACKER 0x07              /* Usage type:  CA */
#define USB_HID_USAGE_VR_OCULOMETER 0x08                /* Usage type:  CA */
#define USB_HID_USAGE_VR_VEST 0x09                      /* Usage type:  CA */
#define USB_HID_USAGE_VR_ANIMATRONIC_DEVICE 0x0A        /* Usage type:  CA */
#define USB_HID_USAGE_VR_STEREO_ENABLE 0x20             /* Usage type:  OOC */
#define USB_HID_USAGE_VR_DISPLAY_ENABLE 0x21            /* Usage type:  OOC */
/** @} */

/**
 * @name Page 0x04: Sport Controls
 * @{
 */
#define USB_HID_USAGE_SPORT_UNDEFINED 0x00
#define USB_HID_USAGE_SPORT_BASEBALL_BAT 0x01           /* Usage type:  CA */
#define USB_HID_USAGE_SPORT_GOLF_CLUB 0x02              /* Usage type:  CA */
#define USB_HID_USAGE_SPORT_ROWING_MACHINE 0x03         /* Usage type:  CA */
#define USB_HID_USAGE_SPORT_TREADMILL 0x04              /* Usage type:  CA */
#define USB_HID_USAGE_SPORT_OAR 0x30                    /* Usage type:  DV */
#define USB_HID_USAGE_SPORT_SLOPE 0x31                  /* Usage type:  DV */
#define USB_HID_USAGE_SPORT_RATE 0x32                   /* Usage type:  DV */
#define USB_HID_USAGE_SPORT_STICK_SPEED 0x33            /* Usage type:  DV */
#define USB_HID_USAGE_SPORT_STICK_FACE_ANGLE 0x34       /* Usage type:  DV */
#define USB_HID_USAGE_SPORT_STICK_HEEL_TOE 0x35         /* Usage type:  DV */
#define USB_HID_USAGE_SPORT_STICK_FOLLOW_THROUGH 0x36   /* Usage type:  DV */
#define USB_HID_USAGE_SPORT_STICK_TEMPO 0x37            /* Usage type:  DV */
#define USB_HID_USAGE_SPORT_STICK_TYPE 0x38             /* Usage type:  NAry */
#define USB_HID_USAGE_SPORT_STICK_HEIGHT 0x39           /* Usage type:  DV */
#define USB_HID_USAGE_SPORT_PUTTER 0x50                 /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_1_IRON 0x51                 /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_2_IRON 0x52                 /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_3_IRON 0x53                 /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_4_IRON 0x54                 /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_5_IRON 0x55                 /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_6_IRON 0x56                 /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_7_IRON 0x57                 /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_8_IRON 0x58                 /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_9_IRON 0x59                 /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_10_IRON 0x5A                /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_11_IRON 0x5B                /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_SAND_WEDGE 0x5C             /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_LOFT_WEDGE 0x5D             /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_POWER_WEDGE 0x5E            /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_1_WOOD 0x5F                 /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_3_WOOD 0x60                 /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_5_WOOD 0x61                 /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_7_WOOD 0x62                 /* Usage type:  Sel */
#define USB_HID_USAGE_SPORT_9_WOOD 0x63                 /* Usage type:  Sel */
/** @} */

/**
 * @name Page 0x05: Game Controls
 * @{
 */
#define USB_HID_USAGE_GAME_UNDEFINED 0x00
#define USB_HID_USAGE_GAME_3D_GAME_CONTROLLER 0x01          /* Usage type:  CA */
#define USB_HID_USAGE_GAME_PINBALL_DEVICE 0x02              /* Usage type:  CA */
#define USB_HID_USAGE_GAME_GUN_DEVICE 0x03                  /* Usage type:  CA */
#define USB_HID_USAGE_GAME_POINT_OF_VIEW 0x20               /* Usage type:  CP */
#define USB_HID_USAGE_GAME_TURN_RIGHT_LEFT 0x21             /* Usage type:  DV */
#define USB_HID_USAGE_GAME_PITCH_FORWARD_BACKWARD 0x22      /* Usage type:  DV */
#define USB_HID_USAGE_GAME_ROLL_RIGHT_LEFT 0x23             /* Usage type:  DV */
#define USB_HID_USAGE_GAME_MOVE_RIGHT_LEFT 0x24             /* Usage type:  DV */
#define USB_HID_USAGE_GAME_MOVE_FORWARD_BACKWARD 0x25       /* Usage type:  DV */
#define USB_HID_USAGE_GAME_MOVE_UP_DOWN 0x26                /* Usage type:  DV */
#define USB_HID_USAGE_GAME_LEAN_RIGHT_LEFT 0x27             /* Usage type:  DV */
#define USB_HID_USAGE_GAME_LEAN_FORWARD_BACKWARD 0x28       /* Usage type:  DV */
#define USB_HID_USAGE_GAME_HEIGHT_OF_POV 0x29               /* Usage type:  DV */
#define USB_HID_USAGE_GAME_FLIPPER 0x2A                     /* Usage type:  MC */
#define USB_HID_USAGE_GAME_SECONDARY_FLIPPER 0x2B           /* Usage type:  MC */
#define USB_HID_USAGE_GAME_BUMP 0x2C                        /* Usage type:  MC */
#define USB_HID_USAGE_GAME_NEW_GAME 0x2D                    /* Usage type:  OSC */
#define USB_HID_USAGE_GAME_SHOOT_BALL 0x2E                  /* Usage type:  OSC */
#define USB_HID_USAGE_GAME_PLAYER 0x2F                      /* Usage type:  OSC */
#define USB_HID_USAGE_GAME_GUN_BOLT 0x30                    /* Usage type:  OOC */
#define USB_HID_USAGE_GAME_GUN_CLIP 0x31                    /* Usage type:  OOC */
#define USB_HID_USAGE_GAME_GUN_SELECTOR 0x32                /* Usage type:  NAry */
#define USB_HID_USAGE_GAME_GUN_SINGLE_SHOT 0x33             /* Usage type:  Sel */
#define USB_HID_USAGE_GAME_GUN_BURST 0x34                   /* Usage type:  Sel */
#define USB_HID_USAGE_GAME_GUN_AUTOMATIC 0x35               /* Usage type:  Sel */
#define USB_HID_USAGE_GAME_GUN_SAFETY 0x36                  /* Usage type:  OOC */
#define USB_HID_USAGE_GAME_GAMEPAD_FIRE_JUMP 0x37           /* Usage type:  CL */
#define USB_HID_USAGE_GAME_GAMEPAD_TRIGGER 0x39             /* Usage type:  CL */
#define USB_HID_USAGE_GAME_FORM_FITTING_GAMEPAD 0x3A        /* Usage type:  SF */
/** @} */

/**
 * @name Page 0x06: Generic Device Controls
 * @{
 */
#define USB_HID_USAGE_GDV_UNDEFINED 0x00
#define USB_HID_USAGE_GDV_BACKGROUND_NONUSER_CONTROLS 0x01          /* Usage type:  CA */
#define USB_HID_USAGE_GDV_BATTERY_STRENGTH 0x20                     /* Usage type:  DV */
#define USB_HID_USAGE_GDV_WIRELESS_CHANNEL 0x21                     /* Usage type:  DV */
#define USB_HID_USAGE_GDV_WIRELESS_ID 0x22                          /* Usage type:  DV */
#define USB_HID_USAGE_GDV_DISCOVER_WIRELESS_CONTROL 0x23            /* Usage type:  OSC */
#define USB_HID_USAGE_GDV_SECURITY_CODE_CHARACTER_ENTERED 0x24      /* Usage type:  OSC */
#define USB_HID_USAGE_GDV_SECURITY_CODE_CHARACTER_ERASED 0x25       /* Usage type:  OSC */
#define USB_HID_USAGE_GDV_SECURITY_CODE_CLEARED 0x26                /* Usage type:  OSC */
#define USB_HID_USAGE_GDV_SEQUENCE_ID 0x27                          /* Usage type:  DV */
#define USB_HID_USAGE_GDV_SEQUENCE_ID_RESET 0x28                    /* Usage type:  DF */
#define USB_HID_USAGE_GDV_RF_SIGNAL_STRENGTH 0x29                   /* Usage type:  DV */
#define USB_HID_USAGE_GDV_SOFTWARE_VERSION 0x2A                     /* Usage type:  CL */
#define USB_HID_USAGE_GDV_PROTOCOL_VERSION 0x2B                     /* Usage type:  CL */
#define USB_HID_USAGE_GDV_HARDWARE_VERSION 0x2C                     /* Usage type:  CL */
#define USB_HID_USAGE_GDV_MAJOR 0x2D                                /* Usage type:  SV */
#define USB_HID_USAGE_GDV_MINOR 0x2E                                /* Usage type:  SV */
#define USB_HID_USAGE_GDV_REVISION 0x2F                             /* Usage type:  SV */
#define USB_HID_USAGE_GDV_HANDEDNESS 0x30                           /* Usage type:  NAry */
#define USB_HID_USAGE_GDV_EITHER_HAND 0x31                          /* Usage type:  Sel */
#define USB_HID_USAGE_GDV_LEFT_HAND 0x32                            /* Usage type:  Sel */
#define USB_HID_USAGE_GDV_RIGHT_HAND 0x33                           /* Usage type:  Sel */
#define USB_HID_USAGE_GDV_BOTH_HANDS 0x34                           /* Usage type:  Sel */
#define USB_HID_USAGE_GDV_GRIP_POSE_OFFSET 0x40                     /* Usage type:  CP */
#define USB_HID_USAGE_GDV_POINTER_POSE_OFFSET 0x41                  /* Usage type:  CP */
/** @} */

/**
 * @name Page 0x07: Keyboard/Keypad
 * @{
 */
#define USB_HID_USAGE_KEY_KEYBOARD_ERRORROLLOVER 0x01                   /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_POSTFAIL 0x02                        /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_ERRORUNDEFINED 0x03                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_A 0x04                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_B 0x05                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_C 0x06                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_D 0x07                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_E 0x08                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F 0x09                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_G 0x0A                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_H 0x0B                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_I 0x0C                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_J 0x0D                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_K 0x0E                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_L 0x0F                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_M 0x10                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_N 0x11                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_O 0x12                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_P 0x13                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_Q 0x14                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_R 0x15                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_S 0x16                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_T 0x17                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_U 0x18                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_V 0x19                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_W 0x1A                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_X 0x1B                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_Y 0x1C                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_Z 0x1D                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_1_AND_EXCLAMATION 0x1E               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_2_AND_AT 0x1F                        /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_3_AND_HASH 0x20                      /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_4_AND_DOLLAR 0x21                    /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_5_AND_PERCENT 0x22                   /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_6_AND_CARET 0x23                     /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_7_AND_AMPERSAND 0x24                 /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_8_AND_ASTERISK 0x25                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_9_AND_LEFT_PARENTHESIS 0x26          /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_0_AND_RIGHT_PARENTHESIS 0x27         /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_RETURN_ENTER 0x28                    /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_ESCAPE 0x29                          /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE 0x2A                /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_TAB 0x2B                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_SPACEBAR 0x2C                        /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_MINUS_AND_UNDERSCORE 0x2D            /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_EQUAL_AND_PLUS 0x2E                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_LEFT_BRACKET_AND_LEFT_BRACE 0x2F     /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_RIGHT_BRACKET_AND_RIGHT_BRACE 0x30   /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_BACKSLASH_AND_PIPE 0x31              /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_NON_US_HASH_AND_TILDE 0x32           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_SEMICOLON_AND_COLON 0x33             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_APOSTROPHE_AND_QUOTE 0x34            /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_GRAVE_ACCENT_AND_TILDE 0x35          /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_COMMA_AND_LESS_THAN 0x36             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_PERIOD_AND_GREATER_THAN 0x37         /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_SLASH_AND_QUESTION_MARK 0x38         /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_CAPS_LOCK 0x39                       /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F1 0x3A                              /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F2 0x3B                              /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F3 0x3C                              /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F4 0x3D                              /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F5 0x3E                              /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F6 0x3F                              /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F7 0x40                              /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F8 0x41                              /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F9 0x42                              /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F10 0x43                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F11 0x44                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F12 0x45                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_PRINTSCREEN 0x46                     /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_SCROLL_LOCK 0x47                     /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_PAUSE 0x48                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_INSERT 0x49                          /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_HOME 0x4A                            /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_PAGEUP 0x4B                          /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_DELETE_FORWARD 0x4C                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_END 0x4D                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_PAGEDOWN 0x4E                        /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_RIGHTARROW 0x4F                      /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_LEFTARROW 0x50                       /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_DOWNARROW 0x51                       /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_UPARROW 0x52                         /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_NUM_LOCK_AND_CLEAR 0x53                /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_SLASH 0x54                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_ASTERISK 0x55                          /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_MINUS 0x56                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_PLUS 0x57                              /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_ENTER 0x58                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_1_AND_END 0x59                         /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_2_AND_DOWN_ARROW 0x5A                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_3_AND_PAGEDN 0x5B                      /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_4_AND_LEFT_ARROW 0x5C                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_5 0x5D                                 /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_6_AND_RIGHT_ARROW 0x5E                 /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_7_AND_HOME 0x5F                        /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_8_AND_UP_ARROW 0x60                    /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_9_AND_PAGEUP 0x61                      /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_0_AND_INSERT 0x62                      /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_PERIOD_AND_DELETE 0x63                 /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_NON_US_BACKSLASH_AND_PIPE 0x64       /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_APPLICATION 0x65                     /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_POWER 0x66                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_EQUAL 0x67                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F13 0x68                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F14 0x69                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F15 0x6A                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F16 0x6B                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F17 0x6C                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F18 0x6D                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F19 0x6E                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F20 0x6F                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F21 0x70                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F22 0x71                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F23 0x72                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_F24 0x73                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_EXECUTE 0x74                         /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_HELP 0x75                            /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_MENU 0x76                            /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_SELECT 0x77                          /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_STOP 0x78                            /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_AGAIN 0x79                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_UNDO 0x7A                            /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_CUT 0x7B                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_COPY 0x7C                            /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_PASTE 0x7D                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_FIND 0x7E                            /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_MUTE 0x7F                            /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_VOLUME_UP 0x80                       /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_VOLUME_DOWN 0x81                     /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_LOCKING_CAPS_LOCK 0x82               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_LOCKING_NUM_LOCK 0x83                /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_LOCKING_SCROLL_LOCK 0x84             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_COMMA 0x85                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_EQUAL_SIGN 0x86                        /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_INTERNATIONAL1 0x87                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_INTERNATIONAL2 0x88                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_INTERNATIONAL3 0x89                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_INTERNATIONAL4 0x8A                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_INTERNATIONAL5 0x8B                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_INTERNATIONAL6 0x8C                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_INTERNATIONAL7 0x8D                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_INTERNATIONAL8 0x8E                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_INTERNATIONAL9 0x8F                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_LANG1 0x90                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_LANG2 0x91                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_LANG3 0x92                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_LANG4 0x93                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_LANG5 0x94                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_LANG6 0x95                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_LANG7 0x96                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_LANG8 0x97                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_LANG9 0x98                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_ALTERNATE_ERASE 0x99                 /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_SYSREQ_ATTENTION 0x9A                /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_CANCEL 0x9B                          /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_CLEAR 0x9C                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_PRIOR 0x9D                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_RETURN 0x9E                          /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_SEPARATOR 0x9F                       /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_OUT 0xA0                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_OPER 0xA1                            /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_CLEAR_AGAIN 0xA2                     /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_CRSEL_PROPS 0xA3                     /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_EXSEL 0xA4                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_00 0xB0                                /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_000 0xB1                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_THOUSANDS_SEPARATOR 0xB2                      /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_DECIMAL_SEPARATOR 0xB3                        /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_CURRENCY_UNIT 0xB4                            /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_CURRENCY_SUB_UNIT 0xB5                        /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_LEFT_PARENTHESIS 0xB6                  /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_RIGHT_PARENTHESIS 0xB7                 /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_LEFT_BRACE 0xB8                        /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_RIGHT_BRACE 0xB9                       /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_TAB 0xBA                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_BACKSPACE 0xBB                         /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_A 0xBC                                 /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_B 0xBD                                 /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_C 0xBE                                 /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_D 0xBF                                 /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_E 0xC0                                 /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_F 0xC1                                 /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_XOR 0xC2                               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_CARET 0xC3                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_PERCENT 0xC4                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_LESS_THAN 0xC5                         /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_GREATER_THAN 0xC6                      /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_AMPERSAND 0xC7                         /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_AMPERSAND_AMPERSAND 0xC8               /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_PIPE 0xC9                              /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_PIPE_PIPE 0xCA                         /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_COLON 0xCB                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_HASH 0xCC                              /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_SPACE 0xCD                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_AT 0xCE                                /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_EXCLAMATION 0xCF                       /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_MEMORY_STORE 0xD0                      /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_MEMORY_RECALL 0xD1                     /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_MEMORY_CLEAR 0xD2                      /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_MEMORY_ADD 0xD3                        /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_MEMORY_SUBTRACT 0xD4                   /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_MEMORY_MULTIPLY 0xD5                   /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_MEMORY_DIVIDE 0xD6                     /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_PLUS_MINUS 0xD7                        /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_CLEAR 0xD8                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_CLEAR_ENTRY 0xD9                       /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_BINARY 0xDA                            /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_OCTAL 0xDB                             /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_DECIMAL 0xDC                           /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYPAD_HEXADECIMAL 0xDD                       /* Usage type:  Sel */
#define USB_HID_USAGE_KEY_KEYBOARD_LEFTCONTROL 0xE0                     /* Usage type:  DV */
#define USB_HID_USAGE_KEY_KEYBOARD_LEFTSHIFT 0xE1                       /* Usage type:  DV */
#define USB_HID_USAGE_KEY_KEYBOARD_LEFTALT 0xE2                         /* Usage type:  DV */
#define USB_HID_USAGE_KEY_KEYBOARD_LEFT_GUI 0xE3                        /* Usage type:  DV */
#define USB_HID_USAGE_KEY_KEYBOARD_RIGHTCONTROL 0xE4                    /* Usage type:  DV */
#define USB_HID_USAGE_KEY_KEYBOARD_RIGHTSHIFT 0xE5                      /* Usage type:  DV */
#define USB_HID_USAGE_KEY_KEYBOARD_RIGHTALT 0xE6                        /* Usage type:  DV */
#define USB_HID_USAGE_KEY_KEYBOARD_RIGHT_GUI 0xE7                       /* Usage type:  DV */
/** @} */

/**
 * @name Page 0x08: LED
 * @{
 */
#define USB_HID_USAGE_LED_UNDEFINED 0x00
#define USB_HID_USAGE_LED_NUM_LOCK 0x01                     /* Usage type:  OOC */
#define USB_HID_USAGE_LED_CAPS_LOCK 0x02                    /* Usage type:  OOC */
#define USB_HID_USAGE_LED_SCROLL_LOCK 0x03                  /* Usage type:  OOC */
#define USB_HID_USAGE_LED_COMPOSE 0x04                      /* Usage type:  OOC */
#define USB_HID_USAGE_LED_KANA 0x05                         /* Usage type:  OOC */
#define USB_HID_USAGE_LED_POWER 0x06                        /* Usage type:  OOC */
#define USB_HID_USAGE_LED_SHIFT 0x07                        /* Usage type:  OOC */
#define USB_HID_USAGE_LED_DO_NOT_DISTURB 0x08               /* Usage type:  OOC */
#define USB_HID_USAGE_LED_MUTE 0x09                         /* Usage type:  OOC */
#define USB_HID_USAGE_LED_TONE_ENABLE 0x0A                  /* Usage type:  OOC */
#define USB_HID_USAGE_LED_HIGH_CUT_FILTER 0x0B              /* Usage type:  OOC */
#define USB_HID_USAGE_LED_LOW_CUT_FILTER 0x0C               /* Usage type:  OOC */
#define USB_HID_USAGE_LED_EQUALIZER_ENABLE 0x0D             /* Usage type:  OOC */
#define USB_HID_USAGE_LED_SOUND_FIELD_ON 0x0E               /* Usage type:  OOC */
#define USB_HID_USAGE_LED_SURROUND_ON 0x0F                  /* Usage type:  OOC */
#define USB_HID_USAGE_LED_REPEAT 0x10                       /* Usage type:  OOC */
#define USB_HID_USAGE_LED_STEREO 0x11                       /* Usage type:  OOC */
#define USB_HID_USAGE_LED_SAMPLING_RATE_DETECT 0x12         /* Usage type:  OOC */
#define USB_HID_USAGE_LED_SPINNING 0x13                     /* Usage type:  OOC */
#define USB_HID_USAGE_LED_CAV 0x14                          /* Usage type:  OOC */
#define USB_HID_USAGE_LED_CLV 0x15                          /* Usage type:  OOC */
#define USB_HID_USAGE_LED_RECORDING_FORMAT_DETECT 0x16      /* Usage type:  OOC */
#define USB_HID_USAGE_LED_OFF_HOOK 0x17                     /* Usage type:  OOC */
#define USB_HID_USAGE_LED_RING 0x18                         /* Usage type:  OOC */
#define USB_HID_USAGE_LED_MESSAGE_WAITING 0x19              /* Usage type:  OOC */
#define USB_HID_USAGE_LED_DATA_MODE 0x1A                    /* Usage type:  OOC */
#define USB_HID_USAGE_LED_BATTERY_OPERATION 0x1B            /* Usage type:  OOC */
#define USB_HID_USAGE_LED_BATTERY_OK 0x1C                   /* Usage type:  OOC */
#define USB_HID_USAGE_LED_BATTERY_LOW 0x1D                  /* Usage type:  OOC */
#define USB_HID_USAGE_LED_SPEAKER 0x1E                      /* Usage type:  OOC */
#define USB_HID_USAGE_LED_HEAD_SET 0x1F                     /* Usage type:  OOC */
#define USB_HID_USAGE_LED_HOLD 0x20                         /* Usage type:  OOC */
#define USB_HID_USAGE_LED_MICROPHONE 0x21                   /* Usage type:  OOC */
#define USB_HID_USAGE_LED_COVERAGE 0x22                     /* Usage type:  OOC */
#define USB_HID_USAGE_LED_NIGHT_MODE 0x23                   /* Usage type:  OOC */
#define USB_HID_USAGE_LED_SEND_CALLS 0x24                   /* Usage type:  OOC */
#define USB_HID_USAGE_LED_CALL_PICKUP 0x25                  /* Usage type:  OOC */
#define USB_HID_USAGE_LED_CONFERENCE 0x26                   /* Usage type:  OOC */
#define USB_HID_USAGE_LED_STAND_BY 0x27                     /* Usage type:  OOC */
#define USB_HID_USAGE_LED_CAMERA_ON 0x28                    /* Usage type:  OOC */
#define USB_HID_USAGE_LED_CAMERA_OFF 0x29                   /* Usage type:  OOC */
#define USB_HID_USAGE_LED_ON_LINE 0x2A                      /* Usage type:  OOC */
#define USB_HID_USAGE_LED_OFF_LINE 0x2B                     /* Usage type:  OOC */
#define USB_HID_USAGE_LED_BUSY 0x2C                         /* Usage type:  OOC */
#define USB_HID_USAGE_LED_READY 0x2D                        /* Usage type:  OOC */
#define USB_HID_USAGE_LED_PAPER_OUT 0x2E                    /* Usage type:  OOC */
#define USB_HID_USAGE_LED_PAPER_JAM 0x2F                    /* Usage type:  OOC */
#define USB_HID_USAGE_LED_REMOTE 0x30                       /* Usage type:  OOC */
#define USB_HID_USAGE_LED_FORWARD 0x31                      /* Usage type:  OOC */
#define USB_HID_USAGE_LED_REVERSE 0x32                      /* Usage type:  OOC */
#define USB_HID_USAGE_LED_STOP 0x33                         /* Usage type:  OOC */
#define USB_HID_USAGE_LED_REWIND 0x34                       /* Usage type:  OOC */
#define USB_HID_USAGE_LED_FAST_FORWARD 0x35                 /* Usage type:  OOC */
#define USB_HID_USAGE_LED_PLAY 0x36                         /* Usage type:  OOC */
#define USB_HID_USAGE_LED_PAUSE 0x37                        /* Usage type:  OOC */
#define USB_HID_USAGE_LED_RECORD 0x38                       /* Usage type:  OOC */
#define USB_HID_USAGE_LED_ERROR 0x39                        /* Usage type:  OOC */
#define USB_HID_USAGE_LED_USAGE_SELECTED_INDICATOR 0x3A     /* Usage type:  US */
#define USB_HID_USAGE_LED_USAGE_IN_USE_INDICATOR 0x3B       /* Usage type:  US */
#define USB_HID_USAGE_LED_USAGE_MULTI_MODE_INDICATOR 0x3C   /* Usage type:  UM */
#define USB_HID_USAGE_LED_INDICATOR_ON 0x3D                 /* Usage type:  Sel */
#define USB_HID_USAGE_LED_INDICATOR_FLASH 0x3E              /* Usage type:  Sel */
#define USB_HID_USAGE_LED_INDICATOR_SLOW_BLINK 0x3F         /* Usage type:  Sel */
#define USB_HID_USAGE_LED_INDICATOR_FAST_BLINK 0x40         /* Usage type:  Sel */
#define USB_HID_USAGE_LED_INDICATOR_OFF 0x41                /* Usage type:  Sel */
#define USB_HID_USAGE_LED_FLASH_ON_TIME 0x42                /* Usage type:  DV */
#define USB_HID_USAGE_LED_SLOW_BLINK_ON_TIME 0x43           /* Usage type:  DV */
#define USB_HID_USAGE_LED_SLOW_BLINK_OFF_TIME 0x44          /* Usage type:  DV */
#define USB_HID_USAGE_LED_FAST_BLINK_ON_TIME 0x45           /* Usage type:  DV */
#define USB_HID_USAGE_LED_FAST_BLINK_OFF_TIME 0x46          /* Usage type:  DV */
#define USB_HID_USAGE_LED_USAGE_INDICATOR_COLOR 0x47        /* Usage type:  UM */
#define USB_HID_USAGE_LED_INDICATOR_RED 0x48                /* Usage type:  Sel */
#define USB_HID_USAGE_LED_INDICATOR_GREEN 0x49              /* Usage type:  Sel */
#define USB_HID_USAGE_LED_INDICATOR_AMBER 0x4A              /* Usage type:  Sel */
#define USB_HID_USAGE_LED_GENERIC_INDICATOR 0x4B            /* Usage type:  OOC */
#define USB_HID_USAGE_LED_SYSTEM_SUSPEND 0x4C               /* Usage type:  OOC */
#define USB_HID_USAGE_LED_EXTERNAL_POWER_CONNECTED 0x4D     /* Usage type:  OOC */
#define USB_HID_USAGE_LED_INDICATOR_BLUE 0x4E               /* Usage type:  Sel */
#define USB_HID_USAGE_LED_INDICATOR_ORANGE 0x4F             /* Usage type:  Sel */
#define USB_HID_USAGE_LED_GOOD_STATUS 0x50                  /* Usage type:  OOC */
#define USB_HID_USAGE_LED_WARNING_STATUS 0x51               /* Usage type:  OOC */
#define USB_HID_USAGE_LED_RGB_LED 0x52                      /* Usage type:  CL */
#define USB_HID_USAGE_LED_RED_LED_CHANNEL 0x53              /* Usage type:  DV */
#define USB_HID_USAGE_LED_BLUE_LED_CHANNEL 0x54             /* Usage type:  DV */
#define USB_HID_USAGE_LED_GREEN_LED_CHANNEL 0x55            /* Usage type:  DV */
#define USB_HID_USAGE_LED_LED_INTENSITY 0x56                /* Usage type:  DV */
#define USB_HID_USAGE_LED_PLAYER_INDICATOR 0x60             /* Usage type:  NAry */
#define USB_HID_USAGE_LED_PLAYER_1 0x61                     /* Usage type:  Sel */
#define USB_HID_USAGE_LED_PLAYER_2 0x62                     /* Usage type:  Sel */
#define USB_HID_USAGE_LED_PLAYER_3 0x63                     /* Usage type:  Sel */
#define USB_HID_USAGE_LED_PLAYER_4 0x64                     /* Usage type:  Sel */
#define USB_HID_USAGE_LED_PLAYER_5 0x65                     /* Usage type:  Sel */
#define USB_HID_USAGE_LED_PLAYER_6 0x66                     /* Usage type:  Sel */
#define USB_HID_USAGE_LED_PLAYER_7 0x67                     /* Usage type:  Sel */
#define USB_HID_USAGE_LED_PLAYER_8 0x68                     /* Usage type:  Sel */
/** @} */

/**
 * @name Page 0x0B: Telephony Device
 * @{
 */
#define USB_HID_USAGE_TELEPHONY_UNDEFINED 0x00
#define USB_HID_USAGE_TELEPHONY_PHONE 0x01                          /* Usage type:  CA */
#define USB_HID_USAGE_TELEPHONY_ANSWERING_MACHINE 0x02              /* Usage type:  CA */
#define USB_HID_USAGE_TELEPHONY_MESSAGE_CONTROLS 0x03               /* Usage type:  CL */
#define USB_HID_USAGE_TELEPHONY_HANDSET 0x04                        /* Usage type:  CL */
#define USB_HID_USAGE_TELEPHONY_HEADSET 0x05                        /* Usage type:  CL */
#define USB_HID_USAGE_TELEPHONY_TELEPHONY_KEY_PAD 0x06              /* Usage type:  NAry */
#define USB_HID_USAGE_TELEPHONY_PROGRAMMABLE_BUTTON 0x07            /* Usage type:  NAry */
#define USB_HID_USAGE_TELEPHONY_HOOK_SWITCH 0x20                    /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_FLASH 0x21                          /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_FEATURE 0x22                        /* Usage type:  OSC */
#define USB_HID_USAGE_TELEPHONY_HOLD 0x23                           /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_REDIAL 0x24                         /* Usage type:  OSC */
#define USB_HID_USAGE_TELEPHONY_TRANSFER 0x25                       /* Usage type:  OSC */
#define USB_HID_USAGE_TELEPHONY_DROP 0x26                           /* Usage type:  OSC */
#define USB_HID_USAGE_TELEPHONY_PARK 0x27                           /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_FORWARD_CALLS 0x28                  /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_ALTERNATE_FUNCTION 0x29             /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_LINE 0x2A                           /* Usage type:  OSC, NAry */
#define USB_HID_USAGE_TELEPHONY_SPEAKER_PHONE 0x2B                  /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_CONFERENCE 0x2C                     /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_RING_ENABLE 0x2D                    /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_RING_SELECT 0x2E                    /* Usage type:  OSC */
#define USB_HID_USAGE_TELEPHONY_PHONE_MUTE 0x2F                     /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_CALLER_ID 0x30                      /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_SEND 0x31                           /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_SPEED_DIAL 0x50                     /* Usage type:  OSC */
#define USB_HID_USAGE_TELEPHONY_STORE_NUMBER 0x51                   /* Usage type:  OSC */
#define USB_HID_USAGE_TELEPHONY_RECALL_NUMBER 0x52                  /* Usage type:  OSC */
#define USB_HID_USAGE_TELEPHONY_PHONE_DIRECTORY 0x53                /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_VOICE_MAIL 0x70                     /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_SCREEN_CALLS 0x71                   /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_DO_NOT_DISTURB 0x72                 /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_MESSAGE 0x73                        /* Usage type:  OSC */
#define USB_HID_USAGE_TELEPHONY_ANSWER_ON_OFF 0x74                  /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_INSIDE_DIAL_TONE 0x90               /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_OUTSIDE_DIAL_TONE 0x91              /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_INSIDE_RING_TONE 0x92               /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_OUTSIDE_RING_TONE 0x93              /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_PRIORITY_RING_TONE 0x94             /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_INSIDE_RINGBACK 0x95                /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_PRIORITY_RINGBACK 0x96              /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_LINE_BUSY_TONE 0x97                 /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_REORDER_TONE 0x98                   /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_CALL_WAITING_TONE 0x99              /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_CONFIRMATION_TONE_1 0x9A            /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_CONFIRMATION_TONE_2 0x9B            /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_TONES_OFF 0x9C                      /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_OUTSIDE_RINGBACK 0x9D               /* Usage type:  MC */
#define USB_HID_USAGE_TELEPHONY_RINGER 0x9E                         /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_0 0xB0                    /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_1 0xB1                    /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_2 0xB2                    /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_3 0xB3                    /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_4 0xB4                    /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_5 0xB5                    /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_6 0xB6                    /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_7 0xB7                    /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_8 0xB8                    /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_9 0xB9                    /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_STAR 0xBA                 /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_POUND 0xBB                /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_A 0xBC                    /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_B 0xBD                    /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_C 0xBE                    /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_KEY_D 0xBF                    /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_CALL_HISTORY_KEY 0xC0         /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_CALLER_ID_KEY 0xC1            /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PHONE_SETTINGS_KEY 0xC2             /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_HOST_CONTROL 0xF0                   /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_HOST_AVAILABLE 0xF1                 /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_HOST_CALL_ACTIVE 0xF2               /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_ACTIVATE_HANDSET_AUDIO 0xF3         /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_RING_TYPE 0xF4                      /* Usage type:  NAry */
#define USB_HID_USAGE_TELEPHONY_RE_DIALABLE_PHONE_NUMBER 0xF5       /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_STOP_RING_TONE 0xF8                 /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_PSTN_RING_TONE 0xF9                 /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_HOST_RING_TONE 0xFA                 /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_ALERT_SOUND_ERROR 0xFB              /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_ALERT_SOUND_CONFIRM 0xFC            /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_ALERT_SOUND_NOTIFICATION 0xFD       /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_SILENT_RING 0xFE                    /* Usage type:  Sel */
#define USB_HID_USAGE_TELEPHONY_EMAIL_MESSAGE_WAITING 0x108         /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_VOICEMAIL_MESSAGE_WAITING 0x109     /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_HOST_HOLD 0x10A                     /* Usage type:  OOC */
#define USB_HID_USAGE_TELEPHONY_INCOMING_CALL_HISTORY_COUNT 0x110   /* Usage type:  DV */
#define USB_HID_USAGE_TELEPHONY_OUTGOING_CALL_HISTORY_COUNT 0x111   /* Usage type:  DV */
#define USB_HID_USAGE_TELEPHONY_INCOMING_CALL_HISTORY 0x112         /* Usage type:  CL */
#define USB_HID_USAGE_TELEPHONY_OUTGOING_CALL_HISTORY 0x113         /* Usage type:  CL */
#define USB_HID_USAGE_TELEPHONY_PHONE_LOCALE 0x114                  /* Usage type:  DV */
#define USB_HID_USAGE_TELEPHONY_PHONE_TIME_SECOND 0x140             /* Usage type:  DV */
#define USB_HID_USAGE_TELEPHONY_PHONE_TIME_MINUTE 0x141             /* Usage type:  DV */
#define USB_HID_USAGE_TELEPHONY_PHONE_TIME_HOUR 0x142               /* Usage type:  DV */
#define USB_HID_USAGE_TELEPHONY_PHONE_DATE_DAY 0x143                /* Usage type:  DV */
#define USB_HID_USAGE_TELEPHONY_PHONE_DATE_MONTH 0x144              /* Usage type:  DV */
#define USB_HID_USAGE_TELEPHONY_PHONE_DATE_YEAR 0x145               /* Usage type:  DV */
#define USB_HID_USAGE_TELEPHONY_HANDSET_NICKNAME 0x146              /* Usage type:  DV */
#define USB_HID_USAGE_TELEPHONY_ADDRESS_BOOK_ID 0x147               /* Usage type:  DV */
#define USB_HID_USAGE_TELEPHONY_CALL_DURATION 0x14A                 /* Usage type:  DV */
#define USB_HID_USAGE_TELEPHONY_DUAL_MODE_PHONE 0x14B               /* Usage type:  CA */
/** @} */

/**
 * @name Page 0x0C: Consumer
 * @{
 */
#define USB_HID_USAGE_CONSUMER_UNDEFINED 0x00
#define USB_HID_USAGE_CONSUMER_CONSUMER_CONTROL 0x01                            /* Usage type:  CA */
#define USB_HID_USAGE_CONSUMER_NUMERIC_KEY_PAD 0x02                             /* Usage type:  NAry */
#define USB_HID_USAGE_CONSUMER_PROGRAMMABLE_BUTTONS 0x03                        /* Usage type:  NAry */
#define USB_HID_USAGE_CONSUMER_MICROPHONE 0x04                                  /* Usage type:  CA */
#define USB_HID_USAGE_CONSUMER_HEADPHONE 0x05                                   /* Usage type:  CA */
#define USB_HID_USAGE_CONSUMER_GRAPHIC_EQUALIZER 0x06                           /* Usage type:  CA */
#define USB_HID_USAGE_CONSUMER_INCREMENT10 0x20                                 /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_INCREMENT100 0x21                                /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_AM_PM 0x22                                       /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_POWER 0x30                                       /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_RESET 0x31                                       /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_SLEEP 0x32                                       /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_SLEEP_AFTER 0x33                                 /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_SLEEP_MODE 0x34                                  /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_ILLUMINATION 0x35                                /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_FUNCTION_BUTTONS 0x36                            /* Usage type:  NAry */
#define USB_HID_USAGE_CONSUMER_MENU 0x40                                        /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_MENU_PICK 0x41                                   /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_MENU_UP 0x42                                     /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_MENU_DOWN 0x43                                   /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_MENU_LEFT 0x44                                   /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_MENU_RIGHT 0x45                                  /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_MENU_ESCAPE 0x46                                 /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_MENU_VALUE_INCREASE 0x47                         /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_MENU_VALUE_DECREASE 0x48                         /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_DATA_ON_SCREEN 0x60                              /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_CLOSED_CAPTION 0x61                              /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_CLOSED_CAPTION_SELECT 0x62                       /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_VCR_TV 0x63                                      /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_BROADCAST_MODE 0x64                              /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_SNAPSHOT 0x65                                    /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_STILL 0x66                                       /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_PICTURE_IN_PICTURE_TOGGLE 0x67                   /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_PICTURE_IN_PICTURE_SWAP 0x68                     /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_RED_MENU_BUTTON 0x69                             /* Usage type:  MC */
#define USB_HID_USAGE_CONSUMER_GREEN_MENU_BUTTON 0x6A                           /* Usage type:  MC */
#define USB_HID_USAGE_CONSUMER_BLUE_MENU_BUTTON 0x6B                            /* Usage type:  MC */
#define USB_HID_USAGE_CONSUMER_YELLOW_MENU_BUTTON 0x6C                          /* Usage type:  MC */
#define USB_HID_USAGE_CONSUMER_ASPECT 0x6D                                      /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_3D_MODE_SELECT 0x6E                              /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_DISPLAY_BRIGHTNESS_INCREMENT 0x6F                /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_DISPLAY_BRIGHTNESS_DECREMENT 0x70                /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_DISPLAY_BRIGHTNESS 0x71                          /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_DISPLAY_BACKLIGHT_TOGGLE 0x72                    /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_DISPLAY_SET_BRIGHTNESS_TO_MINIMUM 0x73           /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_DISPLAY_SET_BRIGHTNESS_TO_MAXIMUM 0x74           /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_DISPLAY_SET_AUTO_BRIGHTNESS 0x75                 /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_CAMERA_ACCESS_ENABLED 0x76                       /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_CAMERA_ACCESS_DISABLED 0x77                      /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_CAMERA_ACCESS_TOGGLE 0x78                        /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_BRIGHTNESS_INCREMENT 0x79               /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_BRIGHTNESS_DECREMENT 0x7A               /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_BACKLIGHT_SET_LEVEL 0x7B                /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_BACKLIGHT_OOC 0x7C                      /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_BACKLIGHT_SET_MINIMUM 0x7D              /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_BACKLIGHT_SET_MAXIMUM 0x7E              /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_BACKLIGHT_AUTO 0x7F                     /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_SELECTION 0x80                                   /* Usage type:  NAry */
#define USB_HID_USAGE_CONSUMER_ASSIGN_SELECTION 0x81                            /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_MODE_STEP 0x82                                   /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_RECALL_LAST 0x83                                 /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_ENTER_CHANNEL 0x84                               /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_ORDER_MOVIE 0x85                                 /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_CHANNEL 0x86                                     /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECTION 0x87                             /* Usage type:  NAry */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_COMPUTER 0x88                       /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_TV 0x89                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_WWW 0x8A                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_DVD 0x8B                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_TELEPHONE 0x8C                      /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_PROGRAM_GUIDE 0x8D                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_VIDEO_PHONE 0x8E                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_GAMES 0x8F                          /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_MESSAGES 0x90                       /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_CD 0x91                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_VCR 0x92                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_TUNER 0x93                          /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_QUIT 0x94                                        /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_HELP 0x95                                        /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_TAPE 0x96                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_CABLE 0x97                          /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_SATELLITE 0x98                      /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_SECURITY 0x99                       /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_HOME 0x9A                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_CALL 0x9B                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_CHANNEL_INCREMENT 0x9C                           /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_CHANNEL_DECREMENT 0x9D                           /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_MEDIA_SELECT_SAP 0x9E                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_VCR_PLUS 0xA0                                    /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_ONCE 0xA1                                        /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_DAILY 0xA2                                       /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_WEEKLY 0xA3                                      /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_MONTHLY 0xA4                                     /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_PLAY 0xB0                                        /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_PAUSE 0xB1                                       /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_RECORD 0xB2                                      /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_FAST_FORWARD 0xB3                                /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_REWIND 0xB4                                      /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_SCAN_NEXT_TRACK 0xB5                             /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_SCAN_PREVIOUS_TRACK 0xB6                         /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_STOP 0xB7                                        /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_EJECT 0xB8                                       /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_RANDOM_PLAY 0xB9                                 /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_SELECT_DISC 0xBA                                 /* Usage type:  NAry */
#define USB_HID_USAGE_CONSUMER_ENTER_DISC 0xBB                                  /* Usage type:  MC */
#define USB_HID_USAGE_CONSUMER_REPEAT 0xBC                                      /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_TRACKING 0xBD                                    /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_TRACK_NORMAL 0xBE                                /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_SLOW_TRACKING 0xBF                               /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_FRAME_FORWARD 0xC0                               /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_FRAME_BACK 0xC1                                  /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_MARK 0xC2                                        /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_CLEAR_MARK 0xC3                                  /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_REPEAT_FROM_MARK 0xC4                            /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_RETURN_TO_MARK 0xC5                              /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_SEARCH_MARK_FORWARD 0xC6                         /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_SEARCH_MARK_BACKWARDS 0xC7                       /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_COUNTER_RESET 0xC8                               /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_SHOW_COUNTER 0xC9                                /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_TRACKING_INCREMENT 0xCA                          /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_TRACKING_DECREMENT 0xCB                          /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_STOP_EJECT 0xCC                                  /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_PLAY_PAUSE 0xCD                                  /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_PLAY_SKIP 0xCE                                   /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_VOICE_COMMAND 0xCF                               /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_INVOKE_CAPTURE_INTERFACE 0xD0                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_START_OR_STOP_GAME_RECORDING 0xD1                /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_HISTORICAL_GAME_CAPTURE 0xD2                     /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_CAPTURE_GAME_SCREENSHOT 0xD3                     /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_SHOW_OR_HIDE_RECORDING_INDICATOR 0xD4            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_START_OR_STOP_MICROPHONE_CAPTURE 0xD5            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_START_OR_STOP_CAMERA_CAPTURE 0xD6                /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_START_OR_STOP_GAME_BROADCAST 0xD7                /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_VOLUME 0xE0                                      /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_BALANCE 0xE1                                     /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_MUTE 0xE2                                        /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_BASS 0xE3                                        /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_TREBLE 0xE4                                      /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_BASS_BOOST 0xE5                                  /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_SURROUND_MODE 0xE6                               /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_LOUDNESS 0xE7                                    /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_MPX 0xE8                                         /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_VOLUME_INCREMENT 0xE9                            /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_VOLUME_DECREMENT 0xEA                            /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_SPEED_SELECT 0xF0                                /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_PLAYBACK_SPEED 0xF1                              /* Usage type:  NAry */
#define USB_HID_USAGE_CONSUMER_STANDARD_PLAY 0xF2                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_LONG_PLAY 0xF3                                   /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_EXTENDED_PLAY 0xF4                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_SLOW 0xF5                                        /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_FAN_ENABLE 0x100                                 /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_FAN_SPEED 0x101                                  /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_LIGHT_ENABLE 0x102                               /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_LIGHT_ILLUMINATION_LEVEL 0x103                   /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_CLIMATE_CONTROL_ENABLE 0x104                     /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_ROOM_TEMPERATURE 0x105                           /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_SECURITY_ENABLE 0x106                            /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_FIRE_ALARM 0x107                                 /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_POLICE_ALARM 0x108                               /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_PROXIMITY 0x109                                  /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_MOTION 0x10A                                     /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_DURESS_ALARM 0x10B                               /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_HOLDUP_ALARM 0x10C                               /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_MEDICAL_ALARM 0x10D                              /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_BALANCE_RIGHT 0x150                              /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_BALANCE_LEFT 0x151                               /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_BASS_INCREMENT 0x152                             /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_BASS_DECREMENT 0x153                             /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_TREBLE_INCREMENT 0x154                           /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_TREBLE_DECREMENT 0x155                           /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_SPEAKER_SYSTEM 0x160                             /* Usage type:  CL */
#define USB_HID_USAGE_CONSUMER_CHANNEL_LEFT 0x161                               /* Usage type:  CL */
#define USB_HID_USAGE_CONSUMER_CHANNEL_RIGHT 0x162                              /* Usage type:  CL */
#define USB_HID_USAGE_CONSUMER_CHANNEL_CENTER 0x163                             /* Usage type:  CL */
#define USB_HID_USAGE_CONSUMER_CHANNEL_FRONT 0x164                              /* Usage type:  CL */
#define USB_HID_USAGE_CONSUMER_CHANNEL_CENTER_FRONT 0x165                       /* Usage type:  CL */
#define USB_HID_USAGE_CONSUMER_CHANNEL_SIDE 0x166                               /* Usage type:  CL */
#define USB_HID_USAGE_CONSUMER_CHANNEL_SURROUND 0x167                           /* Usage type:  CL */
#define USB_HID_USAGE_CONSUMER_CHANNEL_LOW_FREQUENCY_ENHANCEMENT 0x168          /* Usage type:  CL */
#define USB_HID_USAGE_CONSUMER_CHANNEL_TOP 0x169                                /* Usage type:  CL */
#define USB_HID_USAGE_CONSUMER_CHANNEL_UNKNOWN 0x16A                            /* Usage type:  CL */
#define USB_HID_USAGE_CONSUMER_SUB_CHANNEL 0x170                                /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_SUB_CHANNEL_INCREMENT 0x171                      /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_SUB_CHANNEL_DECREMENT 0x172                      /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_ALTERNATE_AUDIO_INCREMENT 0x173                  /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_ALTERNATE_AUDIO_DECREMENT 0x174                  /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_APPLICATION_LAUNCH_BUTTONS 0x180                 /* Usage type:  NAry */
#define USB_HID_USAGE_CONSUMER_AL_LAUNCH_BUTTON_CONFIGURATION_TOOL 0x181        /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_PROGRAMMABLE_BUTTON_CONFIGURATION 0x182       /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_CONSUMER_CONTROL_CONFIGURATION 0x183          /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_WORD_PROCESSOR 0x184                          /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_TEXT_EDITOR 0x185                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_SPREADSHEET 0x186                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_GRAPHICS_EDITOR 0x187                         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_PRESENTATION_APP 0x188                        /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_DATABASE_APP 0x189                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_EMAIL_READER 0x18A                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_NEWSREADER 0x18B                              /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_VOICEMAIL 0x18C                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_CONTACTS_ADDRESS_BOOK 0x18D                   /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_CALENDAR_SCHEDULE 0x18E                       /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_TASK_PROJECT_MANAGER 0x18F                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_LOG_JOURNAL_TIMECARD 0x190                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_CHECKBOOK_FINANCE 0x191                       /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_CALCULATOR 0x192                              /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_A_V_CAPTURE_PLAYBACK 0x193                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_LOCAL_MACHINE_BROWSER 0x194                   /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_LAN_WAN_BROWSER 0x195                         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_INTERNET_BROWSER 0x196                        /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_REMOTE_NETWORKING_ISP_CONNECT 0x197           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_NETWORK_CONFERENCE 0x198                      /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_NETWORK_CHAT 0x199                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_TELEPHONY_DIALER 0x19A                        /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_LOGON 0x19B                                   /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_LOGOFF 0x19C                                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_LOGON_LOGOFF 0x19D                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_TERMINAL_LOCK_SCREENSAVER 0x19E               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_CONTROL_PANEL 0x19F                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_COMMAND_LINE_PROCESSOR_RUN 0x1A0              /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_PROCESS_TASK_MANAGER 0x1A1                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_SELECT_TASK_APPLICATION 0x1A2                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_NEXT_TASK_APPLICATION 0x1A3                   /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_PREVIOUS_TASK_APPLICATION 0x1A4               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_PREEMPTIVE_HALT_TASK_APPLICATION 0x1A5        /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_INTEGRATED_HELP_CENTER 0x1A6                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_DOCUMENTS 0x1A7                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_THESAURUS 0x1A8                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_DICTIONARY 0x1A9                              /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_DESKTOP 0x1AA                                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_SPELL_CHECK 0x1AB                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_GRAMMAR_CHECK 0x1AC                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_WIRELESS_STATUS 0x1AD                         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_KEYBOARD_LAYOUT 0x1AE                         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_VIRUS_PROTECTION 0x1AF                        /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_ENCRYPTION 0x1B0                              /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_SCREEN_SAVER 0x1B1                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_ALARMS 0x1B2                                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_CLOCK 0x1B3                                   /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_FILE_BROWSER 0x1B4                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_POWER_STATUS 0x1B5                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_IMAGE_BROWSER 0x1B6                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_AUDIO_BROWSER 0x1B7                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_MOVIE_BROWSER 0x1B8                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_DIGITAL_RIGHTS_MANAGER 0x1B9                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_DIGITAL_WALLET 0x1BA                          /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_INSTANT_MESSAGING 0x1BC                       /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_OEM_FEATURES_TIPS_TUTORIAL_BROWSER 0x1BD      /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_OEM_HELP 0x1BE                                /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_ONLINE_COMMUNITY 0x1BF                        /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_ENTERTAINMENT_CONTENT_BROWSER 0x1C0           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_ONLINE_SHOPPING_BROWSER 0x1C1                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_SMARTCARD_INFORMATION_HELP 0x1C2              /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_MARKET_MONITOR_FINANCE_BROWSER 0x1C3          /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_CUSTOMIZED_CORPORATE_NEWS_BROWSER 0x1C4       /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_ONLINE_ACTIVITY_BROWSER 0x1C5                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_RESEARCH_SEARCH_BROWSER 0x1C6                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_AUDIO_PLAYER 0x1C7                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_MESSAGE_STATUS 0x1C8                          /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_CONTACT_SYNC 0x1C9                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_NAVIGATION 0x1CA                              /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AL_CONTEXT_AWARE_DESKTOP_ASSISTANT 0x1CB         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_GENERIC_GUI_APPLICATION_CONTROLS 0x200           /* Usage type:  NAry */
#define USB_HID_USAGE_CONSUMER_AC_NEW 0x201                                     /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_OPEN 0x202                                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_CLOSE 0x203                                   /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_EXIT 0x204                                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_MAXIMIZE 0x205                                /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_MINIMIZE 0x206                                /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SAVE 0x207                                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_PRINT 0x208                                   /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_PROPERTIES 0x209                              /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_UNDO 0x21A                                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_COPY 0x21B                                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_CUT 0x21C                                     /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_PASTE 0x21D                                   /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SELECT_ALL 0x21E                              /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_FIND 0x21F                                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_FIND_AND_REPLACE 0x220                        /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SEARCH 0x221                                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_GO_TO 0x222                                   /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_HOME 0x223                                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_BACK 0x224                                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_FORWARD 0x225                                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_STOP 0x226                                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_REFRESH 0x227                                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_PREVIOUS_LINK 0x228                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_NEXT_LINK 0x229                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_BOOKMARKS 0x22A                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_HISTORY 0x22B                                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SUBSCRIPTIONS 0x22C                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_ZOOM_IN 0x22D                                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_ZOOM_OUT 0x22E                                /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_ZOOM 0x22F                                    /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_AC_FULL_SCREEN_VIEW 0x230                        /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_NORMAL_VIEW 0x231                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_VIEW_TOGGLE 0x232                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SCROLL_UP 0x233                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SCROLL_DOWN 0x234                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SCROLL 0x235                                  /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_AC_PAN_LEFT 0x236                                /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_PAN_RIGHT 0x237                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_PAN 0x238                                     /* Usage type:  LC */
#define USB_HID_USAGE_CONSUMER_AC_NEW_WINDOW 0x239                              /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_TILE_HORIZONTALLY 0x23A                       /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_TILE_VERTICALLY 0x23B                         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_FORMAT 0x23C                                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_EDIT 0x23D                                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_BOLD 0x23E                                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_ITALICS 0x23F                                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_UNDERLINE 0x240                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_STRIKETHROUGH 0x241                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SUBSCRIPT 0x242                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SUPERSCRIPT 0x243                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_ALL_CAPS 0x244                                /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_ROTATE 0x245                                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_RESIZE 0x246                                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_FLIP_HORIZONTAL 0x247                         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_FLIP_VERTICAL 0x248                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_MIRROR_HORIZONTAL 0x249                       /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_MIRROR_VERTICAL 0x24A                         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_FONT_SELECT 0x24B                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_FONT_COLOR 0x24C                              /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_FONT_SIZE 0x24D                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_JUSTIFY_LEFT 0x24E                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_JUSTIFY_CENTER_H 0x24F                        /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_JUSTIFY_RIGHT 0x250                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_JUSTIFY_BLOCK_H 0x251                         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_JUSTIFY_TOP 0x252                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_JUSTIFY_CENTER_V 0x253                        /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_JUSTIFY_BOTTOM 0x254                          /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_JUSTIFY_BLOCK_V 0x255                         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_INDENT_DECREASE 0x256                         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_INDENT_INCREASE 0x257                         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_NUMBERED_LIST 0x258                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_RESTART_NUMBERING 0x259                       /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_BULLETED_LIST 0x25A                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_PROMOTE 0x25B                                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_DEMOTE 0x25C                                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_YES 0x25D                                     /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_NO 0x25E                                      /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_CANCEL 0x25F                                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_CATALOG 0x260                                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_BUY_CHECKOUT 0x261                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_ADD_TO_CART 0x262                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_EXPAND 0x263                                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_EXPAND_ALL 0x264                              /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_COLLAPSE 0x265                                /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_COLLAPSE_ALL 0x266                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_PRINT_PREVIEW 0x267                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_PASTE_SPECIAL 0x268                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_INSERT_MODE 0x269                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_DELETE 0x26A                                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_LOCK 0x26B                                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_UNLOCK 0x26C                                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_PROTECT 0x26D                                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_UNPROTECT 0x26E                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_ATTACH_COMMENT 0x26F                          /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_DELETE_COMMENT 0x270                          /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_VIEW_COMMENT 0x271                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SELECT_WORD 0x272                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SELECT_SENTENCE 0x273                         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SELECT_PARAGRAPH 0x274                        /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SELECT_COLUMN 0x275                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SELECT_ROW 0x276                              /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SELECT_TABLE 0x277                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SELECT_OBJECT 0x278                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_REDO_REPEAT 0x279                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SORT 0x27A                                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SORT_ASCENDING 0x27B                          /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SORT_DESCENDING 0x27C                         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_FILTER 0x27D                                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SET_CLOCK 0x27E                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_VIEW_CLOCK 0x27F                              /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SELECT_TIME_ZONE 0x280                        /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_EDIT_TIME_ZONES 0x281                         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SET_ALARM 0x282                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_CLEAR_ALARM 0x283                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SNOOZE_ALARM 0x284                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_RESET_ALARM 0x285                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SYNCHRONIZE 0x286                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SEND_RECEIVE 0x287                            /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SEND_TO 0x288                                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_REPLY 0x289                                   /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_REPLY_ALL 0x28A                               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_FORWARD_MSG 0x28B                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SEND 0x28C                                    /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_ATTACH_FILE 0x28D                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_UPLOAD 0x28E                                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_DOWNLOAD_SAVE_TARGET_AS 0x28F                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SET_BORDERS 0x290                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_INSERT_ROW 0x291                              /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_INSERT_COLUMN 0x292                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_INSERT_FILE 0x293                             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_INSERT_PICTURE 0x294                          /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_INSERT_OBJECT 0x295                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_INSERT_SYMBOL 0x296                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SAVE_AND_CLOSE 0x297                          /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_RENAME 0x298                                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_MERGE 0x299                                   /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SPLIT 0x29A                                   /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_DISRIBUTE_HORIZONTALLY 0x29B                  /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_DISTRIBUTE_VERTICALLY 0x29C                   /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_NEXT_KEYBOARD_LAYOUT_SELECT 0x29D             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_NAVIGATION_GUIDANCE 0x29E                     /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_DESKTOP_SHOW_ALL_WINDOWS 0x29F                /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SOFT_KEY_LEFT 0x2A0                           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_SOFT_KEY_RIGHT 0x2A1                          /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_DESKTOP_SHOW_ALL_APPLICATIONS 0x2A2           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_AC_IDLE_KEEP_ALIVE 0x2B0                         /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_EXTENDED_KEYBOARD_ATTRIBUTES_COLLECTION 0x2C0    /* Usage type:  CL */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_FORM_FACTOR 0x2C1                       /* Usage type:  SV */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_KEY_TYPE 0x2C2                          /* Usage type:  SV */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_PHYSICAL_LAYOUT 0x2C3                   /* Usage type:  SV */
#define USB_HID_USAGE_CONSUMER_VENDOR_SPECIFIC_KEYBOARD_PHYSICAL_LAYOUT 0x2C4   /* Usage type:  SV */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_IETF_LANGUAGE_TAG_INDEX 0x2C5           /* Usage type:  SV */
#define USB_HID_USAGE_CONSUMER_IMPLEMENTED_KEYBOARD_INPUT_ASSIST_CONTROLS 0x2C6 /* Usage type:  SV */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_INPUT_ASSIST_PREVIOUS 0x2C7             /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_INPUT_ASSIST_NEXT 0x2C8                 /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_INPUT_ASSIST_PREVIOUS_GROUP 0x2C9       /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_INPUT_ASSIST_NEXT_GROUP 0x2CA           /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_INPUT_ASSIST_ACCEPT 0x2CB               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_KEYBOARD_INPUT_ASSIST_CANCEL 0x2CC               /* Usage type:  Sel */
#define USB_HID_USAGE_CONSUMER_PRIVACY_SCREEN_TOGGLE 0x2D0                      /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_PRIVACY_SCREEN_LEVEL_DECREMENT 0x2D1             /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_PRIVACY_SCREEN_LEVEL_INCREMENT 0x2D2             /* Usage type:  RTC */
#define USB_HID_USAGE_CONSUMER_PRIVACY_SCREEN_LEVEL_MINIMUM 0x2D3               /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_PRIVACY_SCREEN_LEVEL_MAXIMUM 0x2D4               /* Usage type:  OSC */
#define USB_HID_USAGE_CONSUMER_CONTACT_EDITED 0x500                             /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_CONTACT_ADDED 0x501                              /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_CONTACT_RECORD_ACTIVE 0x502                      /* Usage type:  OOC */
#define USB_HID_USAGE_CONSUMER_CONTACT_INDEX 0x503                              /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_NICKNAME 0x504                           /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_FIRST_NAME 0x505                         /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_LAST_NAME 0x506                          /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_FULL_NAME 0x507                          /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_PHONE_NUMBER_PERSONAL 0x508              /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_PHONE_NUMBER_BUSINESS 0x509              /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_PHONE_NUMBER_MOBILE 0x50A                /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_PHONE_NUMBER_PAGER 0x50B                 /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_PHONE_NUMBER_FAX 0x50C                   /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_PHONE_NUMBER_OTHER 0x50D                 /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_EMAIL_PERSONAL 0x50E                     /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_EMAIL_BUSINESS 0x50F                     /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_EMAIL_OTHER 0x510                        /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_EMAIL_MAIN 0x511                         /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_SPEED_DIAL_NUMBER 0x512                  /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_STATUS_FLAG 0x513                        /* Usage type:  DV */
#define USB_HID_USAGE_CONSUMER_CONTACT_MISC 0x514                               /* Usage type:  DV */
/** @} */

/**
 * @name Page 0x0D: Digitizers
 * @{
 */
#define USB_HID_USAGE_DIGITIZERS_UNDEFINED 0x00
#define USB_HID_USAGE_DIGITIZERS_DIGITIZER 0x01                                         /* Usage type:  CA */
#define USB_HID_USAGE_DIGITIZERS_PEN 0x02                                               /* Usage type:  CA */
#define USB_HID_USAGE_DIGITIZERS_LIGHT_PEN 0x03                                         /* Usage type:  CA */
#define USB_HID_USAGE_DIGITIZERS_TOUCH_SCREEN 0x04                                      /* Usage type:  CA */
#define USB_HID_USAGE_DIGITIZERS_TOUCH_PAD 0x05                                         /* Usage type:  CA */
#define USB_HID_USAGE_DIGITIZERS_WHITEBOARD 0x06                                        /* Usage type:  CA */
#define USB_HID_USAGE_DIGITIZERS_COORDINATE_MEASURING_MACHINE 0x07                      /* Usage type:  CA */
#define USB_HID_USAGE_DIGITIZERS_3D_DIGITIZER 0x08                                      /* Usage type:  CA */
#define USB_HID_USAGE_DIGITIZERS_STEREO_PLOTTER 0x09                                    /* Usage type:  CA */
#define USB_HID_USAGE_DIGITIZERS_ARTICULATED_ARM 0x0A                                   /* Usage type:  CA */
#define USB_HID_USAGE_DIGITIZERS_ARMATURE 0x0B                                          /* Usage type:  CA */
#define USB_HID_USAGE_DIGITIZERS_MULTIPLE_POINT_DIGITIZER 0x0C                          /* Usage type:  CA */
#define USB_HID_USAGE_DIGITIZERS_FREE_SPACE_WAND 0x0D                                   /* Usage type:  CA */
#define USB_HID_USAGE_DIGITIZERS_DEVICE_CONFIGURATION 0x0E                              /* Usage type:  CA */
#define USB_HID_USAGE_DIGITIZERS_CAPACITIVE_HEAT_MAP_DIGITIZER 0x0F                     /* Usage type:  CA */
#define USB_HID_USAGE_DIGITIZERS_STYLUS 0x20                                            /* Usage type:  CA, CL */
#define USB_HID_USAGE_DIGITIZERS_PUCK 0x21                                              /* Usage type:  CL */
#define USB_HID_USAGE_DIGITIZERS_FINGER 0x22                                            /* Usage type:  CL */
#define USB_HID_USAGE_DIGITIZERS_DEVICE_SETTINGS 0x23                                   /* Usage type:  CL */
#define USB_HID_USAGE_DIGITIZERS_CHARACTER_GESTURE 0x24                                 /* Usage type:  CL */
#define USB_HID_USAGE_DIGITIZERS_TIP_PRESSURE 0x30                                      /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_BARREL_PRESSURE 0x31                                   /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_IN_RANGE 0x32                                          /* Usage type:  MC */
#define USB_HID_USAGE_DIGITIZERS_TOUCH 0x33                                             /* Usage type:  MC */
#define USB_HID_USAGE_DIGITIZERS_UNTOUCH 0x34                                           /* Usage type:  OSC */
#define USB_HID_USAGE_DIGITIZERS_TAP 0x35                                               /* Usage type:  OSC */
#define USB_HID_USAGE_DIGITIZERS_QUALITY 0x36                                           /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_DATA_VALID 0x37                                        /* Usage type:  MC */
#define USB_HID_USAGE_DIGITIZERS_TRANSDUCER_INDEX 0x38                                  /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_TABLET_FUNCTION_KEYS 0x39                              /* Usage type:  CL */
#define USB_HID_USAGE_DIGITIZERS_PROGRAM_CHANGE_KEYS 0x3A                               /* Usage type:  CL */
#define USB_HID_USAGE_DIGITIZERS_BATTERY_STRENGTH 0x3B                                  /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_INVERT 0x3C                                            /* Usage type:  MC */
#define USB_HID_USAGE_DIGITIZERS_X_TILT 0x3D                                            /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_Y_TILT 0x3E                                            /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_AZIMUTH 0x3F                                           /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_ALTITUDE 0x40                                          /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_TWIST 0x41                                             /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_TIP_SWITCH 0x42                                        /* Usage type:  MC */
#define USB_HID_USAGE_DIGITIZERS_SECONDARY_TIP_SWITCH 0x43                              /* Usage type:  MC */
#define USB_HID_USAGE_DIGITIZERS_BARREL_SWITCH 0x44                                     /* Usage type:  MC */
#define USB_HID_USAGE_DIGITIZERS_ERASER 0x45                                            /* Usage type:  MC */
#define USB_HID_USAGE_DIGITIZERS_TABLET_PICK 0x46                                       /* Usage type:  MC */
#define USB_HID_USAGE_DIGITIZERS_TOUCH_VALID 0x47                                       /* Usage type:  MC */
#define USB_HID_USAGE_DIGITIZERS_WIDTH 0x48                                             /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_HEIGHT 0x49                                            /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_CONTACT_IDENTIFIER 0x51                                /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_DEVICE_MODE 0x52                                       /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_DEVICE_IDENTIFIER 0x53                                 /* Usage type:  DV, SV */
#define USB_HID_USAGE_DIGITIZERS_CONTACT_COUNT 0x54                                     /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_CONTACT_COUNT_MAXIMUM 0x55                             /* Usage type:  SV */
#define USB_HID_USAGE_DIGITIZERS_SCAN_TIME 0x56                                         /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_SURFACE_SWITCH 0x57                                    /* Usage type:  DF */
#define USB_HID_USAGE_DIGITIZERS_BUTTON_SWITCH 0x58                                     /* Usage type:  DF */
#define USB_HID_USAGE_DIGITIZERS_PAD_TYPE 0x59                                          /* Usage type:  SF */
#define USB_HID_USAGE_DIGITIZERS_SECONDARY_BARREL_SWITCH 0x5A                           /* Usage type:  MC */
#define USB_HID_USAGE_DIGITIZERS_TRANSDUCER_SERIAL_NUMBER 0x5B                          /* Usage type:  SV */
#define USB_HID_USAGE_DIGITIZERS_PREFERRED_COLOR 0x5C                                   /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_PREFERRED_COLOR_IS_LOCKED 0x5D                         /* Usage type:  MC */
#define USB_HID_USAGE_DIGITIZERS_PREFERRED_LINE_WIDTH 0x5E                              /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_PREFERRED_LINE_WIDTH_IS_LOCKED 0x5F                    /* Usage type:  MC */
#define USB_HID_USAGE_DIGITIZERS_LATENCY_MODE 0x60                                      /* Usage type:  DF */
#define USB_HID_USAGE_DIGITIZERS_GESTURE_CHARACTER_QUALITY 0x61                         /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_CHARACTER_GESTURE_DATA_LENGTH 0x62                     /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_CHARACTER_GESTURE_DATA 0x63                            /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_GESTURE_CHARACTER_ENCODING 0x64                        /* Usage type:  NAry */
#define USB_HID_USAGE_DIGITIZERS_UTF8_CHARACTER_GESTURE_ENCODING 0x65                   /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_UTF16_LITTLE_ENDIAN_CHARACTER_GESTURE_ENCODING 0x66    /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_UTF16_BIG_ENDIAN_CHARACTER_GESTURE_ENCODING 0x67       /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_UTF32_LITTLE_ENDIAN_CHARACTER_GESTURE_ENCODING 0x68    /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_UTF32_BIG_ENDIAN_CHARACTER_GESTURE_ENCODING 0x69       /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_CAPACITIVE_HEAT_MAP_PROTOCOL_VENDOR_ID 0x6A            /* Usage type:  SV */
#define USB_HID_USAGE_DIGITIZERS_CAPACITIVE_HEAT_MAP_PROTOCOL_VERSION 0x6B              /* Usage type:  SV */
#define USB_HID_USAGE_DIGITIZERS_CAPACITIVE_HEAT_MAP_FRAME_DATA 0x6C                    /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_GESTURE_CHARACTER_ENABLE 0x6D                          /* Usage type:  DF */
#define USB_HID_USAGE_DIGITIZERS_PREFERRED_LINE_STYLE 0x70                              /* Usage type:  NAry */
#define USB_HID_USAGE_DIGITIZERS_PREFERRED_LINE_STYLE_IS_LOCKED 0x71                    /* Usage type:  MC */
#define USB_HID_USAGE_DIGITIZERS_INK 0x72                                               /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_PENCIL 0x73                                            /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_HIGHLIGHTER 0x74                                       /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_CHISEL_MARKER 0x75                                     /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_BRUSH 0x76                                             /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_NO_PREFERENCE 0x77                                     /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_DIGITIZER_DIAGNOSTIC 0x80                              /* Usage type:  CL */
#define USB_HID_USAGE_DIGITIZERS_DIGITIZER_ERROR 0x81                                   /* Usage type:  NAry */
#define USB_HID_USAGE_DIGITIZERS_ERR_NORMAL_STATUS 0x82                                 /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_ERR_TRANSDUCERS_EXCEEDED 0x83                          /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_ERR_FULL_TRANS_FEATURES_UNAVAILABLE 0x84               /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_ERR_CHARGE_LOW 0x85                                    /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_TRANSDUCER_SOFTWARE_INFO 0x90                          /* Usage type:  CL */
#define USB_HID_USAGE_DIGITIZERS_TRANSDUCER_VENDOR_ID 0x91                              /* Usage type:  SV */
#define USB_HID_USAGE_DIGITIZERS_TRANSDUCER_PRODUCT_ID 0x92                             /* Usage type:  SV */
#define USB_HID_USAGE_DIGITIZERS_DEVICE_SUPPORTED_PROTOCOLS 0x93                        /* Usage type:  NAry, CL */
#define USB_HID_USAGE_DIGITIZERS_TRANSDUCER_SUPPORTED_PROTOCOLS 0x94                    /* Usage type:  NAry, CL */
#define USB_HID_USAGE_DIGITIZERS_NO_PROTOCOL 0x95                                       /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_WACOM_AES_PROTOCOL 0x96                                /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_USI_PROTOCOL 0x97                                      /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_MICROSOFT_PEN_PROTOCOL 0x98                            /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_SUPPORTED_REPORT_RATES 0xA0                            /* Usage type:  SV, CL */
#define USB_HID_USAGE_DIGITIZERS_REPORT_RATE 0xA1                                       /* Usage type:  DV */
#define USB_HID_USAGE_DIGITIZERS_TRANSDUCER_CONNECTED 0xA2                              /* Usage type:  SF */
#define USB_HID_USAGE_DIGITIZERS_SWITCH_DISABLED 0xA3                                   /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_SWITCH_UNIMPLEMENTED 0xA4                              /* Usage type:  Sel */
#define USB_HID_USAGE_DIGITIZERS_TRANSDUCER_SWITCHES 0xA5                               /* Usage type:  Sel */
/** @} */

/**
 * @name Page 0x0E: Haptics
 * @{
 */
#define USB_HID_USAGE_HAPTICS_UNDEFINED 0x00
#define USB_HID_USAGE_HAPTICS_SIMPLE_HAPTIC_CONTROLLER 0x01             /* Usage type:  CA, CL */
#define USB_HID_USAGE_HAPTICS_WAVEFORM_LIST 0x10                        /* Usage type:  NAry */
#define USB_HID_USAGE_HAPTICS_DURATION_LIST 0x11                        /* Usage type:  NAry */
#define USB_HID_USAGE_HAPTICS_AUTO_TRIGGER 0x20                         /* Usage type:  DV */
#define USB_HID_USAGE_HAPTICS_MANUAL_TRIGGER 0x21                       /* Usage type:  DV */
#define USB_HID_USAGE_HAPTICS_AUTO_TRIGGER_ASSOCIATED_CONTROL 0x22      /* Usage type:  SV */
#define USB_HID_USAGE_HAPTICS_INTENSITY 0x23                            /* Usage type:  DV */
#define USB_HID_USAGE_HAPTICS_REPEAT_COUNT 0x24                         /* Usage type:  DV */
#define USB_HID_USAGE_HAPTICS_RETRIGGER_PERIOD 0x25                     /* Usage type:  DV */
#define USB_HID_USAGE_HAPTICS_WAVEFORM_VENDOR_PAGE 0x26                 /* Usage type:  SV */
#define USB_HID_USAGE_HAPTICS_WAVEFORM_VENDOR_ID 0x27                   /* Usage type:  SV */
#define USB_HID_USAGE_HAPTICS_WAVEFORM_CUTOFF_TIME 0x28                 /* Usage type:  SV */
#define USB_HID_USAGE_HAPTICS_WAVEFORM_NONE 0x1001                      /* Usage type:  SV */
#define USB_HID_USAGE_HAPTICS_WAVEFORM_STOP 0x1002                      /* Usage type:  SV */
#define USB_HID_USAGE_HAPTICS_WAVEFORM_CLICK 0x1003                     /* Usage type:  SV */
#define USB_HID_USAGE_HAPTICS_WAVEFORM_BUZZ_CONTINUOUS 0x1004           /* Usage type:  SV */
#define USB_HID_USAGE_HAPTICS_WAVEFORM_RUMBLE_CONTINUOUS 0x1005         /* Usage type:  SV */
#define USB_HID_USAGE_HAPTICS_WAVEFORM_PRESS 0x1006                     /* Usage type:  SV */
#define USB_HID_USAGE_HAPTICS_WAVEFORM_RELEASE 0x1007                   /* Usage type:  SV */
/** @} */

/**
 * @name Page 0x0F: PID
 * @{
 */
#define USB_HID_USAGE_PID_UNDEFINED 0x00
#define USB_HID_USAGE_PID_PHYSICAL_INTERFACE_DEVICE 0x01
#define USB_HID_USAGE_PID_NORMAL 0x20
#define USB_HID_USAGE_PID_SET_EFFECT_REPORT 0x21
#define USB_HID_USAGE_PID_EFFECT_BLOCK_INDEX 0x22
#define USB_HID_USAGE_PID_PARAMETER_BLOCK_OFFSET 0x23
#define USB_HID_USAGE_PID_ROM_FLAG 0x24
#define USB_HID_USAGE_PID_EFFECT_TYPE 0x25
#define USB_HID_USAGE_PID_ET_CONSTANT_FORCE 0x26
#define USB_HID_USAGE_PID_ET_RAMP 0x27
#define USB_HID_USAGE_PID_ET_CUSTOM_FORCE_DATA 0x28
#define USB_HID_USAGE_PID_ET_SQUARE 0x30
#define USB_HID_USAGE_PID_ET_SINE 0x31
#define USB_HID_USAGE_PID_ET_TRIANGLE 0x32
#define USB_HID_USAGE_PID_ET_SAWTOOTH_UP 0x33
#define USB_HID_USAGE_PID_ET_SAWTOOTH_DOWN 0x34
#define USB_HID_USAGE_PID_ET_SPRING 0x40
#define USB_HID_USAGE_PID_ET_DAMPER 0x41
#define USB_HID_USAGE_PID_ET_INERTIA 0x42
#define USB_HID_USAGE_PID_ET_FRICTION 0x43
#define USB_HID_USAGE_PID_DURATION 0x50
#define USB_HID_USAGE_PID_SAMPLE_PERIOD 0x51
#define USB_HID_USAGE_PID_GAIN 0x52
#define USB_HID_USAGE_PID_TRIGGER_BUTTON 0x53
#define USB_HID_USAGE_PID_TRIGGER_REPEAT_INTERVAL 0x54
#define USB_HID_USAGE_PID_AXES_ENABLE 0x55
#define USB_HID_USAGE_PID_DIRECTION_ENABLE 0x56
#define USB_HID_USAGE_PID_DIRECTION 0x57
#define USB_HID_USAGE_PID_TYPE_SPECIFIC_BLOCK_OFFSET 0x58
#define USB_HID_USAGE_PID_BLOCK_TYPE 0x59
#define USB_HID_USAGE_PID_SET_ENVELOPE_REPORT 0x5A
#define USB_HID_USAGE_PID_ATTACK_LEVEL 0x5B
#define USB_HID_USAGE_PID_ATTACK_TIME 0x5C
#define USB_HID_USAGE_PID_FADE_LEVEL 0x5D
#define USB_HID_USAGE_PID_FADE_TIME 0x5E
#define USB_HID_USAGE_PID_SET_CONDITION_REPORT 0x5F
#define USB_HID_USAGE_PID_CP_OFFSET 0x60
#define USB_HID_USAGE_PID_POSITIVE_COEFFICIENT 0x61
#define USB_HID_USAGE_PID_NEGATIVE_COEFFICIENT 0x62
#define USB_HID_USAGE_PID_POSITIVE_SATURATION 0x63
#define USB_HID_USAGE_PID_NEGATIVE_SATURATION 0x64
#define USB_HID_USAGE_PID_DEAD_BAND 0x65
#define USB_HID_USAGE_PID_DOWNLOAD_FORCE_SAMPLE 0x66
#define USB_HID_USAGE_PID_ISOCH_CUSTOM_FORCE_ENABLE 0x67
#define USB_HID_USAGE_PID_CUSTOM_FORCE_DATA_REPORT 0x68
#define USB_HID_USAGE_PID_CUSTOM_FORCE_DATA 0x69
#define USB_HID_USAGE_PID_CUSTOM_FORCE_VENDOR_DEFINED_DATA 0x6A
#define USB_HID_USAGE_PID_SET_CUSTOM_FORCE_REPORT 0x6B
#define USB_HID_USAGE_PID_CUSTOM_FORCE_DATA_OFFSET 0x6C
#define USB_HID_USAGE_PID_SAMPLE_COUNT 0x6D
#define USB_HID_USAGE_PID_SET_PERIODIC_REPORT 0x6E
#define USB_HID_USAGE_PID_OFFSET 0x6F
#define USB_HID_USAGE_PID_MAGNITUDE 0x70
#define USB_HID_USAGE_PID_PHASE 0x71
#define USB_HID_USAGE_PID_PERIOD 0x72
#define USB_HID_USAGE_PID_SET_CONSTANT_FORCE_REPORT 0x73
#define USB_HID_USAGE_PID_SET_RAMP_FORCE_REPORT 0x74
#define USB_HID_USAGE_PID_RAMP_START 0x75
#define USB_HID_USAGE_PID_RAMP_END 0x76
#define USB_HID_USAGE_PID_EFFECT_OPERATION_REPORT 0x77
#define USB_HID_USAGE_PID_EFFECT_OPERATION 0x78
#define USB_HID_USAGE_PID_OP_EFFECT_START 0x79
#define USB_HID_USAGE_PID_OP_EFFECT_START_SOLO 0x7A
#define USB_HID_USAGE_PID_OP_EFFECT_STOP 0x7B
#define USB_HID_USAGE_PID_LOOP_COUNT 0x7C
#define USB_HID_USAGE_PID_DEVICE_GAIN_REPORT 0x7D
#define USB_HID_USAGE_PID_DEVICE_GAIN 0x7E
#define USB_HID_USAGE_PID_PID_POOL_REPORT 0x7F
#define USB_HID_USAGE_PID_RAM_POOL_SIZE 0x80
#define USB_HID_USAGE_PID_ROM_POOL_SIZE 0x81
#define USB_HID_USAGE_PID_ROM_EFFECT_BLOCK_COUNT 0x82
#define USB_HID_USAGE_PID_SIMULTANEOUS_EFFECTS_MAX 0x83
#define USB_HID_USAGE_PID_POOL_ALIGNMENT 0x84
#define USB_HID_USAGE_PID_PID_POOL_MOVE_REPORT 0x85
#define USB_HID_USAGE_PID_MOVE_SOURCE 0x86
#define USB_HID_USAGE_PID_MOVE_DESTINATION 0x87
#define USB_HID_USAGE_PID_MOVE_LENGTH 0x88
#define USB_HID_USAGE_PID_PID_BLOCK_LOAD_REPORT 0x89
#define USB_HID_USAGE_PID_BLOCK_LOAD_STATUS 0x8B
#define USB_HID_USAGE_PID_BLOCK_LOAD_SUCCESS 0x8C
#define USB_HID_USAGE_PID_BLOCK_LOAD_FULL 0x8D
#define USB_HID_USAGE_PID_BLOCK_LOAD_ERROR 0x8E
#define USB_HID_USAGE_PID_BLOCK_HANDLE 0x8F
#define USB_HID_USAGE_PID_PID_BLOCK_FREE_REPORT 0x90
#define USB_HID_USAGE_PID_TYPE_SPECIFIC_BLOCK_HANDLE 0x91
#define USB_HID_USAGE_PID_PID_STATE_REPORT 0x92
#define USB_HID_USAGE_PID_EFFECT_PLAYING 0x94
#define USB_HID_USAGE_PID_PID_DEVICE_CONTROL_REPORT 0x95
#define USB_HID_USAGE_PID_PID_DEVICE_CONTROL 0x96
#define USB_HID_USAGE_PID_DC_ENABLE_ACTUATORS 0x97
#define USB_HID_USAGE_PID_DC_DISABLE_ACTUATORS 0x98
#define USB_HID_USAGE_PID_DC_STOP_ALL_EFFECTS 0x99
#define USB_HID_USAGE_PID_DC_DEVICE_RESET 0x9A
#define USB_HID_USAGE_PID_DC_DEVICE_PAUSE 0x9B
#define USB_HID_USAGE_PID_DC_DEVICE_CONTINUE 0x9C
#define USB_HID_USAGE_PID_DEVICE_PAUSED 0x9F
#define USB_HID_USAGE_PID_ACTUATORS_ENABLED 0xA0
#define USB_HID_USAGE_PID_SAFETY_SWITCH 0xA4
#define USB_HID_USAGE_PID_ACTUATOR_OVERRIDE_SWITCH 0xA5
#define USB_HID_USAGE_PID_ACTUATOR_POWER 0xA6
#define USB_HID_USAGE_PID_START_DELAY 0xA7
#define USB_HID_USAGE_PID_PARAMETER_BLOCK_SIZE 0xA8
#define USB_HID_USAGE_PID_DEVICE_MANAGED_POOL 0xA9
#define USB_HID_USAGE_PID_SHARED_PARAMETER_BLOCKS 0xAA
#define USB_HID_USAGE_PID_CREATE_NEW_EFFECT_REPORT 0xAB
#define USB_HID_USAGE_PID_RAM_POOL_AVAILABLE 0xAC
/** @} */

/**
 * @name Page 0x12: Eye and Head Trackers
 * @{
 */
#define USB_HID_USAGE_EHT_UNDEFINED 0x00
#define USB_HID_USAGE_EHT_EYE_TRACKER 0x01                      /* Usage type:  CA */
#define USB_HID_USAGE_EHT_HEAD_TRACKER 0x02                     /* Usage type:  CA */
#define USB_HID_USAGE_EHT_TRACKING_DATA 0x10                    /* Usage type:  CP */
#define USB_HID_USAGE_EHT_CAPABILITIES 0x11                     /* Usage type:  CL */
#define USB_HID_USAGE_EHT_CONFIGURATION 0x12                    /* Usage type:  CL */
#define USB_HID_USAGE_EHT_STATUS 0x13                           /* Usage type:  CL */
#define USB_HID_USAGE_EHT_CONTROL 0x14                          /* Usage type:  CL */
#define USB_HID_USAGE_EHT_SENSOR_TIMESTAMP 0x20                 /* Usage type:  DV */
#define USB_HID_USAGE_EHT_POSITION_X 0x21                       /* Usage type:  DV */
#define USB_HID_USAGE_EHT_POSITION_Y 0x22                       /* Usage type:  DV */
#define USB_HID_USAGE_EHT_POSITION_Z 0x23                       /* Usage type:  DV */
#define USB_HID_USAGE_EHT_GAZE_POINT 0x24                       /* Usage type:  CP */
#define USB_HID_USAGE_EHT_LEFT_EYE_POSITION 0x25                /* Usage type:  CP */
#define USB_HID_USAGE_EHT_RIGHT_EYE_POSITION 0x26               /* Usage type:  CP */
#define USB_HID_USAGE_EHT_HEAD_POSITION 0x27                    /* Usage type:  CP */
#define USB_HID_USAGE_EHT_HEAD_DIRECTION_POINT 0x28             /* Usage type:  CP */
#define USB_HID_USAGE_EHT_ROTATION_ABOUT_X_AXIS 0x29            /* Usage type:  DV */
#define USB_HID_USAGE_EHT_ROTATION_ABOUT_Y_AXIS 0x2A            /* Usage type:  DV */
#define USB_HID_USAGE_EHT_ROTATION_ABOUT_Z_AXIS 0x2B            /* Usage type:  DV */
#define USB_HID_USAGE_EHT_TRACKER_QUALITY 0x100                 /* Usage type:  SV */
#define USB_HID_USAGE_EHT_MINIMUM_TRACKING_DISTANCE 0x101       /* Usage type:  SV */
#define USB_HID_USAGE_EHT_OPTIMUM_TRACKING_DISTANCE 0x102       /* Usage type:  SV */
#define USB_HID_USAGE_EHT_MAXIMUM_TRACKING_DISTANCE 0x103       /* Usage type:  SV */
#define USB_HID_USAGE_EHT_MAXIMUM_SCREEN_PLANE_WIDTH 0x104      /* Usage type:  SV */
#define USB_HID_USAGE_EHT_MAXIMUM_SCREEN_PLANE_HEIGHT 0x105     /* Usage type:  SV */
#define USB_HID_USAGE_EHT_DISPLAY_MANUFACTURER_ID 0x200         /* Usage type:  SV */
#define USB_HID_USAGE_EHT_DISPLAY_PRODUCT_ID 0x201              /* Usage type:  SV */
#define USB_HID_USAGE_EHT_DISPLAY_SERIAL_NUMBER 0x202           /* Usage type:  SV */
#define USB_HID_USAGE_EHT_DISPLAY_MANUFACTURER_DATE 0x203       /* Usage type:  SV */
#define USB_HID_USAGE_EHT_CALIBRATED_SCREEN_WIDTH 0x204         /* Usage type:  SV */
#define USB_HID_USAGE_EHT_CALIBRATED_SCREEN_HEIGHT 0x205        /* Usage type:  SV */
#define USB_HID_USAGE_EHT_SAMPLING_FREQUENCY 0x300              /* Usage type:  DV */
#define USB_HID_USAGE_EHT_CONFIGURATION_STATUS 0x301            /* Usage type:  DV */
#define USB_HID_USAGE_EHT_DEVICE_MODE_REQUEST 0x400             /* Usage type:  DV */
/** @} */

/**
 * @name Page 0x14: Auxiliary Display
 * @{
 */
#define USB_HID_USAGE_AUXDISP_UNDEFINED 0x00
#define USB_HID_USAGE_AUXDISP_ALPHANUMERIC_DISPLAY 0x01             /* Usage type:  CA */
#define USB_HID_USAGE_AUXDISP_AUXILIARY_DISPLAY 0x02                /* Usage type:  CA */
#define USB_HID_USAGE_AUXDISP_DISPLAY_ATTRIBUTES_REPORT 0x20        /* Usage type:  CL */
#define USB_HID_USAGE_AUXDISP_ASCII_CHARACTER_SET 0x21              /* Usage type:  SF */
#define USB_HID_USAGE_AUXDISP_DATA_READ_BACK 0x22                   /* Usage type:  SF */
#define USB_HID_USAGE_AUXDISP_FONT_READ_BACK 0x23                   /* Usage type:  SF */
#define USB_HID_USAGE_AUXDISP_DISPLAY_CONTROL_REPORT 0x24           /* Usage type:  CL */
#define USB_HID_USAGE_AUXDISP_CLEAR_DISPLAY 0x25                    /* Usage type:  DF */
#define USB_HID_USAGE_AUXDISP_DISPLAY_ENABLE 0x26                   /* Usage type:  DF */
#define USB_HID_USAGE_AUXDISP_SCREEN_SAVER_DELAY 0x27               /* Usage type:  SV, DV */
#define USB_HID_USAGE_AUXDISP_SCREEN_SAVER_ENABLE 0x28              /* Usage type:  DF */
#define USB_HID_USAGE_AUXDISP_VERTICAL_SCROLL 0x29                  /* Usage type:  SF, DF */
#define USB_HID_USAGE_AUXDISP_HORIZONTAL_SCROLL 0x2A                /* Usage type:  SF, DF */
#define USB_HID_USAGE_AUXDISP_CHARACTER_REPORT 0x2B                 /* Usage type:  CL */
#define USB_HID_USAGE_AUXDISP_DISPLAY_DATA 0x2C                     /* Usage type:  DV */
#define USB_HID_USAGE_AUXDISP_DISPLAY_STATUS 0x2D                   /* Usage type:  CL */
#define USB_HID_USAGE_AUXDISP_STAT_NOT_READY 0x2E                   /* Usage type:  Sel */
#define USB_HID_USAGE_AUXDISP_STAT_READY 0x2F                       /* Usage type:  Sel */
#define USB_HID_USAGE_AUXDISP_ERR_NOT_A_LOADABLE_CHARACTER 0x30     /* Usage type:  Sel */
#define USB_HID_USAGE_AUXDISP_ERR_FONT_DATA_CANNOT_BE_READ 0x31     /* Usage type:  Sel */
#define USB_HID_USAGE_AUXDISP_CURSOR_POSITION_REPORT 0x32           /* Usage type:  Sel */
#define USB_HID_USAGE_AUXDISP_ROW 0x33                              /* Usage type:  DV */
#define USB_HID_USAGE_AUXDISP_COLUMN 0x34                           /* Usage type:  DV */
#define USB_HID_USAGE_AUXDISP_ROWS 0x35                             /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_COLUMNS 0x36                          /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_CURSOR_PIXEL_POSITIONING 0x37         /* Usage type:  SF */
#define USB_HID_USAGE_AUXDISP_CURSOR_MODE 0x38                      /* Usage type:  DF */
#define USB_HID_USAGE_AUXDISP_CURSOR_ENABLE 0x39                    /* Usage type:  DF */
#define USB_HID_USAGE_AUXDISP_CURSOR_BLINK 0x3A                     /* Usage type:  DF */
#define USB_HID_USAGE_AUXDISP_FONT_REPORT 0x3B                      /* Usage type:  CL */
#define USB_HID_USAGE_AUXDISP_FONT_DATA 0x3C                        /* Usage type:  Buffered Bytes */
#define USB_HID_USAGE_AUXDISP_CHARACTER_WIDTH 0x3D                  /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_CHARACTER_HEIGHT 0x3E                 /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_CHARACTER_SPACING_HORIZONTAL 0x3F     /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_CHARACTER_SPACING_VERTICAL 0x40       /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_UNICODE_CHARACTER_SET 0x41            /* Usage type:  SF */
#define USB_HID_USAGE_AUXDISP_FONT_7_SEGMENT 0x42                   /* Usage type:  SF */
#define USB_HID_USAGE_AUXDISP_7_SEGMENT_DIRECT_MAP 0x43             /* Usage type:  SF */
#define USB_HID_USAGE_AUXDISP_FONT_14_SEGMENT 0x44                  /* Usage type:  SF */
#define USB_HID_USAGE_AUXDISP_14_SEGMENT_DIRECT_MAP 0x45            /* Usage type:  SF */
#define USB_HID_USAGE_AUXDISP_DISPLAY_BRIGHTNESS 0x46               /* Usage type:  DV */
#define USB_HID_USAGE_AUXDISP_DISPLAY_CONTRAST 0x47                 /* Usage type:  DV */
#define USB_HID_USAGE_AUXDISP_CHARACTER_ATTRIBUTE 0x48              /* Usage type:  CL */
#define USB_HID_USAGE_AUXDISP_ATTRIBUTE_READBACK 0x49               /* Usage type:  SF */
#define USB_HID_USAGE_AUXDISP_ATTRIBUTE_DATA 0x4A                   /* Usage type:  DV */
#define USB_HID_USAGE_AUXDISP_CHAR_ATTR_ENHANCE 0x4B                /* Usage type:  OOC */
#define USB_HID_USAGE_AUXDISP_CHAR_ATTR_UNDERLINE 0x4C              /* Usage type:  OOC */
#define USB_HID_USAGE_AUXDISP_CHAR_ATTR_BLINK 0x4D                  /* Usage type:  OOC */
#define USB_HID_USAGE_AUXDISP_BITMAP_SIZE_X 0x80                    /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_BITMAP_SIZE_Y 0x81                    /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_MAX_BLIT_SIZE 0x82                    /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_BIT_DEPTH_FORMAT 0x83                 /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_DISPLAY_ORIENTATION 0x84              /* Usage type:  DV */
#define USB_HID_USAGE_AUXDISP_PALETTE_REPORT 0x85                   /* Usage type:  CL */
#define USB_HID_USAGE_AUXDISP_PALETTE_DATA_SIZE 0x86                /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_PALETTE_DATA_OFFSET 0x87              /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_PALETTE_DATA 0x88                     /* Usage type:  Buffered Bytes */
#define USB_HID_USAGE_AUXDISP_BLIT_REPORT 0x8A                      /* Usage type:  CL */
#define USB_HID_USAGE_AUXDISP_BLIT_RECTANGLE_X1 0x8B                /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_BLIT_RECTANGLE_Y1 0x8C                /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_BLIT_RECTANGLE_X2 0x8D                /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_BLIT_RECTANGLE_Y2 0x8E                /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_BLIT_DATA 0x8F                        /* Usage type:  Buffered Bytes */
#define USB_HID_USAGE_AUXDISP_SOFT_BUTTON 0x90                      /* Usage type:  CL */
#define USB_HID_USAGE_AUXDISP_SOFT_BUTTON_ID 0x91                   /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_SOFT_BUTTON_SIDE 0x92                 /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_SOFT_BUTTON_OFFSET_1 0x93             /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_SOFT_BUTTON_OFFSET_2 0x94             /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_SOFT_BUTTON_REPORT 0x95               /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_SOFT_KEYS 0xC2                        /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_DISPLAY_DATA_EXTENSIONS 0xCC          /* Usage type:  SF */
#define USB_HID_USAGE_AUXDISP_CHARACTER_MAPPING 0xCF                /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_UNICODE_EQUIVALENT 0xDD               /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_CHARACTER_PAGE_MAPPING 0xDF           /* Usage type:  SV */
#define USB_HID_USAGE_AUXDISP_REQUEST_REPORT 0xFF                   /* Usage type:  DV */
/** @} */

/**
 * @name Page 0x20: Sensors
 * @{
 */
#define USB_HID_USAGE_SENSORS_UNDEFINED 0x00
#define USB_HID_USAGE_SENSORS_SENSOR 0x01                                                   /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_BIOMETRIC 0x10                                                /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_BIOMETRIC_HUMAN_PRESENCE 0x11                                 /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_BIOMETRIC_HUMAN_PROXIMITY 0x12                                /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_BIOMETRIC_HUMAN_TOUCH 0x13                                    /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_BIOMETRIC_BLOOD_PRESSURE 0x14                                 /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_BIOMETRIC_BODY_TEMPERATURE 0x15                               /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_BIOMETRIC_HEART_RATE 0x16                                     /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_BIOMETRIC_HEART_RATE_VARIABILITY 0x17                         /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_BIOMETRIC_PERIPHERAL_OXYGEN_SATURATION 0x18                   /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_BIOMETRIC_RESPIRATORY_RATE 0x19                               /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ELECTRICAL 0x20                                               /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ELECTRICAL_CAPACITANCE 0x21                                   /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ELECTRICAL_CURRENT 0x22                                       /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ELECTRICAL_POWER 0x23                                         /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ELECTRICAL_INDUCTANCE 0x24                                    /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ELECTRICAL_RESISTANCE 0x25                                    /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ELECTRICAL_VOLTAGE 0x26                                       /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ELECTRICAL_POTENTIOMETER 0x27                                 /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ELECTRICAL_FREQUENCY 0x28                                     /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ELECTRICAL_PERIOD 0x29                                        /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ENVIRONMENTAL 0x30                                            /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ENVIRONMENTAL_ATMOSPHERIC_PRESSURE 0x31                       /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ENVIRONMENTAL_HUMIDITY 0x32                                   /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ENVIRONMENTAL_TEMPERATURE 0x33                                /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ENVIRONMENTAL_WIND_DIRECTION 0x34                             /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ENVIRONMENTAL_WIND_SPEED 0x35                                 /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ENVIRONMENTAL_AIR_QUALITY 0x36                                /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ENVIRONMENTAL_HEAT_INDEX 0x37                                 /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ENVIRONMENTAL_SURFACE_TEMPERATURE 0x38                        /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ENVIRONMENTAL_VOLATILE_ORGANIC_COMPOUNDS 0x39                 /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ENVIRONMENTAL_OBJECT_PRESENCE 0x3A                            /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ENVIRONMENTAL_OBJECT_PROXIMITY 0x3B                           /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_LIGHT 0x40                                                    /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_LIGHT_AMBIENT_LIGHT 0x41                                      /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_LIGHT_CONSUMER_INFRARED 0x42                                  /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_LIGHT_INFRARED_LIGHT 0x43                                     /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_LIGHT_VISIBLE_LIGHT 0x44                                      /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_LIGHT_ULTRAVIOLET_LIGHT 0x45                                  /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_LOCATION 0x50                                                 /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_LOCATION_BROADCAST 0x51                                       /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_LOCATION_DEAD_RECKONING 0x52                                  /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_LOCATION_GPS_GLOBAL_POSITIONING_SYSTEM 0x53                   /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_LOCATION_LOOKUP 0x54                                          /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_LOCATION_OTHER 0x55                                           /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_LOCATION_STATIC 0x56                                          /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_LOCATION_TRIANGULATION 0x57                                   /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MECHANICAL 0x60                                               /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MECHANICAL_BOOLEAN_SWITCH 0x61                                /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MECHANICAL_BOOLEAN_SWITCH_ARRAY 0x62                          /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MECHANICAL_MULTIVALUE_SWITCH 0x63                             /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MECHANICAL_FORCE 0x64                                         /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MECHANICAL_PRESSURE 0x65                                      /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MECHANICAL_STRAIN 0x66                                        /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MECHANICAL_WEIGHT 0x67                                        /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MECHANICAL_HAPTIC_VIBRATOR 0x68                               /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MECHANICAL_HALL_EFFECT_SWITCH 0x69                            /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MOTION 0x70                                                   /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MOTION_ACCELEROMETER_1D 0x71                                  /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MOTION_ACCELEROMETER_2D 0x72                                  /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MOTION_ACCELEROMETER_3D 0x73                                  /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MOTION_GYROMETER_1D 0x74                                      /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MOTION_GYROMETER_2D 0x75                                      /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MOTION_GYROMETER_3D 0x76                                      /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MOTION_MOTION_DETECTOR 0x77                                   /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MOTION_SPEEDOMETER 0x78                                       /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MOTION_ACCELEROMETER 0x79                                     /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MOTION_GYROMETER 0x7A                                         /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MOTION_GRAVITY_VECTOR 0x7B                                    /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_MOTION_LINEAR_ACCELEROMETER 0x7C                              /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION 0x80                                              /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_COMPASS_1D 0x81                                   /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_COMPASS_2D 0x82                                   /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_COMPASS_3D 0x83                                   /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_INCLINOMETER_1D 0x84                              /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_INCLINOMETER_2D 0x85                              /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_INCLINOMETER_3D 0x86                              /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_DISTANCE_1D 0x87                                  /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_DISTANCE_2D 0x88                                  /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_DISTANCE_3D 0x89                                  /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_DEVICE_ORIENTATION 0x8A                           /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_COMPASS 0x8B                                      /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_INCLINOMETER 0x8C                                 /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_DISTANCE 0x8D                                     /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_RELATIVE_ORIENTATION 0x8E                         /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_SIMPLE_ORIENTATION 0x8F                           /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_SCANNER 0x90                                                  /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_SCANNER_BARCODE 0x91                                          /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_SCANNER_RFID 0x92                                             /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_SCANNER_NFC 0x93                                              /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_TIME 0xA0                                                     /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_TIME_ALARM_TIMER 0xA1                                         /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_TIME_REAL_TIME_CLOCK 0xA2                                     /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_PERSONAL_ACTIVITY 0xB0                                        /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_PERSONAL_ACTIVITY_ACTIVITY_DETECTION 0xB1                     /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_PERSONAL_ACTIVITY_DEVICE_POSITION 0xB2                        /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_PERSONAL_ACTIVITY_PEDOMETER 0xB3                              /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_PERSONAL_ACTIVITY_STEP_DETECTION 0xB4                         /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_EXTENDED 0xC0                                     /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_EXTENDED_GEOMAGNETIC_ORIENTATION 0xC1             /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_ORIENTATION_EXTENDED_MAGNETOMETER 0xC2                        /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_GESTURE 0xD0                                                  /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_GESTURE_CHASSIS_FLIP_GESTURE 0xD1                             /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_GESTURE_HINGE_FOLD_GESTURE 0xD2                               /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_OTHER 0xE0                                                    /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_OTHER_CUSTOM 0xE1                                             /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_OTHER_GENERIC 0xE2                                            /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_OTHER_GENERIC_ENUMERATOR 0xE3                                 /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_OTHER_HINGE_ANGLE 0xE4                                        /* Usage type:  CA, CP */
#define USB_HID_USAGE_SENSORS_EVENT 0x200                                                   /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_EVENT_SENSOR_STATE 0x201                                      /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_EVENT_SENSOR_EVENT 0x202                                      /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_PROPERTY 0x300                                                /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_FRIENDLY_NAME 0x301                                  /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_PERSISTENT_UNIQUE_ID 0x302                           /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_SENSOR_STATUS 0x303                                  /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_MINIMUM_REPORT_INTERVAL 0x304                        /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_SENSOR_MANUFACTURER 0x305                            /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_SENSOR_MODEL 0x306                                   /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_SENSOR_SERIAL_NUMBER 0x307                           /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_SENSOR_DESCRIPTION 0x308                             /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_SENSOR_CONNECTION_TYPE 0x309                         /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_PROPERTY_SENSOR_DEVICE_PATH 0x30A                             /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_HARDWARE_REVISION 0x30B                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_FIRMWARE_VERSION 0x30C                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_RELEASE_DATE 0x30D                                   /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_REPORT_INTERVAL 0x30E                                /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CHANGE_SENSITIVITY_ABSOLUTE 0x30F                    /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CHANGE_SENSITIVITY_PERCENT_OF_RANGE 0x310            /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CHANGE_SENSITIVITY_PERCENT_RELATIVE 0x311            /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_ACCURACY 0x312                                       /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_RESOLUTION 0x313                                     /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_MAXIMUM 0x314                                        /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_MINIMUM 0x315                                        /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_REPORTING_STATE 0x316                                /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_PROPERTY_SAMPLING_RATE 0x317                                  /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_RESPONSE_CURVE 0x318                                 /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_POWER_STATE 0x319                                    /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_PROPERTY_MAXIMUM_FIFO_EVENTS 0x31A                            /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_REPORT_LATENCY 0x31B                                 /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_FLUSH_FIFO_EVENTS 0x31C                              /* Usage type:  DF */
#define USB_HID_USAGE_SENSORS_PROPERTY_MAXIMUM_POWER_CONSUMPTION 0x31D                      /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_IS_PRIMARY 0x31E                                     /* Usage type:  DF */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_LOCATION 0x400                                     /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ALTITUDE_ANTENNA_SEA_LEVEL 0x402                   /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_DIFFERENTIAL_REFERENCE_STATION_ID 0x403            /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ALTITUDE_ELLIPSOID_ERROR 0x404                     /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ALTITUDE_ELLIPSOID 0x405                           /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ALTITUDE_SEA_LEVEL_ERROR 0x406                     /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ALTITUDE_SEA_LEVEL 0x407                           /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_DIFFERENTIAL_GPS_DATA_AGE 0x408                    /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ERROR_RADIUS 0x409                                 /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_FIX_QUALITY 0x40A                                  /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_FIX_TYPE 0x40B                                     /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GEOIDAL_SEPARATION 0x40C                           /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GPS_OPERATION_MODE 0x40D                           /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GPS_SELECTION_MODE 0x40E                           /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GPS_STATUS 0x40F                                   /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_POSITION_DILUTION_OF_PRECISION 0x410               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HORIZONTAL_DILUTION_OF_PRECISION 0x411             /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_VERTICAL_DILUTION_OF_PRECISION 0x412               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_LATITUDE 0x413                                     /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_LONGITUDE 0x414                                    /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_TRUE_HEADING 0x415                                 /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MAGNETIC_HEADING 0x416                             /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MAGNETIC_VARIATION 0x417                           /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_SPEED 0x418                                        /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_SATELLITES_IN_VIEW 0x419                           /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_SATELLITES_IN_VIEW_AZIMUTH 0x41A                   /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_SATELLITES_IN_VIEW_ELEVATION 0x41B                 /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_SATELLITES_IN_VIEW_IDS 0x41C                       /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_SATELLITES_IN_VIEW_PRNS 0x41D                      /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_SATELLITES_IN_VIEW_S_N_RATIOS 0x41E                /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_SATELLITES_USED_COUNT 0x41F                        /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_SATELLITES_USED_PRNS 0x420                         /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_NMEA_SENTENCE 0x421                                /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ADDRESS_LINE_1 0x422                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ADDRESS_LINE_2 0x423                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CITY 0x424                                         /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_STATE_OR_PROVINCE 0x425                            /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_COUNTRY_OR_REGION 0x426                            /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_POSTAL_CODE 0x427                                  /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_LOCATION 0x42A                                       /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_LOCATION_DESIRED_ACCURACY 0x42B                      /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ENVIRONMENTAL 0x430                                /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ATMOSPHERIC_PRESSURE 0x431                         /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_RELATIVE_HUMIDITY 0x433                            /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_TEMPERATURE 0x434                                  /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_WIND_DIRECTION 0x435                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_WIND_SPEED 0x436                                   /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_AIR_QUALITY_INDEX 0x437                            /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_EQUIVALENT_CO2 0x438                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_VOLATILE_ORGANIC_COMPOUND_CONCENTRATION 0x439      /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_OBJECT_PRESENCE 0x43A                              /* Usage type:  SF */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_OBJECT_PROXIMITY_RANGE 0x43B                       /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_OBJECT_PROXIMITY_OUT_OF_RANGE 0x43C                /* Usage type:  SF */
#define USB_HID_USAGE_SENSORS_PROPERTY_ENVIRONMENTAL 0x440                                  /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_REFERENCE_PRESSURE 0x441                             /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MOTION 0x450                                       /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MOTION_STATE 0x451                                 /* Usage type:  SF */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ACCELERATION 0x452                                 /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ACCELERATION_AXIS_X 0x453                          /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ACCELERATION_AXIS_Y 0x454                          /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ACCELERATION_AXIS_Z 0x455                          /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ANGULAR_VELOCITY 0x456                             /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ANGULAR_VELOCITY_ABOUT_X_AXIS 0x457                /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ANGULAR_VELOCITY_ABOUT_Y_AXIS 0x458                /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ANGULAR_VELOCITY_ABOUT_Z_AXIS 0x459                /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ANGULAR_POSITION 0x45A                             /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ANGULAR_POSITION_ABOUT_X_AXIS 0x45B                /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ANGULAR_POSITION_ABOUT_Y_AXIS 0x45C                /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ANGULAR_POSITION_ABOUT_Z_AXIS 0x45D                /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MOTION_SPEED 0x45E                                 /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MOTION_INTENSITY 0x45F                             /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ORIENTATION 0x470                                  /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HEADING 0x471                                      /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HEADING_X_AXIS 0x472                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HEADING_Y_AXIS 0x473                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HEADING_Z_AXIS 0x474                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HEADING_COMPENSATED_MAGNETIC_NORTH 0x475           /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HEADING_COMPENSATED_TRUE_NORTH 0x476               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HEADING_MAGNETIC_NORTH 0x477                       /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HEADING_TRUE_NORTH 0x478                           /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_DISTANCE 0x479                                     /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_DISTANCE_X_AXIS 0x47A                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_DISTANCE_Y_AXIS 0x47B                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_DISTANCE_Z_AXIS 0x47C                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_DISTANCE_OUT_OF_RANGE 0x47D                        /* Usage type:  SF */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_TILT 0x47E                                         /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_TILT_X_AXIS 0x47F                                  /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_TILT_Y_AXIS 0x480                                  /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_TILT_Z_AXIS 0x481                                  /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ROTATION_MATRIX 0x482                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_QUATERNION 0x483                                   /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MAGNETIC_FLUX 0x484                                /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MAGNETIC_FLUX_X_AXIS 0x485                         /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MAGNETIC_FLUX_Y_AXIS 0x486                         /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MAGNETIC_FLUX_Z_AXIS 0x487                         /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MAGNETOMETER_ACCURACY 0x488                        /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_SIMPLE_ORIENTATION_DIRECTION 0x489                 /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MECHANICAL 0x490                                   /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_BOOLEAN_SWITCH_STATE 0x491                         /* Usage type:  SF */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_BOOLEAN_SWITCH_ARRAY_STATES 0x492                  /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MULTIVALUE_SWITCH_VALUE 0x493                      /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_FORCE 0x494                                        /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ABSOLUTE_PRESSURE 0x495                            /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GAUGE_PRESSURE 0x496                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_STRAIN 0x497                                       /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_WEIGHT 0x498                                       /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_MECHANICAL 0x4A0                                     /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_VIBRATION_STATE 0x4A1                                /* Usage type:  DF */
#define USB_HID_USAGE_SENSORS_PROPERTY_FORWARD_VIBRATION_SPEED 0x4A2                        /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_BACKWARD_VIBRATION_SPEED 0x4A3                       /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_BIOMETRIC 0x4B0                                    /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HUMAN_PRESENCE 0x4B1                               /* Usage type:  SF */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HUMAN_PROXIMITY_RANGE 0x4B2                        /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HUMAN_PROXIMITY_OUT_OF_RANGE 0x4B3                 /* Usage type:  SF */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HUMAN_TOUCH_STATE 0x4B4                            /* Usage type:  SF */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_BLOOD_PRESSURE 0x4B5                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_BLOOD_PRESSURE_DIASTOLIC 0x4B6                     /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_BLOOD_PRESSURE_SYSTOLIC 0x4B7                      /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HEART_RATE 0x4B8                                   /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_RESTING_HEART_RATE 0x4B9                           /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HEARTBEAT_INTERVAL 0x4BA                           /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_RESPIRATORY_RATE 0x4BB                             /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_SPO2 0x4BC                                         /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_LIGHT 0x4D0                                        /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ILLUMINANCE 0x4D1                                  /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_COLOR_TEMPERATURE 0x4D2                            /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CHROMATICITY 0x4D3                                 /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CHROMATICITY_X 0x4D4                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CHROMATICITY_Y 0x4D5                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CONSUMER_IR_SENTENCE_RECEIVE 0x4D6                 /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_INFRARED_LIGHT 0x4D7                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_RED_LIGHT 0x4D8                                    /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GREEN_LIGHT 0x4D9                                  /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_BLUE_LIGHT 0x4DA                                   /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ULTRAVIOLET_A_LIGHT 0x4DB                          /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ULTRAVIOLET_B_LIGHT 0x4DC                          /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ULTRAVIOLET_INDEX 0x4DD                            /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_NEAR_INFRARED_LIGHT 0x4DE                          /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_LIGHT 0x4DF                                          /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CONSUMER_IR_SENTENCE_SEND 0x4E0                      /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_AUTO_BRIGHTNESS_PREFERRED 0x4E2                      /* Usage type:  DF */
#define USB_HID_USAGE_SENSORS_PROPERTY_AUTO_COLOR_PREFERRED 0x4E3                           /* Usage type:  DF */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_SCANNER 0x4F0                                      /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_RFID_TAG_40_BIT 0x4F1                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_NFC_SENTENCE_RECEIVE 0x4F2                         /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_SCANNER 0x4F8                                        /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_NFC_SENTENCE_SEND 0x4F9                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ELECTRICAL 0x500                                   /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CAPACITANCE 0x501                                  /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CURRENT 0x502                                      /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ELECTRICAL_POWER 0x503                             /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_INDUCTANCE 0x504                                   /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_RESISTANCE 0x505                                   /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_VOLTAGE 0x506                                      /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_FREQUENCY 0x507                                    /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_PERIOD 0x508                                       /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_PERCENT_OF_RANGE 0x509                             /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_TIME 0x520                                         /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_YEAR 0x521                                         /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MONTH 0x522                                        /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_DAY 0x523                                          /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_DAY_OF_WEEK 0x524                                  /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HOUR 0x525                                         /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MINUTE 0x526                                       /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_SECOND 0x527                                       /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_MILLISECOND 0x528                                  /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_TIMESTAMP 0x529                                    /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_JULIAN_DAY_OF_YEAR 0x52A                           /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_TIME_SINCE_SYSTEM_BOOT 0x52B                       /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_TIME 0x530                                           /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_TIME_ZONE_OFFSET_FROM_UTC 0x531                      /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_TIME_ZONE_NAME 0x532                                 /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_DAYLIGHT_SAVINGS_TIME_OBSERVED 0x533                 /* Usage type:  DF */
#define USB_HID_USAGE_SENSORS_PROPERTY_TIME_TRIM_ADJUSTMENT 0x534                           /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_ARM_ALARM 0x535                                      /* Usage type:  DF */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM 0x540                                       /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_USAGE 0x541                                 /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_BOOLEAN_ARRAY 0x542                         /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE 0x543                                 /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_1 0x544                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_2 0x545                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_3 0x546                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_4 0x547                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_5 0x548                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_6 0x549                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_7 0x54A                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_8 0x54B                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_9 0x54C                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_10 0x54D                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_11 0x54E                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_12 0x54F                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_13 0x550                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_14 0x551                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_15 0x552                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_16 0x553                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_17 0x554                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_18 0x555                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_19 0x556                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_20 0x557                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_21 0x558                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_22 0x559                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_23 0x55A                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_24 0x55B                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_25 0x55C                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_26 0x55D                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_27 0x55E                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_VALUE_28 0x55F                              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC 0x560                                      /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_GUID_OR_PROPERTYKEY 0x561                  /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_CATEGORY_GUID 0x562                        /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_TYPE_GUID 0x563                            /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_EVENT_PROPERTYKEY 0x564                    /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_PROPERTY_PROPERTYKEY 0x565                 /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_DATA_FIELD_PROPERTYKEY 0x566               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_EVENT 0x567                                /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_PROPERTY 0x568                             /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_DATA_FIELD 0x569                           /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ENUMERATOR_TABLE_ROW_INDEX 0x56A                   /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ENUMERATOR_TABLE_ROW_COUNT 0x56B                   /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_GUID_OR_PROPERTYKEY_KIND 0x56C             /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_GUID 0x56D                                 /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_PROPERTYKEY 0x56E                          /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_TOP_LEVEL_COLLECTION_ID 0x56F              /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_REPORT_ID 0x570                            /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_REPORT_ITEM_POSITION_INDEX 0x571           /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_FIRMWARE_VARTYPE 0x572                     /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_UNIT_OF_MEASURE 0x573                      /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_UNIT_EXPONENT 0x574                        /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_REPORT_SIZE 0x575                          /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GENERIC_REPORT_COUNT 0x576                         /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_GENERIC 0x580                                        /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_ENUMERATOR_TABLE_ROW_INDEX 0x581                     /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_ENUMERATOR_TABLE_ROW_COUNT 0x582                     /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_PERSONAL_ACTIVITY 0x590                            /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ACTIVITY_TYPE 0x591                                /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_ACTIVITY_STATE 0x592                               /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_DEVICE_POSITION 0x593                              /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_STEP_COUNT 0x594                                   /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_STEP_COUNT_RESET 0x595                             /* Usage type:  DF */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_STEP_DURATION 0x596                                /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_STEP_TYPE 0x597                                    /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_PROPERTY_MINIMUM_ACTIVITY_DETECTION_INTERVAL 0x5A0            /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_SUPPORTED_ACTIVITY_TYPES 0x5A1                       /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_PROPERTY_SUBSCRIBED_ACTIVITY_TYPES 0x5A2                      /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_PROPERTY_SUPPORTED_STEP_TYPES 0x5A3                           /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_PROPERTY_SUBSCRIBED_STEP_TYPES 0x5A4                          /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_PROPERTY_FLOOR_HEIGHT 0x5A5                                   /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_CUSTOM_TYPE_ID 0x5B0                               /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM 0x5C0                                         /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_1 0x5C1                                 /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_2 0x5C2                                 /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_3 0x5C3                                 /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_4 0x5C4                                 /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_5 0x5C5                                 /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_6 0x5C6                                 /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_7 0x5C7                                 /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_8 0x5C8                                 /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_9 0x5C9                                 /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_10 0x5CA                                /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_11 0x5CB                                /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_12 0x5CC                                /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_13 0x5CD                                /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_14 0x5CE                                /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_15 0x5CF                                /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_PROPERTY_CUSTOM_VALUE_16 0x5D0                                /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HINGE 0x5E0                                        /* Usage type:  SV, DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HINGE_ANGLE 0x5E1                                  /* Usage type:  SV, DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GESTURE_SENSOR 0x5F0                               /* Usage type:  DV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_GESTURE_STATE 0x5F1                                /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HINGE_FOLD_INITIAL_ANGLE 0x5F2                     /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HINGE_FOLD_FINAL_ANGLE 0x5F3                       /* Usage type:  SV */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HINGE_FOLD_CONTRIBUTING_PANEL 0x5F4                /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_DATA_FIELD_HINGE_FOLD_TYPE 0x5F5                              /* Usage type:  NAry */
#define USB_HID_USAGE_SENSORS_SENSOR_STATE_UNDEFINED 0x800                                  /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_STATE_READY 0x801                                      /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_STATE_NOT_AVAILABLE 0x802                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_STATE_NO_DATA 0x803                                    /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_STATE_INITIALIZING 0x804                               /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_STATE_ACCESS_DENIED 0x805                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_STATE_ERROR 0x806                                      /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_UNKNOWN 0x810                                    /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_STATE_CHANGED 0x811                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_PROPERTY_CHANGED 0x812                           /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_DATA_UPDATED 0x813                               /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_POLL_RESPONSE 0x814                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_CHANGE_SENSITIVITY 0x815                         /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_RANGE_MAXIMUM_REACHED 0x816                      /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_RANGE_MINIMUM_REACHED 0x817                      /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_HIGH_THRESHOLD_CROSS_UPWARD 0x818                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_HIGH_THRESHOLD_CROSS_DOWNWARD 0x819              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_LOW_THRESHOLD_CROSS_UPWARD 0x81A                 /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_LOW_THRESHOLD_CROSS_DOWNWARD 0x81B               /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_ZERO_THRESHOLD_CROSS_UPWARD 0x81C                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_ZERO_THRESHOLD_CROSS_DOWNWARD 0x81D              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_PERIOD_EXCEEDED 0x81E                            /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_FREQUENCY_EXCEEDED 0x81F                         /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SENSOR_EVENT_COMPLEX_TRIGGER 0x820                            /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_CONNECTION_TYPE_PC_INTEGRATED 0x830                           /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_CONNECTION_TYPE_PC_ATTACHED 0x831                             /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_CONNECTION_TYPE_PC_EXTERNAL 0x832                             /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_REPORTING_STATE_REPORT_NO_EVENTS 0x840                        /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_REPORTING_STATE_REPORT_ALL_EVENTS 0x841                       /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_REPORTING_STATE_REPORT_THRESHOLD_EVENTS 0x842                 /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_REPORTING_STATE_WAKE_ON_NO_EVENTS 0x843                       /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_REPORTING_STATE_WAKE_ON_ALL_EVENTS 0x844                      /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_REPORTING_STATE_WAKE_ON_THRESHOLD_EVENTS 0x845                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_POWER_STATE_UNDEFINED 0x850                                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_POWER_STATE_D0_FULL_POWER 0x851                               /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_POWER_STATE_D1_LOW_POWER 0x852                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_POWER_STATE_D2_STANDBY_POWER_WITH_WAKEUP 0x853                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_POWER_STATE_D3_SLEEP_WITH_WAKEUP 0x854                        /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_POWER_STATE_D4_POWER_OFF 0x855                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_FIX_QUALITY_NO_FIX 0x870                                      /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_FIX_QUALITY_GPS 0x871                                         /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_FIX_QUALITY_DGPS 0x872                                        /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_FIX_TYPE_NO_FIX 0x880                                         /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_FIX_TYPE_GPS_SPS_MODE_FIX_VALID 0x881                         /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_FIX_TYPE_DGPS_SPS_MODE_FIX_VALID 0x882                        /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_FIX_TYPE_GPS_PPS_MODE_FIX_VALID 0x883                         /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_FIX_TYPE_REAL_TIME_KINEMATIC 0x884                            /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_FIX_TYPE_FLOAT_RTK 0x885                                      /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_FIX_TYPE_ESTIMATED_DEAD_RECKONED 0x886                        /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_FIX_TYPE_MANUAL_INPUT_MODE 0x887                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_FIX_TYPE_SIMULATOR_MODE 0x888                                 /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_GPS_OPERATION_MODE_MANUAL 0x890                               /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_GPS_OPERATION_MODE_AUTOMATIC 0x891                            /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_GPS_SELECTION_MODE_AUTONOMOUS 0x8A0                           /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_GPS_SELECTION_MODE_DGPS 0x8A1                                 /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_GPS_SELECTION_MODE_ESTIMATED_DEAD_RECKONED 0x8A2              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_GPS_SELECTION_MODE_MANUAL_INPUT 0x8A3                         /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_GPS_SELECTION_MODE_SIMULATOR 0x8A4                            /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_GPS_SELECTION_MODE_DATA_NOT_VALID 0x8A5                       /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_GPS_STATUS_DATA_VALID 0x8B0                                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_GPS_STATUS_DATA_NOT_VALID 0x8B1                               /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_ACCURACY_DEFAULT 0x860                                        /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_ACCURACY_HIGH 0x861                                           /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_ACCURACY_MEDIUM 0x862                                         /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_ACCURACY_LOW 0x863                                            /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_DAY_OF_WEEK_SUNDAY 0x8C0                                      /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_DAY_OF_WEEK_MONDAY 0x8C1                                      /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_DAY_OF_WEEK_TUESDAY 0x8C2                                     /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_DAY_OF_WEEK_WEDNESDAY 0x8C3                                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_DAY_OF_WEEK_THURSDAY 0x8C4                                    /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_DAY_OF_WEEK_FRIDAY 0x8C5                                      /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_DAY_OF_WEEK_SATURDAY 0x8C6                                    /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_KIND_CATEGORY 0x8D0                                           /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_KIND_TYPE 0x8D1                                               /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_KIND_EVENT 0x8D2                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_KIND_PROPERTY 0x8D3                                           /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_KIND_DATA_FIELD 0x8D4                                         /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_MAGNETOMETER_ACCURACY_LOW 0x8E0                               /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_MAGNETOMETER_ACCURACY_MEDIUM 0x8E1                            /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_MAGNETOMETER_ACCURACY_HIGH 0x8E2                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SIMPLE_ORIENTATION_DIRECTION_NOT_ROTATED 0x8F0                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SIMPLE_ORIENTATION_DIRECTION_ROTATED_90_DEGREES_CCW 0x8F1     /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SIMPLE_ORIENTATION_DIRECTION_ROTATED_180_DEGREES_CCW 0x8F2    /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SIMPLE_ORIENTATION_DIRECTION_ROTATED_270_DEGREES_CCW 0x8F3    /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SIMPLE_ORIENTATION_DIRECTION_FACE_UP 0x8F4                    /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_SIMPLE_ORIENTATION_DIRECTION_FACE_DOWN 0x8F5                  /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_NULL 0x900                                                 /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_BOOL 0x901                                                 /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_UI1 0x902                                                  /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_I1 0x903                                                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_UI2 0x904                                                  /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_I2 0x905                                                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_UI4 0x906                                                  /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_I4 0x907                                                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_UI8 0x908                                                  /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_I8 0x909                                                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_R4 0x90A                                                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_R8 0x90B                                                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_WSTR 0x90C                                                 /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_STR 0x90D                                                  /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_CLSID 0x90E                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_VECTOR_VT_UI1 0x90F                                        /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16E0 0x910                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16E1 0x911                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16E2 0x912                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16E3 0x913                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16E4 0x914                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16E5 0x915                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16E6 0x916                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16E7 0x917                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16E8 0x918                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16E9 0x919                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16EA 0x91A                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16EB 0x91B                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16EC 0x91C                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16ED 0x91D                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16EE 0x91E                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F16EF 0x91F                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32E0 0x920                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32E1 0x921                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32E2 0x922                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32E3 0x923                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32E4 0x924                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32E5 0x925                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32E6 0x926                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32E7 0x927                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32E8 0x928                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32E9 0x929                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32EA 0x92A                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32EB 0x92B                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32EC 0x92C                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32ED 0x92D                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32EE 0x92E                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_VT_F32EF 0x92F                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_ACTIVITY_TYPE_UNKNOWN 0x930                                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_ACTIVITY_TYPE_STATIONARY 0x931                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_ACTIVITY_TYPE_FIDGETING 0x932                                 /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_ACTIVITY_TYPE_WALKING 0x933                                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_ACTIVITY_TYPE_RUNNING 0x934                                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_ACTIVITY_TYPE_IN_VEHICLE 0x935                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_ACTIVITY_TYPE_BIKING 0x936                                    /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_ACTIVITY_TYPE_IDLE 0x937                                      /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_NOT_SPECIFIED 0x940                                      /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_LUX 0x941                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_DEGREES_KELVIN 0x942                                     /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_DEGREES_CELSIUS 0x943                                    /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_PASCAL 0x944                                             /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_NEWTON 0x945                                             /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_METERS_SECOND 0x946                                      /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_KILOGRAM 0x947                                           /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_METER 0x948                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_METERS_SECOND_SECOND 0x949                               /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_FARAD 0x94A                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_AMPERE 0x94B                                             /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_WATT 0x94C                                               /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_HENRY 0x94D                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_OHM 0x94E                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_VOLT 0x94F                                               /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_HERTZ 0x950                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_BAR 0x951                                                /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_DEGREES_ANTI_CLOCKWISE 0x952                             /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_DEGREES_CLOCKWISE 0x953                                  /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_DEGREES 0x954                                            /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_DEGREES_SECOND 0x955                                     /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_DEGREES_SECOND_SECOND 0x956                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_KNOT 0x957                                               /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_PERCENT 0x958                                            /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_SECOND 0x959                                             /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_MILLISECOND 0x95A                                        /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_G 0x95B                                                  /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_BYTES 0x95C                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_MILLIGAUSS 0x95D                                         /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_UNIT_BITS 0x95E                                               /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_ACTIVITY_STATE_NO_STATE_CHANGE 0x960                          /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_ACTIVITY_STATE_START_ACTIVITY 0x961                           /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_ACTIVITY_STATE_END_ACTIVITY 0x962                             /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_0 0x970                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_1 0x971                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_2 0x972                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_3 0x973                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_4 0x974                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_5 0x975                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_6 0x976                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_7 0x977                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_8 0x978                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_9 0x979                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_A 0x97A                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_B 0x97B                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_C 0x97C                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_D 0x97D                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_E 0x97E                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_EXPONENT_F 0x97F                                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_DEVICE_POSITION_UNKNOWN 0x980                                 /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_DEVICE_POSITION_UNCHANGED 0x981                               /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_DEVICE_POSITION_ON_DESK 0x982                                 /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_DEVICE_POSITION_IN_HAND 0x983                                 /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_DEVICE_POSITION_MOVING_IN_BAG 0x984                           /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_DEVICE_POSITION_STATIONARY_IN_BAG 0x985                       /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_STEP_TYPE_UNKNOWN 0x990                                       /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_STEP_TYPE_RUNNING 0x991                                       /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_STEP_TYPE_WALKING 0x992                                       /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_GESTURE_STATE_UNKNOWN 0x9A0                                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_GESTURE_STATE_STARTED 0x9A1                                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_GESTURE_STATE_COMPLETED 0x9A2                                 /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_GESTURE_STATE_CANCELLED 0x9A3                                 /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_HINGE_FOLD_CONTRIBUTING_PANEL_UNKNOWN 0x9B0                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_HINGE_FOLD_CONTRIBUTING_PANEL_PANEL_1 0x9B1                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_HINGE_FOLD_CONTRIBUTING_PANEL_PANEL_2 0x9B2                   /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_HINGE_FOLD_CONTRIBUTING_PANEL_BOTH 0x9B3                      /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_HINGE_FOLD_TYPE_UNKNOWN 0x9B4                                 /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_HINGE_FOLD_TYPE_INCREASING 0x9B5                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_HINGE_FOLD_TYPE_DECREASING 0x9B6                              /* Usage type:  Sel */
#define USB_HID_USAGE_SENSORS_MODIFIER_CHANGE_SENSITIVITY_ABSOLUTE 0x1000                   /* Usage type:  US */
#define USB_HID_USAGE_SENSORS_MODIFIER_MAXIMUM 0x2000                                       /* Usage type:  US */
#define USB_HID_USAGE_SENSORS_MODIFIER_MINIMUM 0x3000                                       /* Usage type:  US */
#define USB_HID_USAGE_SENSORS_MODIFIER_ACCURACY 0x4000                                      /* Usage type:  US */
#define USB_HID_USAGE_SENSORS_MODIFIER_RESOLUTION 0x5000                                    /* Usage type:  US */
#define USB_HID_USAGE_SENSORS_MODIFIER_THRESHOLD_HIGH 0x6000                                /* Usage type:  US */
#define USB_HID_USAGE_SENSORS_MODIFIER_THRESHOLD_LOW 0x7000                                 /* Usage type:  US */
#define USB_HID_USAGE_SENSORS_MODIFIER_CALIBRATION_OFFSET 0x8000                            /* Usage type:  US */
#define USB_HID_USAGE_SENSORS_MODIFIER_CALIBRATION_MULTIPLIER 0x9000                        /* Usage type:  US */
#define USB_HID_USAGE_SENSORS_MODIFIER_REPORT_INTERVAL 0xA000                               /* Usage type:  US */
#define USB_HID_USAGE_SENSORS_MODIFIER_FREQUENCY_MAX 0xB000                                 /* Usage type:  US */
#define USB_HID_USAGE_SENSORS_MODIFIER_PERIOD_MAX 0xC000                                    /* Usage type:  US */
#define USB_HID_USAGE_SENSORS_MODIFIER_CHANGE_SENSITIVITY_PERCENT_OF_RANGE 0xD000           /* Usage type:  US */
#define USB_HID_USAGE_SENSORS_MODIFIER_CHANGE_SENSITIVITY_PERCENT_RELATIVE 0xE000           /* Usage type:  US */
/** @} */

/**
 * @name Page 0x40: Medical Instrument
 * @{
 */
#define USB_HID_USAGE_MEDICAL_UNDEFINED 0x00
#define USB_HID_USAGE_MEDICAL_MEDICAL_ULTRASOUND 0x01               /* Usage type:  CA */
#define USB_HID_USAGE_MEDICAL_VCR_ACQUISITION 0x20                  /* Usage type:  OOC */
#define USB_HID_USAGE_MEDICAL_FREEZE_THAW 0x21                      /* Usage type:  OOC */
#define USB_HID_USAGE_MEDICAL_CLIP_STORE 0x22                       /* Usage type:  OSC */
#define USB_HID_USAGE_MEDICAL_UPDATE 0x23                           /* Usage type:  OSC */
#define USB_HID_USAGE_MEDICAL_NEXT 0x24                             /* Usage type:  OSC */
#define USB_HID_USAGE_MEDICAL_SAVE 0x25                             /* Usage type:  OSC */
#define USB_HID_USAGE_MEDICAL_PRINT 0x26                            /* Usage type:  OSC */
#define USB_HID_USAGE_MEDICAL_MICROPHONE_ENABLE 0x27                /* Usage type:  OSC */
#define USB_HID_USAGE_MEDICAL_CINE 0x40                             /* Usage type:  LC */
#define USB_HID_USAGE_MEDICAL_TRANSMIT_POWER 0x41                   /* Usage type:  LC */
#define USB_HID_USAGE_MEDICAL_VOLUME 0x42                           /* Usage type:  LC */
#define USB_HID_USAGE_MEDICAL_FOCUS 0x43                            /* Usage type:  LC */
#define USB_HID_USAGE_MEDICAL_DEPTH 0x44                            /* Usage type:  LC */
#define USB_HID_USAGE_MEDICAL_SOFT_STEP_MINUS_PRIMARY 0x60          /* Usage type:  LC */
#define USB_HID_USAGE_MEDICAL_SOFT_STEP_MINUS_SECONDARY 0x61        /* Usage type:  LC */
#define USB_HID_USAGE_MEDICAL_DEPTH_GAIN_COMPENSATION 0x70          /* Usage type:  LC */
#define USB_HID_USAGE_MEDICAL_ZOOM_SELECT 0x80                      /* Usage type:  OSC */
#define USB_HID_USAGE_MEDICAL_ZOOM_ADJUST 0x81                      /* Usage type:  LC */
#define USB_HID_USAGE_MEDICAL_SPECTRAL_DOPPLER_MODE_SELECT 0x82     /* Usage type:  OSC */
#define USB_HID_USAGE_MEDICAL_SPECTRAL_DOPPLER_ADJUST 0x83          /* Usage type:  LC */
#define USB_HID_USAGE_MEDICAL_COLOR_DOPPLER_MODE_SELECT 0x84        /* Usage type:  OSC */
#define USB_HID_USAGE_MEDICAL_COLOR_DOPPLER_ADJUST 0x85             /* Usage type:  LC */
#define USB_HID_USAGE_MEDICAL_MOTION_MODE_SELECT 0x86               /* Usage type:  OSC */
#define USB_HID_USAGE_MEDICAL_MOTION_MODE_ADJUST 0x87               /* Usage type:  LC */
#define USB_HID_USAGE_MEDICAL_2_D_MODE_SELECT 0x88                  /* Usage type:  OSC */
#define USB_HID_USAGE_MEDICAL_2_D_MODE_ADJUST 0x89                  /* Usage type:  LC */
#define USB_HID_USAGE_MEDICAL_SOFT_CONTROL_SELECT 0xA0              /* Usage type:  OSC */
#define USB_HID_USAGE_MEDICAL_SOFT_CONTROL_ADJUST 0xA1              /* Usage type:  LC */
/** @} */

/**
 * @name Page 0x41: Braille Display
 * @{
 */
#define USB_HID_USAGE_BRAILLE_UNDEFINED 0x00
#define USB_HID_USAGE_BRAILLE_BRAILLE_DISPLAY 0x01                  /* Usage type:  CA */
#define USB_HID_USAGE_BRAILLE_BRAILLE_ROW 0x02                      /* Usage type:  NAry */
#define USB_HID_USAGE_BRAILLE_8_DOT_BRAILLE_CELL 0x03               /* Usage type:  DV */
#define USB_HID_USAGE_BRAILLE_6_DOT_BRAILLE_CELL 0x04               /* Usage type:  DV */
#define USB_HID_USAGE_BRAILLE_NUMBER_OF_BRAILLE_CELLS 0x05          /* Usage type:  DV */
#define USB_HID_USAGE_BRAILLE_SCREEN_READER_CONTROL 0x06            /* Usage type:  NAry */
#define USB_HID_USAGE_BRAILLE_SCREEN_READER_IDENTIFIER 0x07         /* Usage type:  DV */
#define USB_HID_USAGE_BRAILLE_ROUTER_SET_1 0xFA                     /* Usage type:  NAry */
#define USB_HID_USAGE_BRAILLE_ROUTER_SET_2 0xFB                     /* Usage type:  NAry */
#define USB_HID_USAGE_BRAILLE_ROUTER_SET_3 0xFC                     /* Usage type:  Nary */
#define USB_HID_USAGE_BRAILLE_ROUTER_KEY 0x100                      /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_ROW_ROUTER_KEY 0x101                  /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_BUTTONS 0x200                 /* Usage type:  NAry */
#define USB_HID_USAGE_BRAILLE_BRAILLE_KEYBOARD_DOT_1 0x201          /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_KEYBOARD_DOT_2 0x202          /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_KEYBOARD_DOT_3 0x203          /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_KEYBOARD_DOT_4 0x204          /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_KEYBOARD_DOT_5 0x205          /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_KEYBOARD_DOT_6 0x206          /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_KEYBOARD_DOT_7 0x207          /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_KEYBOARD_DOT_8 0x208          /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_KEYBOARD_SPACE 0x209          /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_KEYBOARD_LEFT_SPACE 0x20A     /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_KEYBOARD_RIGHT_SPACE 0x20B    /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_FACE_CONTROLS 0x20C           /* Usage type:  NAry */
#define USB_HID_USAGE_BRAILLE_BRAILLE_LEFT_CONTROLS 0x20D           /* Usage type:  NAry */
#define USB_HID_USAGE_BRAILLE_BRAILLE_RIGHT_CONTROLS 0x20E          /* Usage type:  NAry */
#define USB_HID_USAGE_BRAILLE_BRAILLE_TOP_CONTROLS 0x20F            /* Usage type:  NAry */
#define USB_HID_USAGE_BRAILLE_BRAILLE_JOYSTICK_CENTER 0x210         /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_JOYSTICK_UP 0x211             /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_JOYSTICK_DOWN 0x212           /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_JOYSTICK_LEFT 0x213           /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_JOYSTICK_RIGHT 0x214          /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_D_PAD_CENTER 0x215            /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_D_PAD_UP 0x216                /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_D_PAD_DOWN 0x217              /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_D_PAD_LEFT 0x218              /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_D_PAD_RIGHT 0x219             /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_PAN_LEFT 0x21A                /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_PAN_RIGHT 0x21B               /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_ROCKER_UP 0x21C               /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_ROCKER_DOWN 0x21D             /* Usage type:  Sel */
#define USB_HID_USAGE_BRAILLE_BRAILLE_ROCKER_PRESS 0x21E            /* Usage type:  Sel */
/** @} */

/**
 * @name Page 0x59: Lighting And Illumination
 * @{
 */
#define USB_HID_USAGE_LIGHT_UNDEFINED 0x00
#define USB_HID_USAGE_LIGHT_LAMP_ARRAY 0x01                             /* Usage type:  CA */
#define USB_HID_USAGE_LIGHT_LAMP_ARRAY_ATTRIBUTES_REPORT 0x02           /* Usage type:  CL */
#define USB_HID_USAGE_LIGHT_LAMP_COUNT 0x03                             /* Usage type:  SV, DV */
#define USB_HID_USAGE_LIGHT_BOUNDING_BOX_WIDTH_IN_MICROMETERS 0x04      /* Usage type:  SV */
#define USB_HID_USAGE_LIGHT_BOUNDING_BOX_HEIGHT_IN_MICROMETERS 0x05     /* Usage type:  SV */
#define USB_HID_USAGE_LIGHT_BOUNDING_BOX_DEPTH_IN_MICROMETERS 0x06      /* Usage type:  SV */
#define USB_HID_USAGE_LIGHT_LAMP_ARRAY_KIND 0x07                        /* Usage type:  SV */
#define USB_HID_USAGE_LIGHT_MIN_UPDATE_INTERVAL_IN_MICROSECONDS 0x08    /* Usage type:  SV */
#define USB_HID_USAGE_LIGHT_LAMP_ATTRIBUTES_REQUEST_REPORT 0x20         /* Usage type:  CL */
#define USB_HID_USAGE_LIGHT_LAMP_ID 0x21                                /* Usage type:  SV, DV */
#define USB_HID_USAGE_LIGHT_LAMP_ATTRIBUTES_RESPONSE_REPORT 0x22        /* Usage type:  CL */
#define USB_HID_USAGE_LIGHT_POSITION_X_IN_MICROMETERS 0x23              /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_POSITION_Y_IN_MICROMETERS 0x24              /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_POSITION_Z_IN_MICROMETERS 0x25              /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_LAMP_PURPOSES 0x26                          /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_UPDATE_LATENCY_IN_MICROSECONDS 0x27         /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_RED_LEVEL_COUNT 0x28                        /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_GREEN_LEVEL_COUNT 0x29                      /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_BLUE_LEVEL_COUNT 0x2A                       /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_INTENSITY_LEVEL_COUNT 0x2B                  /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_IS_PROGRAMMABLE 0x2C                        /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_INPUT_BINDING 0x2D                          /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_LAMP_MULTI_UPDATE_REPORT 0x50               /* Usage type:  CL */
#define USB_HID_USAGE_LIGHT_RED_UPDATE_CHANNEL 0x51                     /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_GREEN_UPDATE_CHANNEL 0x52                   /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_BLUE_UPDATE_CHANNEL 0x53                    /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_INTENSITY_UPDATE_CHANNEL 0x54               /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_LAMP_UPDATE_FLAGS 0x55                      /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_LAMP_RANGE_UPDATE_REPORT 0x60               /* Usage type:  CL */
#define USB_HID_USAGE_LIGHT_LAMP_ID_START 0x61                          /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_LAMP_ID_END 0x62                            /* Usage type:  DV */
#define USB_HID_USAGE_LIGHT_LAMP_ARRAY_CONTROL_REPORT 0x70              /* Usage type:  CL */
#define USB_HID_USAGE_LIGHT_AUTONOMOUS_MODE 0x71                        /* Usage type:  DV */
/** @} */

/**
 * @name Page 0x80: USB Monitor
 * @{
 */
#define USB_HID_USAGE_MONITOR_MONITOR_CONTROL 0x01
#define USB_HID_USAGE_MONITOR_EDID_INFORMATION 0x02
#define USB_HID_USAGE_MONITOR_VDIF_INFORMATION 0x03
#define USB_HID_USAGE_MONITOR_VESA_VERSION 0x04
/** @} */

/**
 * @name Page 0x82: VESA Virtual Control
 * @{
 */
#define USB_HID_USAGE_MONITOR_VESA_BRIGHTNESS 0x10
#define USB_HID_USAGE_MONITOR_VESA_CONTRAST 0x12
#define USB_HID_USAGE_MONITOR_VESA_RED_VIDEO_GAIN 0x16
#define USB_HID_USAGE_MONITOR_VESA_GREEN_VIDEO_GAIN 0x18
#define USB_HID_USAGE_MONITOR_VESA_BLUE_VIDEO_GAIN 0x1A
#define USB_HID_USAGE_MONITOR_VESA_FOCUS 0x1C
#define USB_HID_USAGE_MONITOR_VESA_HORIZONTAL_POSITION 0x20
#define USB_HID_USAGE_MONITOR_VESA_HORIZONTAL_SIZE 0x22
#define USB_HID_USAGE_MONITOR_VESA_HORIZONTAL_PINCUSHION 0x24
#define USB_HID_USAGE_MONITOR_VESA_HORIZONTAL_PINCUSHION_BALANCE 0x26
#define USB_HID_USAGE_MONITOR_VESA_HORIZONTAL_MISCONVERGENCE 0x28
#define USB_HID_USAGE_MONITOR_VESA_HORIZONTAL_LINEARITY 0x2A
#define USB_HID_USAGE_MONITOR_VESA_HORIZONTAL_LINEARITY_BALANCE 0x2C
#define USB_HID_USAGE_MONITOR_VESA_VERTICAL_POSITION 0x30
#define USB_HID_USAGE_MONITOR_VESA_VERTICAL_SIZE 0x32
#define USB_HID_USAGE_MONITOR_VESA_VERTICAL_PINCUSHION 0x34
#define USB_HID_USAGE_MONITOR_VESA_VERTICAL_PINCUSHION_BALANCE 0x36
#define USB_HID_USAGE_MONITOR_VESA_VERTICAL_MISCONVERGENCE 0x38
#define USB_HID_USAGE_MONITOR_VESA_VERTICAL_LINEARITY 0x3A
#define USB_HID_USAGE_MONITOR_VESA_VERTICAL_LINEARITY_BALANCE 0x3C
#define USB_HID_USAGE_MONITOR_VESA_PARALLELOGRAM_DISTORTION_KEY_BALANCE 0x40
#define USB_HID_USAGE_MONITOR_VESA_TRAPEZOIDAL_DISTORTION_KEY 0x42
#define USB_HID_USAGE_MONITOR_VESA_TILT_ROTATION 0x44
#define USB_HID_USAGE_MONITOR_VESA_TOP_CORNER_DISTORTION_CONTROL 0x46
#define USB_HID_USAGE_MONITOR_VESA_TOP_CORNER_DISTORTION_BALANCE 0x48
#define USB_HID_USAGE_MONITOR_VESA_BOTTOM_CORNER_DISTORTION_CONTROL 0x4A
#define USB_HID_USAGE_MONITOR_VESA_BOTTOM_CORNER_DISTORTION_BALANCE 0x4C
#define USB_HID_USAGE_MONITOR_VESA_HORIZONTAL_MOIR 0x56
#define USB_HID_USAGE_MONITOR_VESA_VERTICAL_MOIR 0x58
#define USB_HID_USAGE_MONITOR_VESA_RED_VIDEO_BLACK_LEVEL 0x6C
#define USB_HID_USAGE_MONITOR_VESA_GREEN_VIDEO_BLACK_LEVEL 0x6E
#define USB_HID_USAGE_MONITOR_VESA_BLUE_VIDEO_BLACK_LEVEL 0x70
#define USB_HID_USAGE_MONITOR_VESA_INPUT_LEVEL_SELECT 0x5E
#define USB_HID_USAGE_MONITOR_VESA_INPUT_SOURCE_SELECT 0x60
#define USB_HID_USAGE_MONITOR_VESA_ON_SCREEN_DISPLAY 0xCA
#define USB_HID_USAGE_MONITOR_VESA_STEREOMODE 0xD4
#define USB_HID_USAGE_MONITOR_VESA_AUTO_SIZE_CENTER 0xA2
#define USB_HID_USAGE_MONITOR_VESA_POLARITY_HORIZONTAL_SYNCHRONIZATION 0xA4
#define USB_HID_USAGE_MONITOR_VESA_POLARITY_VERTICAL_SYNCHRONIZATION 0xA6
#define USB_HID_USAGE_MONITOR_VESA_SYNCHRONIZATION_TYPE 0xA8
#define USB_HID_USAGE_MONITOR_VESA_SCREEN_ORIENTATION 0xAA
#define USB_HID_USAGE_MONITOR_VESA_HORIZONTAL_FREQUENCY 0xAC
#define USB_HID_USAGE_MONITOR_VESA_VERTICAL_FREQUENCY 0xAE
#define USB_HID_USAGE_MONITOR_VESA_DEGAUSS 0x01
#define USB_HID_USAGE_MONITOR_VESA_SETTINGS 0xB0
/** @} */

/**
 * @name Page 0x8C: Bar Code Scanner
 * @{
 */
#define USB_HID_USAGE_POS_BARCODE_UNDEFINED 0x00
#define USB_HID_USAGE_POS_BARCODE_BAR_CODE_BADGE_READER 0x01                                /* Usage type:  CA */
#define USB_HID_USAGE_POS_BARCODE_BAR_CODE_SCANNER 0x02                                     /* Usage type:  CA */
#define USB_HID_USAGE_POS_BARCODE_DUMB_BAR_CODE_SCANNER 0x03                                /* Usage type:  CA */
#define USB_HID_USAGE_POS_BARCODE_CORDLESS_SCANNER_BASE 0x04                                /* Usage type:  CA */
#define USB_HID_USAGE_POS_BARCODE_BAR_CODE_SCANNER_CRADLE 0x05                              /* Usage type:  CA */
#define USB_HID_USAGE_POS_BARCODE_ATTRIBUTE_REPORT 0x10                                     /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_SETTINGS_REPORT 0x11                                      /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_SCANNED_DATA_REPORT 0x12                                  /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_RAW_SCANNED_DATA_REPORT 0x13                              /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_TRIGGER_REPORT 0x14                                       /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_STATUS_REPORT 0x15                                        /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_UPC_EAN_CONTROL_REPORT 0x16                               /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_EAN_2_3_LABEL_CONTROL_REPORT 0x17                         /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_CODE_39_CONTROL_REPORT 0x18                               /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_INTERLEAVED_2_OF_5_CONTROL_REPORT 0x19                    /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_STANDARD_2_OF_5_CONTROL_REPORT 0x1A                       /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_MSI_PLESSEY_CONTROL_REPORT 0x1B                           /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_CODABAR_CONTROL_REPORT 0x1C                               /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_CODE_128_CONTROL_REPORT 0x1D                              /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_MISC_1D_CONTROL_REPORT 0x1E                               /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_2D_CONTROL_REPORT 0x1F                                    /* Usage type:  CL */
#define USB_HID_USAGE_POS_BARCODE_AIMING_POINTER_MODE 0x30                                  /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_BAR_CODE_PRESENT_SENSOR 0x31                              /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_CLASS_1A_LASER 0x32                                       /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_CLASS_2_LASER 0x33                                        /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_HEATER_PRESENT 0x34                                       /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_CONTACT_SCANNER 0x35                                      /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_ELECTRONIC_ARTICLE_SURVEILLANCE_NOTIFICATION 0x36         /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_CONSTANT_ELECTRONIC_ARTICLE_SURVEILLANCE 0x37             /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_ERROR_INDICATION 0x38                                     /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_FIXED_BEEPER 0x39                                         /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_GOOD_DECODE_INDICATION 0x3A                               /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_HANDS_FREE_SCANNING 0x3B                                  /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_INTRINSICALLY_SAFE 0x3C                                   /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_KLASSE_EINS_LASER 0x3D                                    /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_LONG_RANGE_SCANNER 0x3E                                   /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_MIRROR_SPEED_CONTROL 0x3F                                 /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_NOT_ON_FILE_INDICATION 0x40                               /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_PROGRAMMABLE_BEEPER 0x41                                  /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_TRIGGERLESS 0x42                                          /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_WAND 0x43                                                 /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_WATER_RESISTANT 0x44                                      /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_MULTI_RANGE_SCANNER 0x45                                  /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_PROXIMITY_SENSOR 0x46                                     /* Usage type:  SF */
#define USB_HID_USAGE_POS_BARCODE_FRAGMENT_DECODING 0x4D                                    /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_SCANNER_READ_CONFIDENCE 0x4E                              /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_DATA_PREFIX 0x4F                                          /* Usage type:  NAry */
#define USB_HID_USAGE_POS_BARCODE_PREFIX_AIMI 0x50                                          /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_PREFIX_NONE 0x51                                          /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_PREFIX_PROPRIETARY 0x52                                   /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_ACTIVE_TIME 0x55                                          /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_AIMING_LASER_PATTERN 0x56                                 /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_BAR_CODE_PRESENT 0x57                                     /* Usage type:  OOC */
#define USB_HID_USAGE_POS_BARCODE_BEEPER_STATE 0x58                                         /* Usage type:  OOC */
#define USB_HID_USAGE_POS_BARCODE_LASER_ON_TIME 0x59                                        /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_LASER_STATE 0x5A                                          /* Usage type:  OOC */
#define USB_HID_USAGE_POS_BARCODE_LOCKOUT_TIME 0x5B                                         /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_MOTOR_STATE 0x5C                                          /* Usage type:  OOC */
#define USB_HID_USAGE_POS_BARCODE_MOTOR_TIMEOUT 0x5D                                        /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_POWER_ON_RESET_SCANNER 0x5E                               /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_PREVENT_READ_OF_BARCODES 0x5F                             /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_INITIATE_BARCODE_READ 0x60                                /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_TRIGGER_STATE 0x61                                        /* Usage type:  OOC */
#define USB_HID_USAGE_POS_BARCODE_TRIGGER_MODE 0x62                                         /* Usage type:  NAry */
#define USB_HID_USAGE_POS_BARCODE_TRIGGER_MODE_BLINKING_LASER_ON 0x63                       /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_TRIGGER_MODE_CONTINUOUS_LASER_ON 0x64                     /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_TRIGGER_MODE_LASER_ON_WHILE_PULLED 0x65                   /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_TRIGGER_MODE_LASER_STAYS_ON_AFTER_TRIGGER_RELEASE 0x66    /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_COMMIT_PARAMETERS_TO_NVM 0x6D                             /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_PARAMETER_SCANNING 0x6E                                   /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_PARAMETERS_CHANGED 0x6F                                   /* Usage type:  OOC */
#define USB_HID_USAGE_POS_BARCODE_SET_PARAMETER_DEFAULT_VALUES 0x70                         /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_SCANNER_IN_CRADLE 0x75                                    /* Usage type:  OOC */
#define USB_HID_USAGE_POS_BARCODE_SCANNER_IN_RANGE 0x76                                     /* Usage type:  OOC */
#define USB_HID_USAGE_POS_BARCODE_AIM_DURATION 0x7A                                         /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_GOOD_READ_LAMP_DURATION 0x7B                              /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_GOOD_READ_LAMP_INTENSITY 0x7C                             /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_GOOD_READ_LED 0x7D                                        /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_GOOD_READ_TONE_FREQUENCY 0x7E                             /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_GOOD_READ_TONE_LENGTH 0x7F                                /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_GOOD_READ_TONE_VOLUME 0x80                                /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_NO_READ_MESSAGE 0x82                                      /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_NOT_ON_FILE_VOLUME 0x83                                   /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_POWERUP_BEEP 0x84                                         /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_SOUND_ERROR_BEEP 0x85                                     /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_SOUND_GOOD_READ_BEEP 0x86                                 /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_SOUND_NOT_ON_FILE_BEEP 0x87                               /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_GOOD_READ_WHEN_TO_WRITE 0x88                              /* Usage type:  NAry */
#define USB_HID_USAGE_POS_BARCODE_GRWTI_AFTER_DECODE 0x89                                   /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_GRWTI_BEEP_LAMP_AFTER_TRANSMIT 0x8A                       /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_GRWTI_NO_BEEP_LAMP_USE_AT_ALL 0x8B                        /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_BOOKLAND_EAN 0x91                                         /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_CONVERT_EAN_8_TO_13_TYPE 0x92                             /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_CONVERT_UPC_A_TO_EAN_13 0x93                              /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_CONVERT_UPC_E_TO_A 0x94                                   /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_EAN_13 0x95                                               /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_EAN_8 0x96                                                /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_EAN_99_128_MANDATORY 0x97                                 /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_EAN_99_P5_128_OPTIONAL 0x98                               /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_UPC_EAN 0x9A                                              /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_UPC_EAN_COUPON_CODE 0x9B                                  /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_UPC_EAN_PERIODICALS 0x9C                                  /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_UPC_A 0x9D                                                /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_UPC_A_WITH_128_MANDATORY 0x9E                             /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_UPC_A_WITH_128_OPTIONAL 0x9F                              /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_UPC_A_WITH_P5_OPTIONAL 0xA0                               /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_UPC_E 0xA1                                                /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_UPC_E1 0xA2                                               /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_PERIODICAL 0xA9                                           /* Usage type:  NAry */
#define USB_HID_USAGE_POS_BARCODE_PERIODICAL_AUTO_DISCRIMINATE_PLUS_2 0xAA                  /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_PERIODICAL_ONLY_DECODE_WITH_PLUS_2 0xAB                   /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_PERIODICAL_IGNORE_PLUS_2 0xAC                             /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_PERIODICAL_AUTO_DISCRIMINATE_PLUS_5 0xAD                  /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_PERIODICAL_ONLY_DECODE_WITH_PLUS_5 0xAE                   /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_PERIODICAL_IGNORE_PLUS_5 0xAF                             /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_CHECK 0xB0                                                /* Usage type:  NAry */
#define USB_HID_USAGE_POS_BARCODE_CHECK_DISABLE_PRICE 0xB1                                  /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_CHECK_ENABLE_4_DIGIT_PRICE 0xB2                           /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_CHECK_ENABLE_5_DIGIT_PRICE 0xB3                           /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_CHECK_ENABLE_EUROPEAN_4_DIGIT_PRICE 0xB4                  /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_CHECK_ENABLE_EUROPEAN_5_DIGIT_PRICE 0xB5                  /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_EAN_TWO_LABEL 0xB7                                        /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_EAN_THREE_LABEL 0xB8                                      /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_EAN_8_FLAG_DIGIT_1 0xB9                                   /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_EAN_8_FLAG_DIGIT_2 0xBA                                   /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_EAN_8_FLAG_DIGIT_3 0xBB                                   /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_EAN_13_FLAG_DIGIT_1 0xBC                                  /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_EAN_13_FLAG_DIGIT_2 0xBD                                  /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_TRANSMIT_CHECK_DIGIT 0xF0                                 /* Usage type:  NAry */
#define USB_HID_USAGE_POS_BARCODE_DISABLE_CHECK_DIGIT_TRANSMIT 0xF1                         /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_ENABLE_CHECK_DIGIT_TRANSMIT 0xF2                          /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_SYMBOLOGY_IDENTIFIER_1 0xFB                               /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_SYMBOLOGY_IDENTIFIER_2 0xFC                               /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_SYMBOLOGY_IDENTIFIER_3 0xFD                               /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_DECODED_DATA 0xFE                                         /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_DECODE_DATA_CONTINUED 0xFF                                /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_BAR_SPACE_DATA 0x100                                      /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_SCANNER_DATA_ACCURACY 0x101                               /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_RAW_DATA_POLARITY 0x102                                   /* Usage type:  NAry */
#define USB_HID_USAGE_POS_BARCODE_POLARITY_INVERTED_BAR_CODE 0x103                          /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_POLARITY_NORMAL_BAR_CODE 0x104                            /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_MINIMUM_LENGTH_TO_DECODE 0x106                            /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_MAXIMUM_LENGTH_TO_DECODE 0x107                            /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_FIRST_DISCRETE_LENGTH_TO_DECODE 0x108                     /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_SECOND_DISCRETE_LENGTH_TO_DECODE 0x109                    /* Usage type:  DV */
#define USB_HID_USAGE_POS_BARCODE_DATA_LENGTH_METHOD 0x10A                                  /* Usage type:  NAry */
#define USB_HID_USAGE_POS_BARCODE_DL_METHOD_READ_ANY 0x10B                                  /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_DL_METHOD_CHECK_IN_RANGE 0x10C                            /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_DL_METHOD_CHECK_FOR_DISCRETE 0x10D                        /* Usage type:  SEL */
#define USB_HID_USAGE_POS_BARCODE_AZTEC_CODE 0x110                                          /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_BC412 0x111                                               /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_CHANNEL_CODE 0x112                                        /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_CODE_16 0x113                                             /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_CODE_32 0x114                                             /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_CODE_49 0x115                                             /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_CODE_ONE 0x116                                            /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_COLORCODE 0x117                                           /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_DATA_MATRIX 0x118                                         /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_MAXICODE 0x119                                            /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_MICROPDF 0x11A                                            /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_PDF_417 0x11B                                             /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_POSICODE 0x11C                                            /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_QR_CODE 0x11D                                             /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_SUPERCODE 0x11E                                           /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_ULTRACODE 0x11F                                           /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_USD_5_SLUG_CODE 0x120                                     /* Usage type:  DF */
#define USB_HID_USAGE_POS_BARCODE_VERICODE 0x121                                            /* Usage type:  DF */
/** @} */

/**
 * @name Page 0x8D: Scale
 * @{
 */
#define USB_HID_USAGE_POS_SCALE_UNDEFINED 0x00
#define USB_HID_USAGE_POS_SCALE_WEIGHING_DEVICE 0x01                            /* Usage type:  CA */
#define USB_HID_USAGE_POS_SCALE_SCALE_DEVICE 0x20                               /* Usage type:  CL */
#define USB_HID_USAGE_POS_SCALE_SCALE_CLASS_I_METRIC 0x21                       /* Usage type:  CL */
#define USB_HID_USAGE_POS_SCALE_SCALE_CLASS_I_METRIC_2 0x22                     /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_CLASS_II_METRIC 0x23                      /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_CLASS_III_METRIC 0x24                     /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_CLASS_IIIL_METRIC 0x25                    /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_CLASS_IV_METRIC 0x26                      /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_CLASS_III_ENGLISH 0x27                    /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_CLASS_IIIL_ENGLISH 0x28                   /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_CLASS_IV_ENGLISH 0x29                     /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_CLASS_GENERIC 0x2A                        /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_ATTRIBUTE_REPORT 0x30                     /* Usage type:  CL */
#define USB_HID_USAGE_POS_SCALE_SCALE_CONTROL_REPORT 0x31                       /* Usage type:  CL */
#define USB_HID_USAGE_POS_SCALE_SCALE_DATA_REPORT 0x32                          /* Usage type:  CL */
#define USB_HID_USAGE_POS_SCALE_SCALE_STATUS_REPORT 0x33                        /* Usage type:  CL */
#define USB_HID_USAGE_POS_SCALE_SCALE_WEIGHT_LIMIT_REPORT 0x34                  /* Usage type:  CL */
#define USB_HID_USAGE_POS_SCALE_SCALE_STATISTICS_REPORT 0x35                    /* Usage type:  CL */
#define USB_HID_USAGE_POS_SCALE_DATA_WEIGHT 0x40                                /* Usage type:  DV */
#define USB_HID_USAGE_POS_SCALE_DATA_SCALING 0x41                               /* Usage type:  CV */
#define USB_HID_USAGE_POS_SCALE_WEIGHT_UNIT 0x50                                /* Usage type:  CL */
#define USB_HID_USAGE_POS_SCALE_WEIGHT_UNIT_MILLIGRAM 0x51                      /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_WEIGHT_UNIT_GRAM 0x52                           /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_WEIGHT_UNIT_KILOGRAM 0x53                       /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_WEIGHT_UNIT_CARATS 0x54                         /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_WEIGHT_UNIT_TAELS 0x55                          /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_WEIGHT_UNIT_GRAINS 0x56                         /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_WEIGHT_UNIT_PENNYWEIGHTS 0x57                   /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_WEIGHT_UNIT_METRIC_TON 0x58                     /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_WEIGHT_UNIT_AVOIR_TON 0x59                      /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_WEIGHT_UNIT_TROY_OUNCE 0x5A                     /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_WEIGHT_UNIT_OUNCE 0x5B                          /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_WEIGHT_UNIT_POUND 0x5C                          /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_CALIBRATION_COUNT 0x60                          /* Usage type:  DV */
#define USB_HID_USAGE_POS_SCALE_RE_ZERO_COUNT 0x61                              /* Usage type:  DV */
#define USB_HID_USAGE_POS_SCALE_SCALE_STATUS 0x70                               /* Usage type:  CL */
#define USB_HID_USAGE_POS_SCALE_SCALE_STATUS_FAULT 0x71                         /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_STATUS_STABLE_AT_CENTER_OF_ZERO 0x72      /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_STATUS_IN_MOTION 0x73                     /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_STATUS_WEIGHT_STABLE 0x74                 /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_STATUS_UNDER_ZERO 0x75                    /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_STATUS_OVER_WEIGHT_LIMIT 0x76             /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_STATUS_REQUIRES_CALIBRATION 0x77          /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_SCALE_STATUS_REQUIRES_RE_ZEROING 0x78           /* Usage type:  SEL */
#define USB_HID_USAGE_POS_SCALE_ZERO_SCALE 0x80                                 /* Usage type:  OOC */
#define USB_HID_USAGE_POS_SCALE_ENFORCED_ZERO_RETURN 0x81                       /* Usage type:  OOC */
/** @} */

/**
 * @name Page 0x8E: Magnetic Stripe Reading MSR Devices
 * @{
 */
#define USB_HID_USAGE_POS_MSR_UNDEFINED 0x00
#define USB_HID_USAGE_POS_MSR_MSR_DEVICE_READ_ONLY 0x01     /* Usage type:  CA */
#define USB_HID_USAGE_POS_MSR_TRACK_1_LENGTH 0x11           /* Usage type:  SF, DF, SEL */
#define USB_HID_USAGE_POS_MSR_TRACK_2_LENGTH 0x12           /* Usage type:  SF, DF, SEL */
#define USB_HID_USAGE_POS_MSR_TRACK_3_LENGTH 0x13           /* Usage type:  SF, DF, SEL */
#define USB_HID_USAGE_POS_MSR_TRACK_JIS_LENGTH 0x14         /* Usage type:  SF, DF, SEL */
#define USB_HID_USAGE_POS_MSR_TRACK_DATA 0x20               /* Usage type:  SF, DF, SEL */
#define USB_HID_USAGE_POS_MSR_TRACK_1_DATA 0x21             /* Usage type:  SF, DF, SEL */
#define USB_HID_USAGE_POS_MSR_TRACK_2_DATA 0x22             /* Usage type:  SF, DF, SEL */
#define USB_HID_USAGE_POS_MSR_TRACK_3_DATA 0x23             /* Usage type:  SF, DF, SEL */
#define USB_HID_USAGE_POS_MSR_TRACK_JIS_DATA 0x24           /* Usage type:  SF, DF, SEL */
/** @} */

/**
 * @name Page 0x90: Camera Control
 * @{
 */
#define USB_HID_USAGE_CAMERA_UNDEFINED 0x00
#define USB_HID_USAGE_CAMERA_CAMERA_AUTO_FOCUS 0x20     /* Usage type:  OSC */
#define USB_HID_USAGE_CAMERA_CAMERA_SHUTTER 0x21        /* Usage type:  OSC */
/** @} */

/**
 * @name Page 0xF1D0: FIDO Alliance
 * @{
 */
#define USB_HID_USAGE_FIDO_UNDEFINED 0x00
#define USB_HID_USAGE_FIDO_U2F_AUTHENTICATOR_DEVICE 0x01    /* Usage type:  CA */
#define USB_HID_USAGE_FIDO_INPUT_REPORT_DATA 0x20           /* Usage type:  DV */
#define USB_HID_USAGE_FIDO_OUTPUT_REPORT_DATA 0x21          /* Usage type:  DV */
/** @} */
#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* USB_HID_HID_USAGE_H */
/** @} */
