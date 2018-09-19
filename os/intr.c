#include "defines.h"
#include "interrupt.h"
#include "intr.h"
#include "io.h"

/* 割り込みコントローラ(Generic Interrupt Controller)の定義 */
#define INTC			0x01c20400

#define INTC_VECTOR_REG		(INTC + 0x00)
#define INTC_BASE_ADDR_REG	(INTC + 0x04)
#define NMI_INT_CTRL_REG	(INTC + 0x0c)
#define INTC_PEND_REG(n)	(INTC + 0x10 + ((n) * 4))
#define INTC_EN_REG(n)		(INTC + 0x20 + ((n) * 4))
#define INTC_MASK_REG(n)	(INTC + 0x30 + ((n) * 4))
#define INTC_RESP_REG(n)	(INTC + 0x40 + ((n) * 4))
#define INTC_FF_REG(n)		(INTC + 0x50 + ((n) * 4))
#define INTC_PRIO_REG(n)	(INTC + 0x60 + ((n) * 4))

/* 割り込みコントローラの初期化 */
void intc_init(void)
{
  /* 全ての割り込みを禁止 */
  writereg32(INTC_EN_REG(0), 0);
  writereg32(INTC_EN_REG(1), 0);
  writereg32(INTC_FF_REG(0), 0);
  writereg32(INTC_FF_REG(1), 0);
  
  /* ベクタアドレスの初期化 */
  writereg32(INTC_BASE_ADDR_REG, 0);

  /* 割り込み優先順位はデフォルト */
  writereg32(INTC_PRIO_REG(0), 0);
  writereg32(INTC_PRIO_REG(1), 0);
  writereg32(INTC_PRIO_REG(2), 0);
  writereg32(INTC_PRIO_REG(3), 0);

  /* NMI source type (default: low level) */
  writereg32(NMI_INT_CTRL_REG, 0);

  /* ペンディングされた割り込みをクリア */
  writereg32(INTC_PEND_REG(0), ~0);
  writereg32(INTC_PEND_REG(1), ~0);

  /* 割り込みマスクは解除 */
  writereg32(INTC_MASK_REG(0), 0);
  writereg32(INTC_MASK_REG(1), 0);
  writereg32(INTC_RESP_REG(0), 0);
  writereg32(INTC_RESP_REG(1), 0);
}

/* 割り込み要因の許可 */
void intc_enable(softvec_type_t type)
{
  int reg = INTC_EN_REG(type / 32);

  writereg32(reg, readreg32(reg) | (1 << (type % 32)));
}

/* 割り込み要因の禁止 */
void intc_disable(softvec_type_t type)
{
  int reg = INTC_EN_REG(type / 32);

  writereg32(reg, readreg32(reg) & ~(1 << (type % 32)));
}

/* 割り込み処理の終了通知 */
void intc_sendeoi(softvec_type_t type)
{
  if (type == 0) writereg32(INTC_PEND_REG(0), 1 << 0);
}

/* 割り込みベクタ番号の取得 */
softvec_type_t intc_vector(void)
{
  return readreg32(INTC_VECTOR_REG) >> 2;
}
