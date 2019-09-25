#ifndef _TYPE_COMMON_H
#define _TYPE_COMMON_H

typedef	void			OTP_VOID;
typedef char     		OTP_CHAR;	/* 不保证是否有符号，
										实际上，在PPC和ARM环境下char为无符号，
												在仿真环境下char为有符号 */
typedef signed char     OTP_INT8;	/* 有符号8位 */
typedef unsigned char   OTP_UINT8;	/* 无符号8位 */
typedef signed short    OTP_INT16;	/* 有符号16位 */
typedef unsigned short  OTP_UINT16;	/* 无符号16位 */
typedef signed int		OTP_INT32;	/* 有符号32位 */
typedef unsigned int	OTP_UINT32;	/* 无符号32位 */

/*与定义成了long的移植代码对接，显式说明长度为32位*/
typedef signed long     OTP_LONG32;	/* 有符号32位 */
typedef unsigned long   OTP_ULONG32;	/* 无符号32位 */
typedef	signed long long 	OTP_INT64;	/* 有符号64位 */
typedef	unsigned long long OTP_UINT64;	/* 无符号64位 */

/* 8位寄存器类型 */
typedef volatile OTP_UINT8 OTP_REG8;

/* 指令 */
typedef unsigned int	OTP_INSTRUCT;


/* 局部变量或局部函数修饰符 */
#ifndef LOCAL
#define LOCAL			static
#endif

#ifndef IMPORT
#define IMPORT			extern
#endif


/* OK ERROR 与 STATUS 一起使用*/
#ifndef STATUS
#define STATUS int /*与VXWORKS保持一致*/
#endif
#ifndef OK
#define OK				0
#endif
#ifndef ERROR
#define ERROR			(-1)
#endif

/* TRUE FALSE 与 BOOL 一起使用*/
#ifndef BOOL
#define BOOL int	/*与VXWORKS保持一致*/
#endif
#ifndef FALSE
#define FALSE			0
#endif
#ifndef TRUE
#define TRUE			1
#endif

#ifndef ENABLE
#define ENABLE			1
#endif
#ifndef DISABLE
#define DISABLE			0
#endif

#ifndef NULL
#define NULL 0L
#endif

/*比特位定义*/
#define   BIT(n)  (1 << (n))

#define MCU_EXC_ERROR 

#endif /* _TYPE_COMMON_H */

