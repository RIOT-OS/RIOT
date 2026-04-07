---
title: Porting CPUs
description: Guide on how to port new CPU families to RIOT-OS
---

This guide was created during the process of porting a new MCU family
(Infineon PSoC-6) to RIOT. It is intended as a practical companion to RIOT's
existing board-porting documentation and focuses on the structure of a CPU port.

The goal of a CPU port is to make the MCU family usable from RIOT's generic
build system, startup code and peripheral APIs. A complete port
usually consists of:

- a `cpu/<cpu>/` directory that provides startup, common CPU configuration, and
  peripheral implementations,
- one or more `boards/<board>/` directories that map the CPU to a concrete
  development board, and
- documentation and tests that make the port maintainable.

This document focuses on the CPU side. For the board side, also refer to the
existing RIOT guide on porting boards.

:::note
A CPU port defines what the MCU family can do. A board port defines which of
those features are actually available on a specific board.
:::

## CPU Port File Structure

A typical CPU port has a file structure similar to the following:

```text
cpu/foo
├── include
│   ├── cpu_conf.h
│   ├── periph_cpu.h
│   └── vendor
├── ldscripts
├── periph
│   ├── gpio.c
│   ├── i2c.c
│   ├── spi.c
│   └── ...
├── vectors
│   └── vectors_<model>.c
├── Makefile
├── Makefile.dep
├── Makefile.features
├── Makefile.include
└── cpu.c
```

Not every subdirectory is mandatory, but most non-trivial ports will need at
least these elements.

## Porting Workflow

A practical CPU porting workflow usually looks like this:

1. create the CPU directory and build-system files,
2. add startup support and a working `cpu_init()`,
3. provide CPU and peripheral configuration headers,
4. add the interrupt vector table,
5. implement the required peripherals one by one,
6. connect the CPU to one or more boards via `boards/<board>/`,
7. add the CPU feature entry and validate the port with basic examples.

The following sections describe the individual files and their purpose.

## Build System Files

### `Makefile`

The top-level CPU `Makefile` defines the CPU module and may include common
subdirectories that are shared with other CPUs.

Typical responsibilities:

- define `MODULE = cpu`,
- include `$(RIOTBASE)/Makefile.base`,
- optionally add subdirectories such as `periph/`, `vectors/`, or a common CPU
  family directory.

Example:

```makefile
MODULE = cpu
DIRS = $(RIOTCPU)/cortexm_common periph

include $(RIOTBASE)/Makefile.base
```

### `Makefile.dep`

This file expresses build-time dependencies for the CPU. It is the right place
for conditional module additions that depend on the selected CPU model,
`USEMODULE`, or board features.

Typical responsibilities:

- pull in common CPU-family dependency files,
- select required helper modules

Example:

```makefile
include $(RIOTCPU)/cortexm_common/Makefile.dep

ifneq (,$(filter periph_uart,$(USEMODULE)))
  USEMODULE += periph_uart_reconfigure
endif
```

### `Makefile.features`

This file declares what the CPU provides to the rest of RIOT.

Typical responsibilities:

- define `CPU_CORE` (for example `cortex-m4f`),
- include common family feature files,
- populate `FEATURES_PROVIDED` with the peripherals actually implemented by the
  CPU port.

Example:

```makefile
CPU_CORE = cortex-m4f

FEATURES_PROVIDED += periph_gpio
FEATURES_PROVIDED += periph_i2c
FEATURES_PROVIDED += periph_spi
FEATURES_PROVIDED += periph_timer
FEATURES_PROVIDED += periph_uart
```

Only advertise features that are actually implemented and usable. Since the availability of some features can depend on the specific `CPU_MODEL`, conditional statements might be required here as well.

### `Makefile.include`

This file contains low-level build configuration for the CPU.

Typical responsibilities:

- add include paths via `INCLUDES += -I...`,
- define `LINKER_SCRIPT`,
- define the object containing the interrupt vector table via `VECTORS_O`,
- define ROM and RAM layout (`ROM_START_ADDR`, `ROM_LEN`, `RAM_START_ADDR`,
  `RAM_LEN`) of the `CPU_MODEL`,
- optionally define debugger and flashing helpers.

Example:

```makefile
INCLUDES += -I$(RIOTCPU)/foo/include
LINKER_SCRIPT = $(RIOTCPU)/foo/ldscripts/foo.ld
VECTORS_O = $(CPU_OBJ)/vectors/vectors_$(CPU_MODEL).o
```

For Cortex-M devices, this file often also ties the CPU port to the generic
`cortexm_common` startup and linker support.

## Core CPU Files

### `cpu.c`

`cpu.c` contains `cpu_init()`, which performs the minimum hardware and runtime
initialization needed before RIOT starts normal execution.

Typical responsibilities:

- call generic core initialization for the architecture,
- set interrupt priorities and interrupt grouping if required,
- configure clocks or clock assumptions used by the CPU port,
- perform any CPU-local setup that must happen before peripherals are used.

A minimal shape often looks like this:

```c
void cpu_init(void)
{
    /* On Cortex-M based ports, this is often the central architecture init. */
    cortexm_init();

    /* Add CPU-specific clock or IRQ setup here if the MCU requires it. */
}
```

Keep `cpu_init()` focused on essential CPU-level setup. Board-specific wiring
belongs into the board port, and driver-specific initialization belongs into the
corresponding peripheral implementation.

:::note
Refer to the Technical Reference Manual of the target CPU for details on implementing the boot process.
:::

### `vectors/`

This directory contains the interrupt vector table for the supported CPU model.
Without a correct vector table, the CPU will usually fault immediately after
reset.

Typical responsibilities:

- define the initial stack pointer entry,
- define the `Reset_Handler`,
- provide weak default handlers for all supported interrupts,
- match the IRQ numbering expected by
  `CPU_IRQ_NUMOF`.

### `ldscripts/`

This directory holds CPU-specific linker scripts if the CPU cannot directly use
an existing common linker script.

Typical responsibilities:

- define memory regions,
- place sections such as `.text`, `.data`, `.bss`, interrupt vectors, and stack,
- cooperate with RIOT's startup assumptions.

### `include/cpu_conf.h`

This header provides CPU-wide configuration.

Typical responsibilities:

- include the correct vendor header for the selected `CPU_MODEL`,
- define architecture-related constants such as `CPU_IRQ_NUMOF` and default IRQ
  priority values,
- provide CPU-family level feature or capability macros where needed,
- include common family configuration if applicable.

Example structure:

```c
#pragma once

#include "vendor/foo123.h"
#include "cpu_conf_common.h"

#define CPU_IRQ_NUMOF        (48U)
#define CPU_DEFAULT_IRQ_PRIO (1U)
```

The exact contents depend on the architecture and MCU family, but this file is
usually where RIOT learns which vendor header to use and how many interrupts the
CPU exposes.

### `include/periph_cpu.h`

This header provides CPU-specific peripheral types and helper macros.

Typical responsibilities:

- define or override peripheral handle types if needed,
- provide helper macros for pin encoding, peripheral instance access, or
  alternate-function selection,
- expose CPU-local constants required by the RIOT peripheral APIs (e.g.
  `CLOCK_CORECLOCK` or `PERIPH_CLOCK`),
- define CPU-specific peripheral configuration structs that are later used by
  the board's `periph_conf.h`.

Example structure:

```c
#pragma once

#include <stdint.h>
#include "vendor/<CPU_MODEL>.h"

#define CLOCK_CORECLOCK       (64000000U)
#define PERIPH_CLOCK          (16000000U)

/* CPU-local configuration types that are later used in periph_conf.h */
typedef struct {
    UART_Type *dev;
    gpio_t rx_pin;
    gpio_t tx_pin;
    uint8_t mux;
    IRQn_Type irqn;
} uart_conf_t;

typedef struct {
    TIMER_Type *dev;
    uint8_t channels;
} timer_conf_t;
```

The exact contents depend on the CPU family, but this file usually contains the
CPU-local building blocks that make the generic RIOT peripheral API usable for a
specific MCU implementation.

### `include/vendor/`

Vendor HAL headers can be placed here or integrated via RIOT's package
mechanism. Keeping vendor code clearly separated from the actual RIOT port helps
maintenance and updates.

## Registering the CPU Feature

The CPU should be added to `features.yaml` so the build system can resolve the
feature name consistently.

Example:

```yaml
- title: Example MCU Grouping
  features:
  - name: cpu_foo
    help: CPU family foo
```

After updating `features.yaml`, regenerate the derived feature files from the
RIOT root directory:

```bash
make generate-features
```

## Peripheral Implementation Overview

Peripheral implementations live in `cpu/<cpu>/periph/` and must follow the RIOT
API exactly. The authoritative contracts are the headers in
`drivers/include/periph/`.

Each implementation should:

- include the corresponding RIOT peripheral header (in `drivers/include/periph/`),
- use the function names and signatures defined by the API,
- use the board-provided mappings from `boards/<board>/include/periph_conf.h`,
- translate RIOT's generic types and enums into vendor-register or HAL calls,
- document unsupported options by returning the expected error code instead of
  silently behaving differently.

:::note
The board's `periph_conf.h` usually describes available instances, pin mappings,
clock inputs, channels, and similar hardware topology. The CPU driver should use
that data rather than hard-coding board-specific values.
:::

## GPIO

**RIOT API:** `drivers/include/periph/gpio.h`

The GPIO driver provides the basic digital pin interface for RIOT.

### Typical functionality to implement

- `gpio_init()` for input/output modes,
- `gpio_init_int()` for edge-triggered interrupts,
- `gpio_read()`, `gpio_set()`, `gpio_clear()`, `gpio_toggle()`,
- `gpio_irq_enable()` and `gpio_irq_disable()` if interrupt support is enabled,
- pin-to-port decoding helpers if the CPU uses packed GPIO identifiers.

### What the implementation generally needs to do

- validate the pin identifier,
- decode the encoded `gpio_t` into port and pin numbers if required,
- enable the GPIO block or port clock if the MCU gates it,
- select GPIO mode in the pin-mux or alternate-function registers,
- configure direction, input buffer, pull resistors, output type, and drive
  strength,
- preload the output latch if needed before switching a pin to output mode,
- configure interrupt trigger type and store callback context for
  `gpio_init_int()`,
- dispatch the registered callback from the ISR.

### Example register-level init sketch

```c
int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    /* Example sketch:
     * 1. Decode the port/pin tuple from pin.
     * 2. Enable the GPIO port clock if the MCU requires it.
     * 3. Route the pad to GPIO mode in the pin-mux register.
     * 4. Program direction and input/output buffer enable bits.
     * 5. Program pull-up / pull-down / open-drain / drive-strength bits.
     */
    return 0;
}
```

Keep the ISR short. It should typically acknowledge the interrupt and call the
registered callback.

## UART

**RIOT API:** `drivers/include/periph/uart.h`

UART is used for standard I/O, shell access, and device communication. In RIOT,
receive handling is interrupt-driven.

As a side note, UART setup usually reuses parts of the GPIO implementation:
pin muxing, GPIO direction defaults, and optional RTS/CTS handling all rely on
the underlying pin configuration support being correct.

### Typical functionality to implement

- `uart_init()`,
- `uart_write()`,
- `uart_poweron()` and `uart_poweroff()`,
- optional helpers such as reconfiguration or RX start interrupt support if the
  corresponding modules are enabled.

### What the implementation generally needs to do

- validate the selected UART instance,
- configure the RX/TX pins and select their alternate function,
- enable the UART peripheral clock or power domain,
- select the UART source clock and compute the baud-rate divider or oversampling
  setting from that parent clock,
- configure frame format, FIFO thresholds, and optional hardware flow control,
- store the RX callback context,
- unmask the peripheral interrupt and connect it to the NVIC if receive support
  is enabled,
- enable RX/TX and keep `uart_poweroff()` and `uart_poweron()` transparent to
  upper layers.

### Example register-level init sketch

```c
int uart_init(uart_t dev, uint32_t baud, uart_rx_cb_t rx_cb, void *arg)
{
    /* Example sketch:
     * 1. Validate dev and store rx_cb/arg for ISR use.
     * 2. Configure RX/TX pins and switch them to the UART alternate function.
     * 3. Enable the UART peripheral clock or power bit.
     * 4. Select the parent clock and program the baud-rate divider.
     * 5. Configure word length, parity, and stop bits.
     * 6. Configure FIFOs or DMA trigger thresholds if the hardware has them.
     * 7. If rx_cb != NULL, unmask the RX interrupt and enable the NVIC line.
     * 8. Enable the UART block.
     */
    return 0;
}
```

If the hardware supports FIFOs or DMA, that can be added internally, but the
external RIOT API should remain unchanged.

## Timer

**RIOT API:** `drivers/include/periph/timer.h`

Timers are foundational in RIOT because higher layers such as `ztimer` and
scheduler services depend on them. A reliable timer driver is therefore one of
the most important parts of a CPU port.

### Typical functionality to implement

- `timer_init()`,
- `timer_set()` and `timer_set_absolute()`,
- `timer_clear()`,
- `timer_read()`,
- `timer_start()` and `timer_stop()`,
- `timer_set_periodic()` if the CPU advertises periodic timer support.

### What the implementation generally needs to do

- configure the timer to a requested tick frequency,
- enable the timer peripheral clock and choose the correct timer source clock,
- select a prescaler or divider that can generate the requested `freq`,
- configure counter mode, bit width, reload behavior, and compare channels,
- store callback context for the timer device,
- clear pending compare and overflow events before enabling interrupts,
- acknowledge compare and overflow events in the ISR,
- call the registered callback with the channel number,
- expose the correct timer width and maximum counter value via the board
  configuration.

### Example register-level init sketch

```c
int timer_init(tim_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
    /* Example sketch:
     * 1. Validate dev and store cb/arg for the ISR.
     * 2. Enable the timer peripheral clock.
     * 3. Select timer mode and bit width.
     * 4. Choose a prescaler/divider so the timer runs at freq ticks per second.
     * 5. Reset the counter and clear pending compare/overflow events.
     * 6. Configure compare channels and unmask the timer interrupt.
     * 7. Start the timer.
     */
    return 0;
}
```

Pay close attention to counter width, overflow behavior, and compare semantics.
Mistakes here often break `ztimer`, sleeping, or scheduling in subtle ways.

## SPI

**RIOT API:** `drivers/include/periph/spi.h`

RIOT's SPI API is transaction-based because an SPI bus is a shared resource.
This means the driver is expected to configure and power the peripheral around
`spi_acquire()` / `spi_release()`.

GPIO support is also important here: SPI buses reuse GPIO handling for pin mux
setup, optional manual chip-select lines, and some reconfiguration paths.

### Typical functionality to implement

- `spi_init()` for bus-level initialization,
- `spi_init_pins()` and `spi_init_cs()`,
- `spi_acquire()` and `spi_release()`,
- data transfer helpers such as `spi_transfer_bytes()` and related single-word
  helpers.

### What the implementation generally needs to do

- initialize pins without assuming one fixed client device,
- enable the SPI peripheral clock or power domain when the bus is acquired,
- select a divider or prescaler that matches the requested `spi_clk_t`,
- configure clock polarity and phase in `spi_acquire()`,
- assert and deassert chip select if the driver manages CS,
- transfer bytes in full duplex,
- leave the peripheral in a low-power or disabled state after
  `spi_release()`.

### Generic high-level shape

```c
spi_init(bus);
spi_init_cs(bus, cs);

spi_acquire(bus, cs, SPI_MODE_0, SPI_CLK_1MHZ);
spi_transfer_bytes(bus, cs, false, tx_buf, rx_buf, len);
spi_release(bus);
```

In the CPU driver, this means that `spi_init()`, `spi_init_cs()`,
`spi_acquire()`, `spi_transfer_bytes()`, and `spi_release()` must work together
as one consistent transaction model.

If the hardware supports DMA or FIFOs, those can be used internally, but the
transaction semantics of the RIOT API should stay intact.

## I2C

**RIOT API:** `drivers/include/periph/i2c.h`

RIOT's I2C API is also transaction-based. Similar to SPI, the bus should
usually be powered only while a transaction is active.

GPIO support matters here as well because the bus pins must be configured
correctly, pull-ups may need to be selected, and some ports implement bus
recovery or reconfiguration using GPIO-level control of SDA/SCL.

### Typical functionality to implement

- `i2c_init()`,
- `i2c_acquire()` and `i2c_release()`,
- read and write primitives such as `i2c_read_bytes()` and `i2c_write_bytes()`,
- support for flags such as `I2C_NOSTOP`, `I2C_NOSTART`, `I2C_ADDR10`, and
  `I2C_REG16` if the hardware can provide them or if they can be emulated
  correctly.

### What the implementation generally needs to do

- configure the bus speed and pins,
- enable the I2C peripheral clock or power domain when the bus is acquired,
- select the clock divider or timing register values for the requested bus
  speed,
- enable pull-ups as appropriate for the platform,
- serialize access to the shared controller,
- generate START, RESTART, and STOP conditions correctly,
- translate RIOT flags into controller transactions,
- power the peripheral up in `i2c_acquire()` and down again in
  `i2c_release()` where practical.

### Generic high-level shape

```c
i2c_init(dev);

i2c_acquire(dev);
i2c_read_bytes(dev, addr, data, len, 0);
i2c_release(dev);
```

In the CPU driver, `i2c_init()`, `i2c_acquire()`, `i2c_read_bytes()`,
`i2c_write_bytes()`, and `i2c_release()` should implement correct START,
RESTART, ACK/NACK, and STOP handling for the underlying controller.

Correct handling of repeated starts, ACK/NACK conditions, and bus-busy recovery
is more important than adding optional optimizations early.

## General Recommendations

### Start small

Bring up the CPU with the smallest possible configuration first:

- boot to `cpu_init()`,
- confirm the vector table works,
- get `examples/basic/hello-world` running over UART,
- validate one timer instance so `ztimer` works,
- then implement the remaining peripherals incrementally.

### Reuse common code

If the MCU belongs to an already supported architecture or family, reuse the
existing common code where possible. For this case, a base directory (e.g. `cpu/<CPU_FAM>_common`) should be created that contains all shared/common configurations. This drastically reduces maintenance and lowers the
risk of subtle startup or interrupt bugs.

### Refer to the manual of the target CPU

Many CPUs are shipped with detailed manuals containing register-level information about peripherals, clock trees, the boot sequence, interrupt handling, and more. This information should be read, understood, and then mapped to the RIOT framework.

### Keep board details out of the CPU driver

The CPU driver should not assume a specific LED pin, shield, or sensor layout.
Those belong in the board port. The CPU driver should consume generic
configuration from `periph_conf.h` and `periph_cpu.h`.

### Match the API exactly

Before implementing a peripheral, read the corresponding header in
`drivers/include/periph/`. The header documents not only the function
signatures, but also the expected semantics, error handling, and power
management model.

### Validate with basic applications

Useful first tests are:

- `examples/basic/hello-world` for startup and UART,
- `tests/periph_gpio` or GPIO-based samples for pin control,
- timer-based examples for `periph_timer` and `ztimer`,
- a small SPI or I2C loopback / device test once the bus drivers are added.

## Further Reading

- RIOT guide: board porting (`doc/guides/advanced_tutorials/porting_boards.md`)
- RIOT peripheral APIs in `drivers/include/periph/`
- architecture-common code such as `cpu/cortexm_common/` when porting
  Cortex-M-based MCUs
