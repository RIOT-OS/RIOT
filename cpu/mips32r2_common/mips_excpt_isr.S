/*
 * Copyright 2014-2015, Imagination Technologies Limited and/or its
 *                      affiliated group companies.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/

#
# Keep each function in a separate named section
#define _FUNCTION_SECTIONS_
.set nomips16

#include <mips/regdef.h>
#include <mips/asm.h>
#include <mips/cpu.h>

#define VEC_SPACE (SZPTR * 8)

LEAF(__isr_vec)
	.set	push
	.set	noat
AENT(__isr_vec_sw0)
.weak   _mips_isr_sw0
	LA	k1, _mips_isr_sw0
	beqz	k1, 1f
	jr	k1
.org	VEC_SPACE
AENT(__isr_vec_sw1)
.weak   _mips_isr_sw1
	LA	k1, _mips_isr_sw1
	beqz	k1, 1f
	jr	k1
.org	2 * VEC_SPACE
AENT(__isr_vec_hw0)
.weak   _mips_isr_hw0
	LA	k1, _mips_isr_hw0
	beqz	k1, 1f
	jr	k1
.org	3 * VEC_SPACE
AENT(__isr_vec_hw1)
.weak   _mips_isr_hw1
	LA	k1, _mips_isr_hw1
	beqz	k1, 1f
	jr	k1
.org	4 * VEC_SPACE
AENT(__isr_vec_hw2)
.weak   _mips_isr_hw2
	LA	k1, _mips_isr_hw2
	beqz	k1, 1f
	jr	k1
.org	5 * VEC_SPACE
AENT(__isr_vec_hw3)
.weak   _mips_isr_hw3
	LA	k1, _mips_isr_hw3
	beqz	k1, 1f
	jr	k1
.org	6 * VEC_SPACE
AENT(__isr_vec_hw4)
.weak   _mips_isr_hw4
	LA	k1, _mips_isr_hw4
	beqz	k1, 1f
	jr	k1
.org	7 * VEC_SPACE
AENT(__isr_vec_hw5)
.weak   _mips_isr_hw5
	LA	k1, _mips_isr_hw5
	beqz	k1, 1f
	jr	k1
.org	8 * VEC_SPACE
AENT(__isr_vec_fallback)
.weak   _mips_interrupt
1:
	LA      k1, _mips_interrupt
	beqz    k1, 1b
	jr      k1
	.set    pop
END(__isr_vec)
