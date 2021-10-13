// See LICENSE for license details.

#ifndef _SIFIVE_I2C_H
#define _SIFIVE_I2C_H

/* Register offsets */
#define I2C_PRESCALE_LO		(0x00)
#define I2C_PRESCALE_HI		(0x04)
#define I2C_CONTROL			(0x08)
#define I2C_DATA			(0x0C)
#define I2C_CMD				(0x10)
#define I2C_STATUS			(0x10)

/* CONTROL register */
#define I2C_CONTROL_EN		(1 << 7)
#define I2C_CONTROL_IE		(1 << 6)

/* CMD register */
#define I2C_CMD_STA			(1 << 7)
#define I2C_CMD_STO			(1 << 6)
#define I2C_CMD_RD			(1 << 5)
#define I2C_CMD_WR			(1 << 4)
#define I2C_CMD_ACK			(1 << 3)
#define I2C_CMD_IACK		(1 << 0)

/* STATUS register */
#define I2C_STATUS_RXACK	(1 << 7)
#define I2C_STATUS_BUSY		(1 << 6)
#define I2C_STATUS_ALOST	(1 << 5)
#define I2C_STATUS_TIP		(1 << 1)
#define I2C_STATUS_IF		(1 << 0)

#endif /* _SIFIVE_I2C_H */
