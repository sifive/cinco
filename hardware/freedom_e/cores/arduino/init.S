/* See LICENSE for license details. */

	.section .init
	/* init_array/fini_array are used instead */
	.globl _init
	.globl _fini
	.type _init,@function
	.type _fini,@function
_init:
_fini:
	ret
