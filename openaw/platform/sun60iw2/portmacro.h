/* SPDX-License-Identifier: MIT */
#ifndef PORTMACRO_H
#define PORTMACRO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE		uint32_t
#define portBASE_TYPE		long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;
typedef uint32_t TickType_t;

#define portMAX_DELAY		(TickType_t)0xffffffffUL
#define portTICK_TYPE_IS_ATOMIC	1

#define portSTACK_GROWTH	(-1)
#define portTICK_PERIOD_MS	((TickType_t)1000 / configTICK_RATE_HZ)
#define portBYTE_ALIGNMENT	8

extern void vPortYield(void);
extern void vPortEnterCritical(void);
extern void vPortExitCritical(void);

#define portYIELD()			vPortYield()
#define portYIELD_WITHIN_API()		vPortYield()
#define portENTER_CRITICAL()		vPortEnterCritical()
#define portEXIT_CRITICAL()		vPortExitCritical()
#define portDISABLE_INTERRUPTS()	__asm__ volatile("csrci mstatus, 0x8")
#define portENABLE_INTERRUPTS()		__asm__ volatile("csrsi mstatus, 0x8")
/* No nested interrupts are enabled; the trap has saved the interrupted task. */
extern void vTaskSwitchContext(void);
#define portYIELD_FROM_ISR(x) do { if (x) vTaskSwitchContext(); } while (0)
#define portEND_SWITCHING_ISR(x) portYIELD_FROM_ISR(x)
static inline UBaseType_t port_irq_save(void) {
 UBaseType_t previous; __asm__ volatile("csrrc %0, mstatus, %1":"=r"(previous):"r"(8u):"memory"); return previous;
}
static inline void port_irq_restore(UBaseType_t previous) {
 if (previous & 8u) __asm__ volatile("csrsi mstatus, 8":::"memory");
}
#define portSET_INTERRUPT_MASK_FROM_ISR() port_irq_save()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x) port_irq_restore(x)
#define portMEMORY_BARRIER()		__asm__ volatile("" ::: "memory")
#define portINLINE			__inline
#ifndef portFORCE_INLINE
#define portFORCE_INLINE		inline __attribute__((always_inline))
#endif
#define portPOINTER_SIZE_TYPE		uint32_t

#define portTASK_FUNCTION_PROTO(vFunction, pvParameters) \
	void vFunction(void *pvParameters)
#define portTASK_FUNCTION(vFunction, pvParameters) \
	void vFunction(void *pvParameters)

#ifdef __cplusplus
}
#endif

#endif
