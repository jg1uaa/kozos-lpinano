#include "defines.h"
#include "intr.h"
#include "interrupt.h"
#include "kozos.h"
#include "lib.h"
#include "armpsr.h"
#include "monitor.h"

/*
 * 動作停止 (モニタの起動)
 */
static void die(unsigned long sp)
{
  /* 例外発生時のSPの値を表示 */
  puts(", sp=");
  putxval(sp, 8);
  putc('\n');

  /* モニタを起動する */
  while (1) monitor_main();
}

/* 
 * 例外ハンドラ (未定義命令)
 * 例外を発生したアドレスを表示し、動作を停止する
 */
void exception_undef(unsigned long sp)
{
  unsigned long pc, spsr;

  /* 保存されたコンテキストから、PCおよびSPSRを得る */
  spsr = *(unsigned long *)(sp + 0);
  pc = *(unsigned long *)(sp + 12);

  /* ARM/Thumbモードに応じ、例外を発生させたPCの値を表示 */
  puts("Undefined instruction at ");
  putxval((spsr & PSR_T) ? (pc - 2) : (pc - 4), 8);

  /* 動作を停止する */
  die(sp);
}

/* 
 * 例外ハンドラ (SWI)
 * 呼び出し元のSWI命令から割り込みベクタを取得する
 */
void exception_swi(unsigned long sp)
{
  softvec_type_t type;
  unsigned long pc, spsr;

  /* 保存されたコンテキストから、PCおよびSPSRを得る */
  spsr = *(unsigned long *)(sp + 0);
  pc = *(unsigned long *)(sp + 12);

  /* ARM/Thumbモードに応じ、呼び出し元のSWI命令を参照する */
  if (spsr & PSR_T) {
	  type = *(unsigned short *)(pc - 2);
  } else {
	  type = *(unsigned long *)(pc - 4);
  }

  /* 有効なベクタ番号は、0〜255の範囲とする */
  type &= 0xff;

  thread_intr(type, sp);
}

/* 
 * 例外ハンドラ (プリフェッチアボート)
 * 例外を発生したアドレスを表示し、動作を停止する
 */
void exception_prefetch_abort(unsigned long sp)
{
  unsigned long pc;

  /* 保存されたコンテキストから、PCを得る */
  pc = *(unsigned long *)(sp + 12);

  /* 例外を発生させたPCの値を表示 */
  puts("Prefetch Abort at ");
  putxval(pc - 4, 8);

  /* 動作を停止する */
  die(sp);
}

/* 
 * 例外ハンドラ (データアボート)
 * 例外を発生したアドレスを表示し、動作を停止する
 */
void exception_data_abort(unsigned long sp)
{
  unsigned long pc;

  /* 保存されたコンテキストから、PCを得る */
  pc = *(unsigned long *)(sp + 12);

  /* 例外を発生させたPCの値を表示 */
  puts("Data Abort at ");
  putxval(pc - 8, 8);

  /* 動作を停止する */
  die(sp);
}

/* 
 * 例外ハンドラ (IRQ)
 * 割り込みコントローラから割り込みベクタを取得する
 */
void exception_irq(unsigned long sp)
{
  softvec_type_t type = intc_vector();

  thread_intr(type, sp);
}
