<!--
SPDX-FileCopyrightText: 2026 Ajit Upadhyay <u.ajiit@gmail.com>
SPDX-License-Identifier: LGPL-2.1-only
-->

# Architecture

How the architecture-independent MSI-like mailbox, its backend contract, the
Cortex-M NVIC backend, and the nucleo-f746zg demo fit together.

## 1. Place in RIOT

```text
+---------------------------+
| Application / test        |  tests/sys/msi
|  thread posts             |
|  isr_rng()                |
+-------------+-------------+
              |
              v
+---------------------------+
| sys/msi                   |  generic protocol + mailbox
|  msi_post()               |
|  msi_isr()                |
|  msi_register()           |
+-------------+-------------+
              |
              v
+---------------------------+
| msi_arch backend API      |  msi_arch_init()
|                           |  msi_arch_trigger()
+-------------+-------------+
              |
              v
+---------------------------+
| cpu/cortexm_common        |  current backend
|  NVIC_EnableIRQ()         |
|  NVIC_SetPendingIRQ()     |
+-------------+-------------+
              |
              v
+---------------------------+
| STM32F746 / Cortex-M7     |
|  NVIC + RNG vector        |
+---------------------------+
```

The protocol lives in `sys/` and does not include CMSIS or directly access an
interrupt controller. Architecture-specific notification lives under `cpu/`.
The board/application-specific part is:

- which backend doorbell identifier is selected
- how that doorbell handler reaches `msi_isr()`

The generic module requires the `msi_arch` feature. Cortex-M currently
provides it. Native, RISC-V, GIC, or hardware-mailbox implementations can add
the same feature after implementing the backend interface.

## 2. Components

```text
                    msi_init(doorbell)
                          |
          +---------------+---------------+
          |                               |
          v                               v
   clear _slots[]               msi_arch_init(doorbell)
   _initialized = true

                    msi_register(vec, cb, arg)
                          |
                          v
                   _handlers[vec]

                    msi_post(vec, event, data)
                          |
          +---------------+---------------+
          |                               |
          v                               v
   _slots[vec] = {event,data,seq,valid=1}   msi_arch_trigger()

                    CPU exception
                          |
                          v
                   isr_rng()          (application)
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
| `_initialized` | `sys/msi/msi.c` | Generic backend readiness state |
| `msi_post()` | `sys/msi/msi.c` | Sender side of the protocol |
| `msi_isr()` | `sys/msi/msi.c` | Receiver side of the protocol |
| `msi_arch_init()` | CPU backend | Configure a doorbell |
| `msi_arch_trigger()` | CPU backend | Ring the configured doorbell |
| `isr_*` | application | Bind NVIC vector to `msi_isr()` |
| callback | application | Handle one posted message |

## 3. Interrupt path on nucleo-f746zg

```text
msi_poster thread                    generic layer
    |
    | msi_post(vec, event, data)
    v
slot[vec].valid = 1
    |
    | msi_arch_trigger()
    v
Cortex-M backend
    |
    | NVIC_SetPendingIRQ(RNG_IRQn)
    v
+------------------+
|      NVIC        |
|  RNG_IRQn        |   unused peripheral line, used as software IRQ
+--------+---------+
         |
         | exception entry (handler mode)
         v
  isr_rng()
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

`RNG_IRQn` is interpreted only by the Cortex-M backend. It is not special; it
is an unused vector selected so the demo does not steal USART, EXTI, or TIM
IRQs that RIOT already uses.

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

One backend doorbell is shared. The vector ID is the slot index, not a second
hardware interrupt.

## 5. Concurrency model

```text
Thread mode                         Handler mode
-----------                         ------------
msi_post()                          msi_isr()
  irq_disable()                       read valid
  check/write slot                    copy event/data
  msi_arch_trigger()                  valid = 0
  irq_restore()  ------------------>  callback()
```

Same core, two contexts:

- The critical section makes `valid` a single-producer occupancy flag.
- The ISR is the consumer.
- Two threads posting the **same** vector are serialized by
  `irq_disable()`. The loser sees `-EBUSY` if the slot is still full.
- Two threads posting **different** vectors can both succeed; one doorbell
  run drains both slots.

This generic mailbox is not SMP-safe. A second core would need atomic
publication plus suitable cache coherency/maintenance. A hardware
mailbox/FIFO backend alone does not make the shared slots SMP-safe.

## 6. Mapping onto the interrupt learning path

From the original RIOT interrupt note:

```text
GPIO IRQ / NVIC     already in RIOT (periph_gpio)
        |
        v
Software interrupt  Cortex-M backend (NVIC_SetPendingIRQ)
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

Architecturally the generic module is the mailbox protocol. The current
Cortex-M backend places it at “software interrupt + mailbox”, not at PCIe.

```text
Generic MSI-like flow
---------------------
Sender ----write----> slot ----msi_arch_trigger----> backend ----> ISR

Current Cortex-M backend
------------------------
msi_arch_trigger ----NVIC ISPR write----> NVIC ----> ISR
```

## 7. File layout

```text
sys/include/msi.h          public API
sys/include/msi_arch.h     private architecture backend contract
sys/msi/msi.c              mailbox, doorbell, ISR drain
sys/msi/Makefile           MODULE = msi
sys/msi/Makefile.dep       require msi_arch
sys/msi/README.md          start here
sys/msi/PROTOCOL.md        protocol rules
sys/msi/ARCHITECTURE.md    this file

cpu/cortexm_common/msi_arch.c
                           Cortex-M NVIC backend
cpu/cortexm_common/Makefile.features
                           provide msi_arch

tests/sys/msi/main.c       nucleo-f746zg demo
tests/sys/msi/Makefile     BOARD ?= nucleo-f746zg
tests/sys/msi/README.md    how to flash the demo
```

RIOT picks up `sys/msi` automatically when an app sets `USEMODULE += msi`.

## 8. Application contract

The generic module does not install an ISR symbol. Backends with static vector
tables therefore leave the final vector binding selectable per application.

The application must:

1. Choose a backend-specific doorbell identifier.
2. Call `msi_init(doorbell)`.
3. Connect the doorbell handler to `msi_isr()`.
4. Keep callbacks ISR-safe.

For Cortex-M with a static RIOT vector table, this means choosing an unused
`IRQn` and defining the matching `void isr_<name>(void)`.

If the application enables the real RNG peripheral IRQ while the demo owns
`RNG_IRQn`, the two uses conflict. Pick another unused line.

## 9. Limits that follow from the architecture

| Limit | Reason |
|---|---|
| Cortex-M is the only current backend | Other architectures do not provide `msi_arch` yet |
| One doorbell | Generic protocol shares one backend notification |
| One message per vector | Occupancy flag, not a ring buffer |
| Same-core only | Local IRQ exclusion is not an SMP lock |
| Demo prints in the ISR | Educational; production handlers should wake a thread |

To support another architecture, implement `msi_arch_init()` and
`msi_arch_trigger()`, provide the `msi_arch` feature, and connect that
architecture's doorbell handler to `msi_isr()`. The generic mailbox code
does not need to change.
