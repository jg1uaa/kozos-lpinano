#include "defines.h"
#include "interrupt.h"
#include "intr.h"
#include "io.h"

/* 割り込みコントローラ(Generic Interrupt Controller)の定義 */
#define GIC_DIST		0x01c81000
#define GIC_CPUIF		0x01c82000

#define GICD_CTLR		(GIC_DIST + 0x0000)
#define GICD_ISENABLER(x)	(GIC_DIST + 0x0100 + 4 * (x))
#define GICD_ICENABLER(x)	(GIC_DIST + 0x0180 + 4 * (x))
#define GICD_ISACTIVER(x)	(GIC_DIST + 0x0300 + 4 * (x))
#define GICD_ICACTIVER(x)	(GIC_DIST + 0x0380 + 4 * (x))
#define GICD_IPRIORITYR(x)	(GIC_DIST + 0x0400 + 4 * (x))
#define GICD_ITARGETSR(x)	(GIC_DIST + 0x0800 + 4 * (x))
#define GICD_ICFGR(x)		(GIC_DIST + 0x0c00 + 4 * (x))

#define GICC_CTLR		(GIC_CPUIF + 0x0000)
#define GICC_PMR		(GIC_CPUIF + 0x0004)
#define GICC_IAR		(GIC_CPUIF + 0x000c)
#define GICC_EOIR		(GIC_CPUIF + 0x0010)

/* 割り込みコントローラの初期化 */
void intc_init(void)
{
  int i;

  /* 全ての割り込みを禁止 */
  for (i = 0; i < 4; i++) writereg32(GICD_ICENABLER(i), ~0);

  /* 割り込み#32〜#159をCPU#0へ */
  for (i = 8; i < 40; i++) writereg32(GICD_ITARGETSR(i), 0x01010101);

  /* 割り込みを許可する優先順位: すべて */
  writereg32(GICC_PMR, 0xff);

  /* CPU interface/Distributor両方の割り込み処理を開始 */
  writereg32(GICC_CTLR, 1);
  writereg32(GICD_CTLR, 1);
}

/* 割り込み要因の許可 */
void intc_enable(softvec_type_t type)
{
  writereg32(GICD_ISENABLER(type / 32), 1 << (type % 32));
}

/* 割り込み要因の禁止 */
void intc_disable(softvec_type_t type)
{
  writereg32(GICD_ICENABLER(type / 32), 1 << (type % 32));
}

/* 割り込み処理の終了通知 */
void intc_sendeoi(softvec_type_t type)
{
  writereg32(GICC_EOIR, type);
}

/* 割り込みベクタ番号の取得 */
softvec_type_t intc_vector(void)
{
  return readreg32(GICC_IAR);
}
