/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

@-----------------------------------------------------------@
@ Fast Block Copy (declared in diskio.h)
@-----------------------------------------------------------@

.global Copy_un2al
.arm
Copy_un2al:
	STMFD	SP!, {R4-R8}
	ANDS	IP, R1, #3
	BEQ	lb_align

	BIC	R1, #3
	MOV	IP, IP, LSL #3
	RSB	R8, IP, #32
	LDMIA	R1!, {R7}
lb_l1:	MOV	R3, R7
	LDMIA	R1!, {R4-R7}
	MOV	R3, R3, LSR IP
	ORR	R3, R3, R4, LSL R8
	MOV	R4, R4, LSR IP
	ORR	R4, R4, R5, LSL R8
	MOV	R5, R5, LSR IP
	ORR	R5, R5, R6, LSL R8
	MOV	R6, R6, LSR IP
	ORR	R6, R6, R7, LSL R8
	SUBS	R2, R2, #16
	STMIA	R0!, {R3-R6}
	BNE	lb_l1
	LDMFD	SP!, {R4-R8}
	BX	LR

lb_align:
	LDMIA	R1!, {R3-R6}
	SUBS	R2, R2, #16
	STMIA	R0!, {R3-R6}
	BNE	lb_align
	LDMFD	SP!, {R4-R8}
	BX	LR


.global Copy_al2un
.arm
Copy_al2un:
	STMFD	SP!, {R4-R8}
	ANDS	IP, R0, #3
	BEQ	sb_align

	MOV	IP, IP, LSL #3
	RSB	R8, IP, #32

	LDMIA	R1!, {R4-R7}
sb_p1:	STRB	R4, [R0], #1
	MOV	R4, R4, LSR #8
	TST	R0, #3
	BNE	sb_p1
	ORR	R4, R4, R5, LSL IP
	MOV	R5, R5, LSR R8
	ORR	R5, R5, R6, LSL IP
	MOV	R6, R6, LSR R8
	ORR	R6, R6, R7, LSL IP
	SUBS	R2, R2, #16
	STMIA	R0!, {R4-R6}

sb_l1:	MOV	R3, R7
	LDMIA	R1!, {R4-R7}
	MOV	R3, R3, LSR R8
	ORR	R3, R3, R4, LSL IP
	MOV	R4, R4, LSR R8
	ORR	R4, R4, R5, LSL IP
	MOV	R5, R5, LSR R8
	ORR	R5, R5, R6, LSL IP
	MOV	R6, R6, LSR R8
	ORR	R6, R6, R7, LSL IP
	SUBS	R2, R2, #16
	STMIA	R0!, {R3-R6}
	BNE	sb_l1

	MOV	R7, R7, LSR R8
sb_p2:	SUBS	IP, IP, #8
	STRB	R7, [R0], #1
	MOV	R7, R7, LSR #8
	BNE	sb_p2

	LDMFD	SP!, {R4-R8}
	BX	LR

sb_align:
	LDMIA	R1!, {R3-R6}
	SUBS	R2, #16
	STMIA	R0!, {R3-R6}
	BNE	sb_align
	LDMFD	SP!, {R4-R8}
	BX	LR

.end
