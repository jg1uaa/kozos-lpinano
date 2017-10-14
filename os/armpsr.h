/* ARMv4向けの定義（ARMv5以降ではフラグの種類が増えます） */

#define	PSR_N		(1 << 31)	/* negative */
#define	PSR_Z		(1 << 30)	/* zero */
#define	PSR_C		(1 << 29)	/* carry */
#define	PSR_V		(1 << 28)	/* overflow */

#define	PSR_I		(1 << 7)	/* IRQ禁止 */
#define	PSR_F		(1 << 6)	/* FIQ禁止 */
#define	PSR_T		(1 << 5)	/* Thumbモード */
#define	PSR_M_USR	(0x10 << 0)	/* プロセッサモード：user */
#define	PSR_M_FIQ	(0x11 << 0)	/* プロセッサモード：FIQ */
#define	PSR_M_IRQ	(0x12 << 0)	/* プロセッサモード：IRQ */
#define	PSR_M_SVC	(0x13 << 0)	/* プロセッサモード：supervisor */
#define	PSR_M_ABT	(0x17 << 0)	/* プロセッサモード：abort */
#define	PSR_M_UND	(0x1b << 0)	/* プロセッサモード：undefined */
#define	PSR_M_SYS	(0x1f << 0)	/* プロセッサモード：system */
