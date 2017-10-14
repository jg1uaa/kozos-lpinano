#include "armpsr.h"

/* 割り込みベクタ：例外要因に対する処理を行う */

	.section .vector, "ax"
	.arm
	.align 0
	.global __start
__start:
	b	start_entry
undef_vector:
	b	undef_entry
swi_vector:
	b	swi_entry
prefetch_abort_vector:
	b	prefetch_abort_entry
data_abort_vector:
	b	data_abort_entry
reserved_vector:
	b	reserved_vector		// 非対応
irq_vector:
	b	irq_entry
fiq_vector:
	b	fiq_vector		// 非対応

/* スタートアップ */

	.section .text
	.arm
	.align 0
	.global start_entry

start_entry:
	/* 各種スタックを初期化 */
	msr	cpsr_fsxc, #(PSR_M_SYS | PSR_I | PSR_F)
	ldr	sp, =_sys_stack
	msr	cpsr_fsxc, #(PSR_M_UND | PSR_I | PSR_F)
	ldr	sp, =_und_stack
	msr	cpsr_fsxc, #(PSR_M_ABT | PSR_I | PSR_F)
	ldr	sp, =_abt_stack
	msr	cpsr_fsxc, #(PSR_M_FIQ | PSR_I | PSR_F)
	ldr	sp, =_fiq_stack
	msr	cpsr_fsxc, #(PSR_M_IRQ | PSR_I | PSR_F)
	ldr	sp, =_irq_stack
	msr	cpsr_fsxc, #(PSR_M_SVC | PSR_I | PSR_F)
	ldr	sp, =_svc_stack

	/* ベクタテーブルアドレス(VBAR)を設定 */
	ldr	r0, =__start
	mcr	p15, 0, r0, c12, c0, 0

	/* vector remappingの有効化、MMU無効化 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #(1 << 24)	/* clear SCTLR.VE */
	bic	r0, r0, #(1 << 13)	/* clear SCTLR.V */
	mcr	p15, 0, r0, c1, c0, 0
	isb

	/* OS本体はSVCモードで動作し、ユーザプログラムはSYSモードで動作する */
start_main:
	mov	r0, #0
	bl	main

	/* 暴走防止用の無限ループ */
1:
	b	1b

	.pool

/*
 * コンテキストのレイアウト
 *
 * 以下の順番でコンテキスト（レジスタ）を保存する。
 *
 *	     +-------------+
 *	+  0 | SPSR_xxx    |	xxx: 例外発生時のCPUモード
 *	+  4 | R0          |
 *	+  8 | R1          |
 *	+ 12 | R14_xxx(PC) |
 *	     +=============+
 *	+ 16 | R2          |
 *	+ 20 | R3          |
 *	+ 24 | R4          |
 *	+ 28 | R5          |
 *	+ 32 | R6          |
 *	+ 36 | R7          |
 *	+ 40 | R8          |
 *	+ 44 | R9          |
 *	+ 48 | R10         |
 *	+ 52 | R11         |
 *	+ 56 | R12         |
 *	+ 60 | R14_sys     |
 *	     +-------------+
 */

/*
 * 例外処理用マクロ：例外処理開始（コンテキスト保存）
 *
 * R13_xxxレジスタに設定した例外用スタックを使用して、R13_sysレジスタで
 * 示されるスタックにコンテキストを保存する。
 * USR, SYSモード以外のCPUモードからこのマクロを呼び出し、呼び出し後の
 * CPUモードはSVC, 割り込みは全て禁止とする。
 */
.macro	EIT_ENTER
	/* 一時スタックにSPSR, R0, R1, PCを保存 */
	stmfd	sp!, {r0, r1, lr}
	mrs	r0, spsr
	stmfd	sp!, {r0}
	mrs	r1, cpsr

	/* SYSモードのレジスタを保存 */
	msr	cpsr_c, #(PSR_M_SYS | PSR_I | PSR_F)
	stmfd	sp!, {r2-r12, r14}
	sub	sp, sp, #0x10		// 一時スタック保存用領域を確保
	mov	r0, sp

	/* 一時スタックの内容を移動 */
	orr	r1, r1, #(PSR_I | PSR_F)
	msr	cpsr_c, r1
	ldmfd	sp!, {r1-r4}

	msr	cpsr_c, #(PSR_M_SVC | PSR_I | PSR_F)
	stm	r0, {r1-r4}

	/* スタックポインタを初期化 */
	ldr	sp, =_svc_stack
.endm

/*
 * 例外処理用マクロ：例外処理終了（コンテキスト復帰）
 *
 * R0レジスタの示すコンテキストを復帰する。
 * マクロ呼び出し時のCPUモードは、SVCモードに限定する。
 */
.macro	EIT_LEAVE
	/* 保存されているSPSR, R0, R1, PCを一時スタックに移動 */
	ldmfd	r0!, {r1-r3, r12}
	stmfd	sp!, {r1-r3, r12}

	/* SYSモードのレジスタを復帰 */
	msr	cpsr_c, #(PSR_M_SYS | PSR_I | PSR_F)
	mov	sp, r0
	ldmfd	sp!, {r2-r12, r14}

	/* CPUモードをSVCモードに切り替える */
	msr	cpsr_c, #(PSR_M_SVC | PSR_I | PSR_F)
	ldmfd	sp!, {r0}
	msr	spsr_fsxc, r0
	ldmfd	sp!, {r0, r1, pc}^
.endm

/* 例外処理エントリ：未定義命令 */
undef_entry:
	EIT_ENTER
	mov	r4, r0
	bl	exception_undef
	mov	r0, r4
	EIT_LEAVE

/* 例外処理エントリ：SWI */
swi_entry:
	EIT_ENTER
	mov	r4, r0
	bl	exception_swi
	mov	r0, r4
	EIT_LEAVE

/* 例外処理エントリ：プリフェッチアボート */
prefetch_abort_entry:
	EIT_ENTER
	mov	r4, r0
	bl	exception_prefetch_abort
	mov	r0, r4
	EIT_LEAVE

/* 例外処理エントリ：データアボート */
data_abort_entry:
	EIT_ENTER
	mov	r4, r0
	bl	exception_data_abort
	mov	r0, r4
	EIT_LEAVE

/* 例外処理エントリ：IRQ */
irq_entry:
	sub	lr, lr, #4	// 復帰アドレス調整
	EIT_ENTER
	mov	r4, r0
	bl	exception_irq
	mov	r0, r4
	EIT_LEAVE

/* ディスパッチ */
	.global dispatch
dispatch:
	ldr	r0, [r0]
	EIT_LEAVE

	.pool
