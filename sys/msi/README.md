<!--
SPDX-FileCopyrightText: 2026 Ajit Upadhyay <u.ajiit@gmail.com>
SPDX-License-Identifier: LGPL-2.1-only
-->

# MSI-like Mailbox Doorbell for RIOT

This module implements an **architecture-independent software MSI-like
protocol**. A sender writes a message into a mailbox slot, then asks an
architecture backend to ring a doorbell. The receiving interrupt handler
reads the mailbox.

The current backend uses a Cortex-M NVIC pending interrupt. Other
architectures can provide the same backend interface without changing the
mailbox protocol.

It is **not** PCI Express MSI/MSI-X. On STM32, it is also unrelated to the MSI
(Multi-Speed Internal) oscillator.

The idea copied from PCIe MSI is:

> Notify the CPU with a memory write, instead of a dedicated interrupt wire
> per event source.

| Document | Contents |
|---|---|
| [PROTOCOL.md](PROTOCOL.md) | Message format, post/ack rules, error cases |
| [ARCHITECTURE.md](ARCHITECTURE.md) | Layers, components, IRQ path, mapping to real MSI |
| [msi.h](../include/msi.h) | Public C API |
| [msi_arch.h](../include/msi_arch.h) | Architecture backend contract |

## How the protocol works

There are four vectors (slots) by default. Each vector is one independent
`{mailbox, handler}` pair, like one MSI-X table entry.

```text
Sender thread                         Shared mailbox                 Receiver
-------------                         --------------                 --------
1. wait until slot.valid == 0
2. write event, data, seq
3. write slot.valid = 1      ------>  payload is published
4. msi_arch_trigger()        ------>  backend doorbell
                                                             backend delivers IRQ
                                                             msi_isr()
                                                             copy event/data
                                                             slot.valid = 0
                                                             callback()
```

The important rule is publish-then-notify:

1. Write the payload first.
2. Set `valid` last.
3. Then ring the doorbell.

The ISR treats `valid != 0` as “a message is present”. Clearing `valid` is the
acknowledgement that frees the slot for the next post.

## Cortex-M example

```c
#include "cpu.h"
#include "msi.h"

static void on_msi(unsigned vec, uint32_t event, uint32_t data, void *arg)
{
    (void)arg;
    /* runs in ISR context */
    printf("vec=%u event=%lu data=0x%lx\n",
           vec, (unsigned long)event, (unsigned long)data);
}

/* Must match the IRQ passed to msi_init() */
void isr_hash_rng(void)
{
    msi_isr();
}

int main(void)
{
    msi_init(HASH_RNG_IRQn);
    msi_register(0, on_msi, NULL);
    msi_post(0, 1, 0xA001);   /* write mailbox + ring doorbell */
}
```

On STM32F746 the Cortex-M backend interprets `HASH_RNG_IRQn` as the doorbell
identifier. Any unused NVIC line can be used if the application also
implements the matching `isr_*` function.

## Build the demo

```bash
cd tests/sys/msi
make BOARD=nucleo-f746zg flash term
```

The application creates a thread that calls `msi_post()`. The ISR prints the
payload. Expected result: `TEST PASSED` after four posts.

## Requirements

- A CPU architecture providing the `msi_arch` feature
- An architecture-specific doorbell owned by the application
- A doorbell handler that calls `msi_isr()`

The provided Cortex-M backend requires an unused NVIC IRQ. Future GIC,
RISC-V software-interrupt, native, or hardware-mailbox backends can implement
`msi_arch_init()` and `msi_arch_trigger()` separately.

## What this is for

Use it to study message-signaled interrupts on a normal MCU:

- mailbox + doorbell instead of a GPIO/UART/timer wire
- several software vectors sharing one IRQ
- thread-to-ISR notification without a dedicated peripheral

It does not implement GIC, ITS, or a PCIe root complex.
