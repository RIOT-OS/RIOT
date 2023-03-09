.equiv  T_REG_BASE,       0x04000000
.equiv  T_REG_IE,         0x04000200
.equiv  T_REG_IF,         0x04000202
.equiv  T_REG_WAITCNT,    0x04000204
.equiv  T_REG_IME,        0x04000208
.equiv  T_REG_IFBIOS,     0x03FFFFF8

.linkage=global
.align 2
.arm
.global isrColor
.func   isrColor
isrColor:
    mov         r1, #T_REG_BASE

    @ Get IE & IF
    ldr         r0, [r1, #T_REG_IE - T_REG_BASE]!
    and         r0, r0, r0, lsr #16

    @ Ack IF
    strh        r0, [r1, #T_REG_IF  - T_REG_IE]

    mov	r3, #0x5000000
    ldrh	r3, [r3]
    mov	r2, #0x5000000
    add	r3, r3, #1
    lsl	r3, r3, #16
    lsr	r3, r3, #16
    strh	r3, [r2]

    @ Ack IFBIOS
    ldr         r2, [r1, #T_REG_IFBIOS - T_REG_IE]!
    orr         r2, r2, r0
    strh        r2, [r1], #8

    bx          lr
.endfunc

.linkage=global
.align 2
.arm
.global isrStub
.func   isrStub
isrStub:
    mov         r1, #T_REG_BASE

    @ Get IE & IF
    ldr         r0, [r1, #T_REG_IE - T_REG_BASE]!
    and         r0, r0, r0, lsr #16

    @ Ack IF
    strh        r0, [r1, #T_REG_IF  - T_REG_IE]

    @ Ack IFBIOS
    ldr         r2, [r1, #T_REG_IFBIOS - T_REG_IE]!
    orr         r2, r2, r0
    strh        r2, [r1], #8

    bx          lr
.endfunc
