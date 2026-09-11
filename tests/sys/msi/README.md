<!--
SPDX-FileCopyrightText: 2026 Ajit Upadhyay <u.ajiit@gmail.com>
SPDX-License-Identifier: LGPL-2.1-only
-->

# MSI-like mailbox test

Demo for the `msi` module on **nucleo-f746zg**.

A thread writes a generic mailbox slot. The Cortex-M backend then pends
`HASH_RNG_IRQn`, and the ISR prints the payload. This is a software MSI-like
doorbell, not PCI Express MSI.

Protocol and architecture:

- [sys/msi/README.md](../../../sys/msi/README.md)
- [sys/msi/PROTOCOL.md](../../../sys/msi/PROTOCOL.md)
- [sys/msi/ARCHITECTURE.md](../../../sys/msi/ARCHITECTURE.md)

## Build

```bash
make BOARD=nucleo-f746zg flash term
```

`BOARD` defaults to `nucleo-f746zg`. The generic module uses the Cortex-M NVIC
backend supplied by `cpu/cortexm_common`.

## What you should see

```text
MSI-like mailbox test
thread writes mailbox + doorbell, ISR prints the payload
[thread] post vec=1 event=1 data=0xa001
[ISR] doorbell-1 vec=1 event=1 data=0xa001
[thread] post vec=0 event=2 data=0xa002
[ISR] doorbell-0 vec=0 event=2 data=0xa002
[thread] post vec=1 event=3 data=0xa003
[ISR] doorbell-1 vec=1 event=3 data=0xa003
[thread] post vec=0 event=4 data=0xa004
[ISR] doorbell-0 vec=0 event=4 data=0xa004
done: ISR handled 4 of 4 posts
TEST PASSED
```

`[thread]` lines run in thread mode. `msi_post()` calls the generic
`msi_arch_trigger()` hook, whose Cortex-M implementation calls
`NVIC_SetPendingIRQ(HASH_RNG_IRQn)`. `[ISR]` lines run in
`isr_hash_rng()`.
