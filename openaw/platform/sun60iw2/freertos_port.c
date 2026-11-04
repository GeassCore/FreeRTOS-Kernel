/* SPDX-License-Identifier: MIT */
#include "FreeRTOS.h"
#include "task.h"
#include "platform.h"
void xPortStartFirstTask(void);
void vPortTrapEntry(void);
UBaseType_t uxCriticalNesting = 0xaaaaaaaaUL;
static void task_returned(void) { platform_fault(0xdead0002, 0); }
StackType_t *pxPortInitialiseStack(StackType_t *top, TaskFunction_t code, void *arg)
{
 StackType_t *sp = top - 20; uint32_t gp;
 __asm__ volatile("mv %0, gp":"=r"(gp));
 for (unsigned i=0; i<20; i++) sp[i]=0;
 sp[0]=(StackType_t)task_returned; sp[1]=gp; sp[8]=(StackType_t)arg;
 sp[14]=(StackType_t)code; sp[15]=0x1880; return sp;
}
BaseType_t xPortStartScheduler(void)
{
 uxCriticalNesting=0;
 __asm__ volatile("csrw mtvec, %0"::"r"(vPortTrapEntry):"memory");
 platform_set_stage(3); platform_init_tick(configTICK_RATE_HZ);
 xPortStartFirstTask(); return pdFALSE;
}
void vPortTrapHandler(uint32_t cause, uint32_t *frame)
{
 if ((cause & 0x80000000u) && (cause & 0xffu)==20) {
  platform_ack_tick(); e902_debug->timer_irq++; e902_debug->tick++;
  if (xTaskIncrementTick()!=pdFALSE) vTaskSwitchContext();
 } else if (!(cause & 0x80000000u) && (cause & 0xfffu)==11) { frame[14]+=4; vTaskSwitchContext(); }
 else platform_fault(cause,frame[14]);
}
void vPortEndScheduler(void) { platform_fault(0xdead0003,0); }
void vPortEnterCritical(void) { __asm__ volatile("csrci mstatus, 8":::"memory"); uxCriticalNesting++; }
void vPortExitCritical(void) {
 configASSERT(uxCriticalNesting); if (--uxCriticalNesting==0) __asm__ volatile("csrsi mstatus, 8":::"memory");
}
