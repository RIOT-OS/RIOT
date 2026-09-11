<!--
SPDX-FileCopyrightText: 2026 Ajit Upadhyay <u.ajiit@gmail.com>
SPDX-License-Identifier: LGPL-2.1-only
-->

# MSI-like Mailbox Doorbell for RIOT

This module is a **software MSI-like protocol** for Cortex-M. A sender writes a
message into a mailbox slot, then rings a doorbell. The doorbell is an NVIC
pending bit. The CPU takes the IRQ and the ISR reads the mailbox.

It is **not** PCI Express MSI/MSI-X.  
It is **not** the STM32 MSI (Multi-Speed Internal) oscillator.

The idea copied from PCIe MSI is:

> Notify the CPU with a memory write, instead of a dedicated interrupt wire
> per event source.

| Document | Contents |
|---|---|
| [PROTOCOL.md](PROTOCOL.md) | Message format, post/ack rules, error cases |
| [ARCHITECTURE.md](ARCHITECTURE.md) | Layers, components, IRQ path, mapping to real MSI |
| [msi.h](../include/msi.h) | Public C API |

## How the protocol works

There are four vectors (slots) by default. Each vector is one independent
`{mailbox, handler}` pair, like one MSI-X table entry.

```text
Sender thread                         Shared mailbox                 Receiver
-------------                         --------------                 --------
1. wait until slot.valid == 0
2. write event, data, seq
3. write slot.valid = 1      ------>  payload is published
4. NVIC_SetPendingIRQ()      ------>  doorbell
                                                             NVIC delivers IRQ
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

## Quick example

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

On STM32F746 the test uses unused `HASH_RNG_IRQn` as the doorbell. Any unused
NVIC line can be used if you also implement the matching `isr_*` function.

## Build the demo

```bash
cd tests/sys/msi
make BOARD=nucleo-f746zg flash term
```

The application creates a thread that calls `msi_post()`. The ISR prints the
payload. Expected result: `TEST PASSED` after four posts.

## Requirements

- Cortex-M (`cpu_core_cortexm`)
- An unused NVIC IRQ that the application owns
- Application provides the CPU ISR and calls `msi_isr()`

## What this is for

Use it to study message-signaled interrupts on a normal MCU:

- mailbox + doorbell instead of a GPIO/UART/timer wire
- several software vectors sharing one IRQ
- thread-to-ISR notification without a dedicated peripheral

It does not implement GIC, ITS, or a PCIe root complex.
