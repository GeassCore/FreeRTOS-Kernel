/* RV32E non-nested CLIC trap frame: 14 registers, mepc, mstatus, critical nesting, padding. */
.section .text
.globl vPortYield
.type vPortYield,@function
vPortYield:
 ecall
 ret
.balign 64
.globl vPortTrapEntry
.type vPortTrapEntry,@function
vPortTrapEntry:
 addi sp,sp,-80
 sw ra,0(sp)
 sw gp,4(sp)
 sw tp,8(sp)
 sw t0,12(sp)
 sw t1,16(sp)
 sw t2,20(sp)
 sw s0,24(sp)
 sw s1,28(sp)
 sw a0,32(sp)
 sw a1,36(sp)
 sw a2,40(sp)
 sw a3,44(sp)
 sw a4,48(sp)
 sw a5,52(sp)
 csrr t0,mepc
 sw t0,56(sp)
 csrr t0,mstatus
 sw t0,60(sp)
 la t0,uxCriticalNesting
 lw t1,0(t0)
 sw t1,64(sp)
 la t0,pxCurrentTCB
 lw t1,0(t0)
 sw sp,0(t1)
 csrr a0,mcause
 mv a1,sp
 call vPortTrapHandler
 j .Lrestore
.globl xPortStartFirstTask
.type xPortStartFirstTask,@function
xPortStartFirstTask:
.Lrestore:
 la t0,pxCurrentTCB
 lw t1,0(t0)
 lw sp,0(t1)
 lw t1,64(sp)
 la t0,uxCriticalNesting
 sw t1,0(t0)
 lw t0,56(sp)
 csrw mepc,t0
 lw t0,60(sp)
 csrw mstatus,t0
 lw ra,0(sp)
 lw gp,4(sp)
 lw tp,8(sp)
 lw t0,12(sp)
 lw t1,16(sp)
 lw t2,20(sp)
 lw s0,24(sp)
 lw s1,28(sp)
 lw a0,32(sp)
 lw a1,36(sp)
 lw a2,40(sp)
 lw a3,44(sp)
 lw a4,48(sp)
 lw a5,52(sp)
 addi sp,sp,80
 mret
