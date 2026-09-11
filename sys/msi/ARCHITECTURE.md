<!--
SPDX-FileCopyrightText: 2026 Ajit Upadhyay <u.ajiit@gmail.com>
SPDX-License-Identifier: LGPL-2.1-only
-->

# Architecture

How the MSI-like mailbox fits into RIOT, the Cortex-M NVIC, and the
nucleo-f746zg demo.

## 1. Place in RIOT

```text
+---------------------------+
| Application / test        |  tests/sys/msi
|  thread posts             |
|  isr_hash_rng()           |
+-------------+-------------+
              |
              v
+---------------------------+
| sys/msi                   |  protocol + mailbox
|  msi_post()               |
|  msi_isr()                |
|  msi_register()           |
+-------------+-------------+
              |
              v
+---------------------------+
| RIOT CPU / CMSIS          |  cpu.h, irq.h
|  NVIC_EnableIRQ()         |
|  NVIC_SetPendingIRQ()     |
+-------------+-------------+
              |
              v
+---------------------------+
| STM32F746 / Cortex-M7     |
|  NVIC + HASH_RNG vector   |
+---------------------------+
```

The module lives in `sys/` because it is a generic Cortex-M helper, not a
board driver. The board-specific part is only:

- which `IRQn` is used as the doorbell
- which `isr_*` symbol the application defines

`FEATURES_REQUIRED += cpu_core_cortexm` keeps it off native, AVR, and RISC-V
until those backends exist.

## 2. Components

```text
                    msi_init(irqn)
                          |
          +---------------+---------------+
          |                               |
          v                               v
   clear _slots[]                  NVIC_EnableIRQ(irqn)
   _irqn = irqn

                    msi_register(vec, cb, arg)
                          |
                          v
                   _handlers[vec]

                    msi_post(vec, event, data)
                          |
          +---------------+---------------+
          |                               |
          v                               v
   _slots[vec] = {event,data,seq,valid=1}   NVIC ISPR (doorbell)

                    CPU exception
                          |
                          v
                   isr_hash_rng()     (application)
                          |
                          v
                     msi_isr()        (module)
                          |
                          v
                   _handlers[vec].cb  (application, ISR context)
```

| Symbol | Location | Responsibility |
|---|---|---|
| `_slots[]` | `sys/msi/msi.c` | Protocol mailbox |
| `_handlers[]` | `sys/msi/msi.c` | Vector-to-callback map |
| `_irqn` | `sys/msi/msi.c` | Shared doorbell IRQ |
| `msi_post()` | `sys/msi/msi.c` | Sender side of the protocol |
| `msi_isr()` | `sys/msi/msi.c` | Receiver side of the protocol |
| `isr_*` | application | Bind NVIC vector to `msi_isr()` |
| callback | application | Handle one posted message |

## 3. Interrupt path on nucleo-f746zg

```text
msi_poster thread
    |
    | msi_post(vec, event, data)
    v
slot[vec].valid = 1
    |
    | write NVIC->ISPR  (HASH_RNG pending)
    v
+------------------+
|      NVIC        |
|  HASH_RNG_IRQn   |   unused peripheral line, used as software IRQ
+--------+---------+
         |
         | exception entry (handler mode)
         v
  isr_hash_rng()
         |
         v
     msi_isr()
         |
         +-- slot 0 posted? -> callback "doorbell-0"
         +-- slot 1 posted? -> callback "doorbell-1"
         +-- slot 2,3 unused
         |
         | exception return
         v
msi_poster continues
```

`HASH_RNG_IRQn` is not special. It is only an unused vector so the demo does
not steal USART, EXTI, or TIM IRQs that RIOT already uses.

`cortexm_init()` already programs every NVIC line to
`CPU_DEFAULT_IRQ_PRIO`. The module only enables the line.

## 4. Data structures

```text
_slots[MSI_VEC_NUMOF]
+--------+--------+--------+--------+
| valid  | seq    | event  | data   |   vec 0
+--------+--------+--------+--------+
| valid  | seq    | event  | data   |   vec 1
+--------+--------+--------+--------+
| ...                               |

_handlers[MSI_VEC_NUMOF]
+--------+--------+
| cb     | arg    |   vec 0
+--------+--------+
| cb     | arg    |   vec 1
+--------+--------+
```

Slots are the **protocol state**.  
Handlers are **local dispatch**, not part of the on-wire / in-memory
message.

One doorbell IRQ is shared. The vector id is the slot index, not a second
hardware IRQ.

## 5. Concurrency model

```text
Thread mode                         Handler mode
-----------                         ------------
msi_post()                          msi_isr()
  irq_disable()                       read valid
  check/write slot                    copy event/data
  NVIC_SetPendingIRQ                  valid = 0
  irq_restore()  ------------------>  callback()
```

Same core, two contexts:

- The critical section makes `valid` a single-producer occupancy flag.
- The ISR is the consumer.
- Two threads posting the **same** vector are serialized by
  `irq_disable()`. The loser sees `-EBUSY` if the slot is still full.
- Two threads posting **different** vectors can both succeed; one doorbell
  run drains both slots.

This is not SMP-safe. A second core would need cache maintenance or a
hardware mailbox/FIFO (for example the RP2350 SIO FIFO).

## 6. Mapping onto the interrupt learning path

From the original RIOT interrupt note:

```text
GPIO IRQ / NVIC     already in RIOT (periph_gpio)
        |
        v
Software interrupt  this module (NVIC_SetPendingIRQ)
        |
        v
Mailbox / doorbell  this protocol
        |
        v
Multicore IPI       not implemented (F746 is single core)
        |
        v
GIC / ITS / PCIe MSI  not in this tree
```

Architecturally the module sits at “software interrupt + mailbox”, not at
PCIe.

```text
Traditional MCU IRQ                 This module
-------------------                 -----------
Peripheral ----wire----> NVIC       Thread ----write----> slot
                                      |                    |
                                      +----NVIC ISPR-------+
                                               |
                                               v
                                              ISR
```

## 7. File layout

```text
sys/include/msi.h          public API
sys/msi/msi.c              mailbox, doorbell, ISR drain
sys/msi/Makefile           MODULE = msi
sys/msi/Makefile.dep       require cpu_core_cortexm
sys/msi/README.md          start here
sys/msi/PROTOCOL.md        protocol rules
sys/msi/ARCHITECTURE.md    this file

tests/sys/msi/main.c       nucleo-f746zg demo
tests/sys/msi/Makefile     BOARD ?= nucleo-f746zg
tests/sys/msi/README.md    how to flash the demo
```

RIOT picks up `sys/msi` automatically when an app sets `USEMODULE += msi`.

## 8. Application contract

The module does not install an ISR symbol itself. That keeps the doorbell
IRQ selectable per board.

The application must:

1. Choose an unused `IRQn`.
2. Call `msi_init(irqn)`.
3. Define the matching `void isr_<name>(void)` from the STM32 vector table.
4. Call `msi_isr()` from that function.
5. Keep callbacks ISR-safe.

If the application enables the real HASH/RNG peripheral IRQ while the demo
owns `HASH_RNG_IRQn`, the two uses conflict. Pick another unused line.

## 9. Limits that follow from the architecture

| Limit | Reason |
|---|---|
| Cortex-M only | Uses CMSIS `NVIC_*` |
| One doorbell IRQ | Simple NVIC binding |
| One message per vector | Occupancy flag, not a ring buffer |
| Same-core only | No cache/IPI path |
| Demo prints in the ISR | Educational; production handlers should wake a thread |

To grow this toward real MSI later, keep the mailbox protocol and replace
only the doorbell backend: NVIC ISPR today, a hardware mailbox next, GIC
ITS / PCIe write on a future ARM64 port.
