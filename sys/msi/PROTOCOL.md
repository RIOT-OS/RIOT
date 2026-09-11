<!--
SPDX-FileCopyrightText: 2026 Ajit Upadhyay <u.ajiit@gmail.com>
SPDX-License-Identifier: LGPL-2.1-only
-->

# MSI-like Protocol

This document specifies the software protocol implemented by `sys/msi`.

The protocol name is **MSI-like mailbox doorbell**. A writer posts a word into
shared memory, then asks an architecture backend to trigger a doorbell. The
backend delivers an interrupt whose handler drains the mailbox.

## 1. Goal

Deliver a small event from a thread (or another ISR of equal or lower
priority) to a handler that runs in interrupt context, without using a
dedicated peripheral IRQ wire for that event.

PCIe MSI does this with a posted memory write of programmed `{address, data}`.
This protocol does the same job with:

| PCIe MSI | This protocol |
|---|---|
| MSI address | mailbox slot + architecture doorbell |
| MSI data | `event` and `data` fields |
| MSI / MSI-X vector | slot index `vec` |
| Posted write | `valid = 1` then `msi_arch_trigger()` |
| Host ISR | `msi_isr()` + registered callback |

## 2. Roles

| Role | Who | Duty |
|---|---|---|
| Sender | Thread (or other context that may post) | Fill a free slot, then ring the doorbell |
| Mailbox | Shared SRAM slots | Hold one posted message per vector |
| Doorbell backend | Architecture implementation | Turn the post into a CPU interrupt |
| Receiver | `msi_isr()` and the vector callback | Consume the slot and handle the event |

On nucleo-f746zg, the supplied Cortex-M backend uses an NVIC line and the
sender and receiver are the **same Cortex-M7**. The protocol still has two
contexts: thread mode and handler mode.

## 3. Vectors

A **vector** is one mailbox slot plus one optional callback.

```text
vec 0  -->  slot[0]  -->  handler[0]
vec 1  -->  slot[1]  -->  handler[1]
vec 2  -->  slot[2]  -->  handler[2]
vec 3  -->  slot[3]  -->  handler[3]
```

Default width is `MSI_VEC_NUMOF = 4`. Override it at compile time:

```make
CFLAGS += -DMSI_VEC_NUMOF=8
```

All vectors share **one** doorbell IRQ. The ISR scans every slot. That is
closer to “one MSI address, several data/vector values” than to full MSI-X
(independent address+data per vector).

## 4. Message format

Each slot is:

```c
typedef struct {
    volatile uint32_t valid;   /* 0 = empty, 1 = posted */
    volatile uint32_t seq;     /* incremented on every successful post */
    volatile uint32_t event;   /* sender event code */
    volatile uint32_t data;    /* sender payload */
} msi_slot_t;
```

| Field | Size | Meaning |
|---|---|---|
| `valid` | 32-bit | Publication flag. `0` free, `1` occupied |
| `seq` | 32-bit | Monotonic counter per slot, for debug / ordering |
| `event` | 32-bit | Application event identifier |
| `data` | 32-bit | Application payload |

There is no checksum, version field, or routing header. The vector index is
the address of the message.

`event` and `data` are opaque to the protocol. The module does not interpret
them.

## 5. Doorbell backend

The generic protocol calls two architecture hooks:

```c
int msi_arch_init(int doorbell);
void msi_arch_trigger(void);
```

The meaning of `doorbell` is backend-specific. `msi_post()` does not access an
interrupt controller directly.

### Cortex-M backend

For Cortex-M, the doorbell identifier is an external IRQ number. Ringing it
uses:

```text
NVIC_SetPendingIRQ(doorbell)
```

That write is the MSI-like act: a memory-mapped store into the NVIC ISPR
makes the CPU take an interrupt.

The application must implement the matching RIOT ISR and call `msi_isr()`:

```c
void isr_hash_rng(void)
{
    msi_isr();
}
```

On STM32F746 the demo uses `HASH_RNG_IRQn` because the HASH/RNG hardware IRQ
is unused. Any unused `IRQn` is valid if the `isr_*` name matches the
generated vector table.

## 6. State machine (one vector)

```text
              msi_init()
                 |
                 v
              EMPTY  (valid = 0)
                 |
                 |  sender: write event, data, seq
                 |  sender: valid = 1
                 |  sender: msi_arch_trigger
                 v
              POSTED (valid = 1)
                 |
                 |  ISR: copy event, data
                 |  ISR: valid = 0
                 |  ISR: callback(vec, event, data, arg)
                 v
              EMPTY
```

A second `msi_post()` on the same vector while the slot is `POSTED` returns
`-EBUSY`. The sender must wait until the ISR has cleared `valid`.

## 7. Post procedure (sender)

`msi_post(vec, event, data)` is the complete send path.

```text
if vec is out of range or doorbell not initialized:
    return -EINVAL

disable IRQs
if slot.valid != 0:
    restore IRQs
    return -EBUSY

slot.event = event
slot.data  = data
slot.seq   = slot.seq + 1
slot.valid = 1              # publish last
msi_arch_trigger()          # architecture doorbell
restore IRQs
return 0
```

Rules:

1. **Payload before `valid`.** The ISR may observe `valid == 1` only after
   `event` and `data` are written.
2. **`valid` before doorbell.** The pending bit must not be set on an empty
   slot.
3. **One message per slot.** No queue. Occupied slot is back-pressure.
4. **IRQs disabled during the publish window.** That makes the check-and-set
   of `valid` atomic against `msi_isr()` on the same core.

After `irq_restore()`, the backend can deliver the interrupt. With the
Cortex-M backend, the callback usually runs before `msi_post()` returns to
the caller.

## 8. Receive procedure (ISR)

`msi_isr()` is the complete receive path.

```text
for vec in 0 .. MSI_VEC_NUMOF-1:
    if slot[vec].valid == 0:
        continue
    event = slot[vec].event
    data  = slot[vec].data
    cb    = handler[vec].cb
    arg   = handler[vec].arg
    slot[vec].valid = 0     # ack / free slot
    if cb:
        cb(vec, event, data, arg)
```

Rules:

1. **Scan all vectors.** One doorbell can retire several posts if they were
   published before the ISR ran.
2. **Ack before callback.** `valid` is cleared first so a callback that posts
   again on the same vector can succeed.
3. **No callback still consumes the slot.** An unregistered vector will not
   stay stuck in `POSTED`.
4. **Callback runs in interrupt context.** It must be short. Do not block.
   `printf` is acceptable only in the demo.

## 9. Setup procedure

Before any post:

```text
msi_init(doorbell)              # clear slots, initialize backend
msi_register(vec, cb, arg)      # optional per vector, but required to handle
connect the doorbell handler to msi_isr()
```

`msi_register()` may be called again to replace a handler. `cb` must not be
`NULL`.

`msi_init()` may be called again. It clears every slot and asks the backend to
switch the doorbell.

## 10. Ordering and memory

On a single-core target:

- `irq_disable()` / `irq_restore()` give a critical section between thread
  and ISR.
- `volatile` on slot fields stops the compiler from caching them.

The generic implementation does not provide cross-core synchronization or
flush data caches. It is enough for same-core thread-to-ISR use. It is **not**
enough for another CPU or a bus-master device writing the mailbox unless a
future implementation adds atomics, cache maintenance, or places the mailbox
in coherent/non-cacheable memory.

`seq` is incremented under the same critical section as the post. It is not
exposed in the callback; it is only stored in the slot.

## 11. Errors

| Result | When |
|---|---|
| `0` | Success |
| `-EINVAL` | Bad vector, `cb == NULL`, doorbell not initialized, or backend rejects its identifier |
| `-EBUSY` | Slot still `POSTED` (`valid == 1`) |

There is no timeout inside `msi_post()`. The sender retries or waits.

## 12. What the protocol does not do

- No multi-producer queue on one vector
- No flow-control window larger than one slot
- No addressing beyond `vec`
- No encryption, authentication, or CRC
- No PCIe configuration space, MSI enable bit, or MSI-X table
- No GIC SGI / ITS backend yet
- No inter-processor interrupt backend yet

Those can be layered later. The protocol itself is only **post one word,
ring one doorbell, ack by clearing `valid`**.

## 13. Worked example (nucleo-f746zg test)

The test thread posts four messages, alternating vectors 0 and 1:

```text
post(1, event=1, data=0xA001)
post(0, event=2, data=0xA002)
post(1, event=3, data=0xA003)
post(0, event=4, data=0xA004)
```

Each post:

```text
[thread]  write slot + msi_arch_trigger()
[backend] NVIC_SetPendingIRQ(HASH_RNG)
[NVIC]    take HASH_RNG
[isr]     isr_hash_rng() -> msi_isr()
[ISR]     print doorbell-N, vec, event, data
[ISR]     valid = 0
[thread]  msi_post() returns 0
[thread]  sleep 200 ms
```

If all four callbacks ran, the test prints `TEST PASSED`.
