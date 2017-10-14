#include "serial.h"
#include "io.h"

#define	UARTBase		0x01c28000	// UART0に固定
#define	UARTClock		24000000	// Hz
#define	UARTSpeed		115200		// bps
#define	UARTDivisor		(UARTClock / (UARTSpeed * 16))

#define	InReg(adr)		readreg32(adr)
#define	OutReg(adr, dat)	writereg32(adr, dat)

#define	URBR			(UARTBase + 0x00)	// R-
#define	UTHR			(UARTBase + 0x00)	// -W
#define	UIER			(UARTBase + 0x04)	// RW
#define	UIIR			(UARTBase + 0x08)	// R-
#define	UFCR			(UARTBase + 0x08)	// -W
#define	ULCR			(UARTBase + 0x0c)	// RW
#define	UMCR			(UARTBase + 0x10)	// RW
#define	ULSR			(UARTBase + 0x14)	// R-
#define	UMSR			(UARTBase + 0x18)	// R-

#define	UDLL			(UARTBase + 0x00)	// RW
#define	UDLM			(UARTBase + 0x04)	// RW

/* デバイス初期化 */
int serial_init(int index)
{
  OutReg(UIER, 0x00);		// 割り込み発生を禁止
  while (!(InReg(ULSR) & 0x40));// 送信データがあった場合は送信完了まで待つ
  OutReg(ULCR, 0x80);		// 分周比レジスタに切り替え
  OutReg(UDLM, 0xff);		// (分周比が0となるのを防止する)
  OutReg(UDLL, (UARTDivisor >> 0) & 0xff);
  OutReg(UDLM, (UARTDivisor >> 8) & 0xff);
  OutReg(ULCR, 0x03);		// data 8bit, stop 1bit, non-parity
  OutReg(UMCR, 0x03);		// RTS#, DTR# assert
  OutReg(UFCR, 0x07);		// FIFO使用許可/FIFOリセット

  return 0;
}

/* 送信可能か？ */
int serial_is_send_enable(int index)
{
  return InReg(ULSR) & 0x20;
}

/* １文字送信 */
int serial_send_byte(int index, unsigned char c)
{
  /* 送信可能になるまで待つ */
  while (!serial_is_send_enable(index));

  /* 送信開始 */
  OutReg(UTHR, c);

  return 0;
}

/* 受信可能か？ */
int serial_is_recv_enable(int index)
{
  return InReg(ULSR) & 0x1f;
}

/* １文字受信 */
unsigned char serial_recv_byte(int index)
{
  unsigned char s, c;

  while (1) {

    /* 受信文字が来るまで待つ */
    while (!(s = serial_is_recv_enable(index)));

    /* 受信完了 */
    c = InReg(URBR);

    /* 受信したデータにエラーが無いことをチェック */
    if (!(s & 0x1e)) break;
  }

  return c;
}

/* 送信割込み有効か？ */
int serial_intr_is_send_enable(int index)
{
  return InReg(UIER) & 0x02;
}

/* 送信割込み有効化 */
void serial_intr_send_enable(int index)
{
  OutReg(UIER, InReg(UIER) | 0x02);
}

/* 送信割込み無効化 */
void serial_intr_send_disable(int index)
{
  OutReg(UIER, InReg(UIER) & ~0x02);
}

/* 受信割込み有効か？ */
int serial_intr_is_recv_enable(int index)
{
  return InReg(UIER) & 0x01;
}

/* 受信割込み有効化 */
void serial_intr_recv_enable(int index)
{
  OutReg(UIER, InReg(UIER) | 0x01);
}

/* 受信割込み無効化 */
void serial_intr_recv_disable(int index)
{
  OutReg(UIER, InReg(UIER) & ~0x01);
}
