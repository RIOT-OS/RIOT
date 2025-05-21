/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 */

#pragma once

/**
 * @ingroup     drivers_lis2dh12
 * @{
 *
 * @file
 * @brief       LIS2DH12 register definitions
 *
 * @author      Jan Mohr <jan.mohr@ml-pa.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Definition of read Registers
 * @{
 */

/**
 * @brief   STATUS_REG_AUX definitions
 */
enum {
    LIS2DH12_STATUS_REG_AUX_TDA = 0x04,   /**< Temperature new data available */
    LIS2DH12_STATUS_REG_AUX_TOR = 0x40,   /**< Temperature data overrun */
};

/**
 * @brief   STATUS_REG_TEMP definitions
 */
enum {
    LIS2DH12_TEMP_CFG_REG_DISABLE = 0x00, /**< Temperature sensor disable */
    LIS2DH12_TEMP_CFG_REG_ENABLE  = 0xC0, /**< Temperature sensor enable  */
};

/**
 * @brief   STATUS_REG definitions
 */
enum {
    LIS2DH12_STATUS_REG_XDA   = 0x01,  /**< X-axis new data available */
    LIS2DH12_STATUS_REG_YDA   = 0x02,  /**< Y-axis new data available */
    LIS2DH12_STATUS_REG_ZDA   = 0x04,  /**< Z-axis new data available */
    LIS2DH12_STATUS_REG_ZYXDA = 0x08,  /**< On X-, Y-, Z-axis new data available */
    LIS2DH12_STATUS_REG_XOR   = 0x10,  /**< X-axis data overrun */
    LIS2DH12_STATUS_REG_YOR   = 0x20,  /**< Y-axis data overrun */
    LIS2DH12_STATUS_REG_ZOR   = 0x40,  /**< Y-axis data overrun */
    LIS2DH12_STATUS_REG_ZYXOR = 0x80,  /**< On X-, Y-, Z-axis data overrun */
};

/**
 * @brief   INT1_SRC and INT2_SRC definitions
 */
enum {
   LIS2DH12_INT_SRC_XL = 0x01, /**< X low event */
   LIS2DH12_INT_SRC_XH = 0x02, /**< X high event */
   LIS2DH12_INT_SRC_YL = 0x04, /**< Y low event */
   LIS2DH12_INT_SRC_YH = 0x08, /**< Y high event */
   LIS2DH12_INT_SRC_ZL = 0x10, /**< Z low event */
   LIS2DH12_INT_SRC_ZH = 0x20, /**< Z high event */
   LIS2DH12_INT_SRC_IA = 0x40, /**< Interrupt 1 active, at least one interrupt \
                                        has been generated */
};

/**
 * @brief   Interrupt config register values
 */
enum {
    LIS2DH12_INT_CFG_XLIE = 0x01,   /**< enable X low event */
    LIS2DH12_INT_CFG_XHIE = 0x02,   /**< enable X high event */
    LIS2DH12_INT_CFG_YLIE = 0x04,   /**< enable Y low event */
    LIS2DH12_INT_CFG_YHIE = 0x08,   /**< enable Y high event */
    LIS2DH12_INT_CFG_ZLIE = 0x10,   /**< enable Z low event */
    LIS2DH12_INT_CFG_ZHIE = 0x20,   /**< enable Z high event */
    LIS2DH12_INT_CFG_6D   = 0x40,   /**< enable 6-direction detection */
    LIS2DH12_INT_CFG_AOI  = 0x80,   /**< and/or combination interrupt events */
};

/**
 * @brief   Interrupt type register values
 */
enum {
    /* for interrupt 1 (CTRL_REG3) */
    LIS2DH12_INT_TYPE_I1_OVERRUN = 0x02, /**< FIFO overrun interrupt on INT1 */
    LIS2DH12_INT_TYPE_I1_WTM     = 0x04, /**< FIFO watermark interrupt on INT1 */
    LIS2DH12_INT_TYPE_I1_ZYXDA   = 0x10, /**< ZYXDA interrupt on INT1 */
    LIS2DH12_INT_TYPE_I1_IA2     = 0x20, /**< IA2 interrupt on INT1 */
    LIS2DH12_INT_TYPE_I1_IA1     = 0x40, /**< IA1 interrupt on INT1 */
    LIS2DH12_INT_TYPE_I1_CLICK   = 0x80, /**< click interrupt on INT1 */

    /* for interrupt 2 (CTRL_REG6) */
    LIS2DH12_INT_TYPE_INT_POLARITY = 0x02, /**< INT1 and INT2 pin polarity */
    LIS2DH12_INT_TYPE_I2_ACT       = 0x08, /**< enable activity interrupt on INT2 */
    LIS2DH12_INT_TYPE_I2_BOOT      = 0x10, /**< enable boot on INT2 */
    LIS2DH12_INT_TYPE_I2_IA2       = 0x20, /**< IA2 on INT2 */
    LIS2DH12_INT_TYPE_I2_IA1       = 0x40, /**< IA1 on INT2 */
    LIS2DH12_INT_TYPE_I2_CLICK     = 0x80, /**< click interrupt on INT2 */

    LIS2DH12_INT_TYPE_IA2          = 0x20, /**< Event 2 */
    LIS2DH12_INT_TYPE_IA1          = 0x40, /**< Event 1 */
    LIS2DH12_INT_TYPE_CLICK        = 0x80, /**< click interrupt */
};

/**
 * @brief   Event slots
 */
enum {
    LIS2DH12_EVENT_1               = 0x1, /**< first event slot */
    LIS2DH12_EVENT_2               = 0x2, /**< second event slot */
    LIS2DH12_EVENT_CLICK           = 0x3, /**< click event */
};

/**
 * @brief   Extract interrupt flags for Event Slot 1
 */
#define LIS2DH12_INT_SRC_1(ret)     (((uint32_t)(ret) >>  0) & 0x7F)

/**
 * @brief   Extract interrupt flags for Event Slot 2
 */
#define LIS2DH12_INT_SRC_2(ret)     (((uint32_t)(ret) >>  8) & 0x7F)

/**
 * @brief   Extract interrupt flags for Click Event
 */
#define LIS2DH12_INT_SRC_CLICK(ret) (((uint32_t)(ret) >> 16) & 0x7F)

/**
 * @brief   CLICK_SRC definitions
 */
typedef union {
    struct {
        uint8_t X_AXIS:1;    /**< X click detected */
        uint8_t Y_AXIS:1;    /**< Y click detected */
        uint8_t Z_AXIS:1;    /**< Z click detected */
        uint8_t Sign:1;      /**< Click sign, "0" positive, "1" negative */
        uint8_t SClick:1;    /**< Single click detected */
        uint8_t DClick:1;    /**< Double click detected */
        uint8_t IA:1;        /**< Interrupt active, at least one interrupt \
                                has been generated */
        uint8_t _RESERVED:1; /**< Reserved bit */
    } bit;                     /**< Structure used for bit access */
    uint8_t reg;               /**< Type used for register access */
} LIS2DH12_CLICK_SRC_t;

/**
 * @brief   FIFO_SRC_REG definitions
 */
typedef union {
    struct {
        uint8_t FSS:5;          /**< Number of unread samples in FIFO */
        uint8_t EMPTY:1;        /**< FIFO is empty */
        uint8_t OVRN_FIFO:1;    /**< Overrun in FIFO occurred */
        uint8_t WTM:1;          /**< FIFO content watermark level */
    } bit;                      /**< Structure used for bit access */
    uint8_t reg;                /**< Type used for register access */
} LIS2DH12_FIFO_SRC_REG_t;
/** @} */

/**
 *
 * @name    Definition of read/write Registers
 * @{
 */
/**
 * @brief   CTRL_REG_0 definitions
 */
typedef union {
    struct {
        uint8_t CTRL0_DEFAULT_VALUE:7;  /**< Always set this to CTRL_REG0_DEFAULT */
        uint8_t SDO_PU_DISC:1;          /**< disconnect pull-up on SDO/SA0 */
    } bit;                              /**< Structure used for bit access */
    uint8_t reg;                        /**< Type used for register access */
} LIS2DH12_CTRL_REG0_t;

/**
 * @brief   TEMP_CFG_REG definitions
 */
typedef union {
    struct {
        uint8_t _RESERVED:6;    /**< Should always be zero */
        uint8_t TEMP_EN:2;      /**< "00" disables Temperature sensor, "11" enables */
    } bit;                      /**< Structure used for bit access */
    uint8_t reg;                /**< Type used for register access */
} LIS2DH12_TEMP_CFG_REG_t;

/**
 * @brief   CTRL_REG1 definitions
 */
typedef union {
    struct {
        uint8_t Xen:1;  /**< X axis enable */
        uint8_t Yen:1;  /**< Y axis enable */
        uint8_t Zen:1;  /**< Z axis enable */
        uint8_t LPen:1; /**< Enable Low Power mode */
        uint8_t ODR:4;  /**< Set Data rate */
    } bit;              /**< Structure used for bit access */
    uint8_t reg;        /**< Type used for register access */
} LIS2DH12_CTRL_REG1_t;

#define LIS2DH12_CTRL_REG2_HP_IA1   (1 << 0)
#define LIS2DH12_CTRL_REG2_HP_IA2   (1 << 1)
#define LIS2DH12_CTRL_REG2_HPCLICK  (1 << 2)
#define LIS2DH12_CTRL_REG2_FDS      (1 << 3)

#define LIS2DH12_CLICK_THS_LIR      (0x80)

/**
 * @brief   CTRL_REG2 definitions
 */
typedef union {
    struct {
        uint8_t HP_IA1:1;   /**< High pass filter enable for AOI on interrupt 1 */
        uint8_t HP_IA2:1;   /**< High pass filter enable for AOI on interrupt 2 */
        uint8_t HPCLICK:1;  /**< High pass filter enable for CLICK function */
        uint8_t FDS:1;      /**< Enables filter output data */
        uint8_t HPCF:2;     /**< High pass filter cutoff frequency */
        uint8_t HPM:2;      /**< High pass filter mode selection */
    } bit;                  /**< Structure used for bit access */
    uint8_t reg;            /**< Type used for register access */
} LIS2DH12_CTRL_REG2_t;

/**
 * @brief   CTRL_REG3 definitions
 */
typedef union {
    struct {
        uint8_t _RESERVED0:1;   /**< Reserved bit */
        uint8_t I1_OVERRUN:1;   /**< Enable FIFO overrun interrupt on INT1 */
        uint8_t I1_WTM:1;       /**< Enable FIFO watermark interrupt on INT1 */
        uint8_t _RESERVED3:1;   /**< Should always be "0" */
        uint8_t I1_ZYXDA:1;     /**< Enable ZYXDA interrupt on INT1 */
        uint8_t I1_IA2:1;       /**< Enable IA2 interrupt on INT1 */
        uint8_t I1_IA1:1;       /**< Enable IA1 interrupt on INT1 */
        uint8_t I1_CLICK:1;     /**< Enable CLICK interrupt on INT1 */
    } bit;                      /**< Structure used for bit access */
    uint8_t reg;                /**< Type used for register access */
} LIS2DH12_CTRL_REG3_t;

/**
 * @brief   CTRL_REG4 definitions
 */
typedef union {
    struct {
        uint8_t SPIM:1; /**< SPI serial interface mode selection (SIM)*/
        uint8_t ST:2;   /**< Self-test enable */
        uint8_t HR:1;   /**< Operating mode */
        uint8_t FS:2;   /**< Full-scale selection */
        uint8_t BLE:1;  /**< Big/Little endian data selection */
        uint8_t BDU:1;  /**< Block data update */
    } bit;              /**< Structure used for bit access */
    uint8_t reg;        /**< Type used for register access */
} LIS2DH12_CTRL_REG4_t;

/**
 * @brief   CTRL_REG5 definitions
 */
typedef union {
    struct {
        uint8_t D4D_INT2:1;     /**< 4D detection enabled on INT2 */
        uint8_t LIR_INT2:1;     /**< Latch interrupt request for INT2 */
        uint8_t D4D_INT1:1;     /**< 4D detection enabled on INT1 */
        uint8_t LIR_INT1:1;     /**< Latch interrupt request for INT2 */
        uint8_t _RESERVED:2;    /**< Reserved bits */
        uint8_t FIFO_EN:1;      /**< FIFO enable */
        uint8_t BOOT:1;         /**< Clears the data content */
    } bit;                      /**< Structure used for bit access */
    uint8_t reg;                /**< Type used for register access */
} LIS2DH12_CTRL_REG5_t;

/**
 * @brief   CTRL_REG6 definitions
 */
typedef union {
    struct {
        uint8_t _RESERVED0:1;   /**< Reserved bit */
        uint8_t INT_POLARITY:1; /**< Set pin polarity for INT1 and INT2 */
        uint8_t _RESERVED2:1;   /**< Reserved bit */
        uint8_t I2_ACT:1;       /**< Enable activity interrupt on INT2 */
        uint8_t I2_BOOT:1;      /**< Enable boot on INT2 */
        uint8_t I2_IA2:1;       /**< Enable IA2 on INT2 */
        uint8_t I2_IA1:1;       /**< Enable IA1 on INT2 */
        uint8_t I2_CLICK:1;     /**< Enable CLICK interrupt on INT2 */
    } bit;                      /**< Structure used for bit access */
    uint8_t reg;                /**< Type used for register access */
} LIS2DH12_CTRL_REG6_t;

/**
 * @brief   REFERENCE definitions
 */
typedef union {
    uint8_t reg;    /**< Set reference value */
} LIS2DH12_REFERENCE_t;

/**
 * @brief   FIFO_CTRL_REG definitions
 */
typedef union {
    struct {
        uint8_t FTH:5;  /**< Set the watermark level for FIFO */
        uint8_t TR:1;   /**< Triggering selection, FIFO event triggers INT1 or INT2 */
        uint8_t FM:2;   /**< FIFO mode selection */
    } bit;              /**< Structure used for bit access */
    uint8_t reg;        /**< Type used for register access */
} LIS2DH12_FIFO_CTRL_REG_t;

/**
 * @brief   INT1_CFG definitions
 */
typedef union {
    struct {
        uint8_t XLIE:1; /**< Enable interrupt on X low event */
        uint8_t XHIE:1; /**< Enable interrupt on X high event */
        uint8_t YLIE:1; /**< Enable interrupt on Y low event */
        uint8_t YHIE:1; /**< Enable interrupt on Y high event */
        uint8_t ZLIE:1; /**< Enable interrupt on Z low event */
        uint8_t ZHIE:1; /**< Enable interrupt on Z high event */
        uint8_t D6D:1;  /**< 6 direction detection function enable */
        uint8_t AOI:1;  /**< AND/OR combination of interrupt events */
    } bit;              /**< Structure used for bit access */
    uint8_t reg;        /**< Type used for register access */
} LIS2DH12_INT1_CFG_t;

/**
 * @brief   INT1_THS definitions
 */
typedef union {
    struct {
        uint8_t THS:7;          /**< Sets threshold level, the LSB changes according to
                                    LIS2DH12_SCALE (@2G LSB=16mg; @4G LSB=32mg; @8G LSB=62mg
                                    @16G LSB=186mg) */
        uint8_t _RESERVED:1;    /**< needs to be zero */
    } bit;                      /**< Structure used for bit access */
    uint8_t reg;                /**< Type used for register access */
} LIS2DH12_INT1_THS_t;

/**
 * @brief   INT1_DURATION definitions
 */
typedef union {
    struct {
        uint8_t D:7;            /**< Sets the minimum duration of INT1, in ODR cycles */
        uint8_t _RESERVED:1;    /**< need to be zero */
    } bit;                      /**< Structure used for bit access */
    uint8_t reg;                /**< Type used for register access */
} LIS2DH12_INT1_DURATION_t;

/**
 * @brief   INT2_CFG definitions
 */
typedef union {
    struct {
        uint8_t XLIE:1; /**< Enable interrupt on X low event */
        uint8_t XHIE:1; /**< Enable interrupt on X high event */
        uint8_t YLIE:1; /**< Enable interrupt on Y low event */
        uint8_t YHIE:1; /**< Enable interrupt on Y high event */
        uint8_t ZLIE:1; /**< Enable interrupt on Z low event */
        uint8_t ZHIE:1; /**< Enable interrupt on Z high event */
        uint8_t D6D:1;  /**< 6 direction detection function enable */
        uint8_t AOI:1;  /**< AND/OR combination of interrupt events */
    } bit;              /**< Structure used for bit access */
    uint8_t reg;        /**< Type used for register access */
} LIS2DH12_INT2_CFG_t;

/**
 * @brief   INT2_THS definitions
 */
typedef union {
    struct {
        uint8_t THS:7;          /**< Sets threshold level, LSB according to LIS2DH12_SCALE */
        uint8_t _RESERVED:1;    /**< needs to be zero */
    } bit;                      /**< Structure used for bit access */
    uint8_t reg;                /**< Type used for register access */
} LIS2DH12_INT2_THS_t;

/**
 * @brief   INT2_DURATION definitions
 */
typedef union {
    struct {
        uint8_t D:7;            /**< Sets the minimum duration of INT2, in ODR cycles */
        uint8_t _RESERVED:1;    /**< need to be zero */
    } bit;                      /**< Structure used for bit access */
    uint8_t reg;                /**< Type used for register access */
} LIS2DH12_INT2_DURATION_t;

/**
 * @brief   CLICK_CFG definitions
 */
typedef union {
    struct {
        uint8_t XS:1;           /**< Interrupt single-click enable on X-axis */
        uint8_t XD:1;           /**< Interrupt double-click enable on X-axis */
        uint8_t YS:1;           /**< Interrupt single-click enable on Y-axis */
        uint8_t YD:1;           /**< Interrupt double-click enable on Y-axis */
        uint8_t ZS:1;           /**< Interrupt single-click enable on Z-axis */
        uint8_t ZD:1;           /**< Interrupt double-click enable on Z-axis */
        uint8_t _RESERVED:2;    /**< Reserved bits */
    } bit;                      /**< Structure used for bit access */
    uint8_t reg;                /**< Type used for register access */
} LIS2DH12_CLICK_CFG_t;

/**
 * @brief   CLICK_THS definitions
 */
typedef union {
    struct {
        uint8_t THS:7;          /**< Sets the click threshold, LSB according to LIS2DH12_SCALE */
        uint8_t LIR_CLICK:1;    /**< Enables latency on interrupt kept high, \
                                    "0" for duration of latency window, \
                                    "1" kept high until CLICK_SRC is read */
    } bit;                      /**< Structure used for bit access */
    uint8_t reg;                /**< Type used for register access */
} LIS2DH12_CLICK_THS_t;

/**
 * @brief   TIME_LIMIT definitions
 */
typedef union {
    struct {
        uint8_t TLI:7;          /**< Click time limit, in ODR cycles */
        uint8_t _RESERVED:1;    /**< reserved bit */
    } bit;                      /**< Structure used for bit access */
    uint8_t reg;                /**< Type used for register access */
} LIS2DH12_TIME_LIMIT_t;

/**
 * @brief   TIME_LATENCY definitions
 */
typedef union {
    uint8_t reg; /**< Sets time latency, in ODR cycles */
} LIS2DH12_TIME_LATENCY_t;

/**
 * @brief   TIME_WINDOW definitions
 */
typedef union {
    uint8_t reg; /**< Sets time window, in ODR cycles */
} LIS2DH12_TIME_WINDOW_t;

/**
 * @brief   ACT_THS definitions
 */
typedef union {
    struct {
        uint8_t ACTH:7;         /**< Sets the threshold sleep-to-wake or return-to-sleep
                                    LSB according to LIS2DH12_SCALE */
        uint8_t _RESERVED:1;    /**< reserved bit */
    } bit;                      /**< Structure used for bit access */
    uint8_t reg;                /**< Type used for register access */
} LIS2DH12_ACT_THS_t;

/**
 * @brief   ACT_DURATION definitions
 */
typedef union {
    uint8_t reg; /**< Sleep-to-wake and return-to-sleep duration, in ODR cycles */
} LIS2DH12_ACT_DURATION_t;
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
