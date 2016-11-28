/*-
 * Copyright (c) 2013-2015 Marko Zec, University of Zagreb
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id$
 */

	// To use this file for Cinco purposes:
	// riscv32-unknown-elf-gcc -march=RV32IMAFC -m32 -c -o start.o start.S -I$CINCO/hardware/freedom_e/system/include/riscv/
	// riscv32-unknown-elf-ar rcs libcrt0.a start.o
	// cp libcrt0.a $CINCO/hardware/freedom_e/system/riscv/lib/
	
#include "encoding.h"	

/*
 * Clear BSS, set small data index register and jump into main().
 *
 */

/* Stack pointer isn't being set by bootloader ATM so set it here */

	.section .init
	.globl _start
	.type _start,@function

_start:
	la	gp, _gp	
	li	sp, 0x80004000  /* top of memory, FIXME! */
	la	s1, _end	/* End of BSS section, word aligned */
	la	s0, __bss_start	/* Start of BSS section, NOT word aligned */
	j	bss_bzero_enter

	/* The loader doesn't bzero the BSS, so we must do it here. */
bss_bzero_loop:
	sb	zero, (s0)
	addi	s0, s0, 1
bss_bzero_enter:
	bne	s0, s1, bss_bzero_loop

	la	s0, __init_array_start
	la	s1, __init_array_end
	move	s2, ra
	j	ctor_loop_enter

ctor_loop:
	lw	a0, (s0)
	addi	s0, s0, 4
	jalr	a0
ctor_loop_enter:
	bne	s0, s1,	ctor_loop

enable_fpu:

	li t0, MSTATUS_FS
	csrs mstatus, t0
	csrr t1, mstatus
	and t1, t1, t0
	beqz t1, 1f
	fssr x0
1:	


	move	ra, s2
	j	main
