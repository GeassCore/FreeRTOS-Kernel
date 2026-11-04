/* SPDX-License-Identifier: MIT */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "platform.h"
static QueueHandle_t work_queue;
static TaskStatus_t task_status[E902_DEBUG_TASKS];
static void heartbeat_task(void *arg) {
 TickType_t last=xTaskGetTickCount();
 for (;;) { platform_set_stage(4); e902_debug->heartbeat=++e902_heartbeat_count; vTaskDelayUntil(&last,pdMS_TO_TICKS(1000)); }
}
static void producer_task(void *arg) {
 uint32_t value=0;
 for (;;) { value++; configASSERT(xQueueSend(work_queue,&value,portMAX_DELAY)==pdPASS); e902_debug->queue_tx++; vTaskDelay(pdMS_TO_TICKS(20)); }
}
static void consumer_task(void *arg) {
 uint32_t value,expected=1;
 for (;;) { configASSERT(xQueueReceive(work_queue,&value,portMAX_DELAY)==pdPASS); configASSERT(value==expected++); e902_debug->queue_rx++; }
}
/* This task deliberately never yields: advancing other counters proves preemption. */
static void busy_task(void *arg) { for (;;) e902_debug->busy_counter++; }
static void monitor_task(void *arg) {
 for (;;) {
  UBaseType_t n=uxTaskGetSystemState(task_status,E902_DEBUG_TASKS,NULL);
  e902_debug->sequence++; e902_debug->task_count=n;
  for (UBaseType_t i=0;i<n;i++) {
   volatile struct e902_task_info *d=&e902_debug->tasks[i]; TaskStatus_t *t=&task_status[i];
   unsigned j=0; for (;j<15 && t->pcTaskName[j];j++) d->name[j]=t->pcTaskName[j];
   for (;j<16;j++) d->name[j]=0;
   d->state=t->eCurrentState; d->priority=t->uxCurrentPriority;
   d->stack_words=t->usStackHighWaterMark; d->number=t->xTaskNumber;
  }
  __asm__ volatile("fence rw,rw":::"memory"); e902_debug->sequence++;
  vTaskDelay(pdMS_TO_TICKS(1000));
 }
}
static void mailbox_task(void *arg) { platform_mailbox_init(); for (;;) { while (platform_mailbox_echo()) ; vTaskDelay(pdMS_TO_TICKS(10)); } }
static void timer_callback(TimerHandle_t timer) { e902_debug->software_timer++; }
int main(void) {
 platform_set_stage(2); work_queue=xQueueCreate(8,sizeof(uint32_t)); configASSERT(work_queue);
 configASSERT(xTaskCreate(heartbeat_task,"heartbeat",256,NULL,3,NULL)==pdPASS);
 configASSERT(xTaskCreate(producer_task,"producer",256,NULL,2,NULL)==pdPASS);
 configASSERT(xTaskCreate(consumer_task,"consumer",256,NULL,3,NULL)==pdPASS);
 configASSERT(xTaskCreate(busy_task,"cpu-load",256,NULL,1,NULL)==pdPASS);
 configASSERT(xTaskCreate(monitor_task,"monitor",512,NULL,2,NULL)==pdPASS);
 configASSERT(xTaskCreate(mailbox_task,"mailbox",256,NULL,2,NULL)==pdPASS);
 TimerHandle_t timer=xTimerCreate("quarter-second",pdMS_TO_TICKS(250),pdTRUE,NULL,timer_callback);
 configASSERT(timer); configASSERT(xTimerStart(timer,0)==pdPASS);
 vTaskStartScheduler(); platform_fault(0xdead0004,0);
}
