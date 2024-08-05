#include "MyRTOS.h"

#define NVIC_INT_CTRL 0xE000ED04
#define NVIC_PENDSVSET 0x10000000
#define NVIC_SYSPRI2 0xE000ED22
#define NVIC_PENDSV_PRI 0x000000FF

#define MEM32(addr)  *(volatile unsigned long *)(addr)
#define MEM8(addr)  *(volatile unsigned char *)(addr)

void tTaskInit(tTask * task, void (*entry)(void *), void * param, tTaskStack * stack) {
    *(--stack) = (unsigned long)(1 << 24);
    *(--stack) = (unsigned long)entry;
    *(--stack) = (unsigned long)0x14;
    *(--stack) = (unsigned long)0x12;
    *(--stack) = (unsigned long)0x3;
    *(--stack) = (unsigned long)0x2;
    *(--stack) = (unsigned long)0x1;
    *(--stack) = (unsigned long)param;

    *(--stack) = (unsigned long)0x11;
    *(--stack) = (unsigned long)0x10;
    *(--stack) = (unsigned long)0x9;
    *(--stack) = (unsigned long)0x8;
    *(--stack) = (unsigned long)0x7;
    *(--stack) = (unsigned long)0x6;
    *(--stack) = (unsigned long)0x5;
    *(--stack) = (unsigned long)0x4;

    task->stack = stack;
    task->delayTicks = 0;
}

void tTaskRunFirst(void)
{
    __set_PSP(0);

    MEM32(NVIC_SYSPRI2) |= NVIC_PENDSV_PRI;
    MEM32(NVIC_INT_CTRL) |= NVIC_PENDSVSET;
}

void tTaskSwitch(void)
{
    MEM32(NVIC_INT_CTRL) |= NVIC_PENDSVSET;
}

void tTaskSched(void) {
    if (currentTask == idleTask) {
        if (taskTable[0]->delayTicks == 0) {
            nextTask = taskTable[0];
        } else if (taskTable[1]->delayTicks == 0) {
            nextTask = taskTable[1];
        } else {
            return;
        }
    } else {
        if (currentTask == taskTable[0]) {
            if (taskTable[1]->delayTicks == 0) {
                nextTask = taskTable[1];
            } else if (currentTask->delayTicks != 0) {
                nextTask = idleTask;
            } else {
                return;
            }
        } else if (currentTask == taskTable[1]) {
            if (taskTable[0]->delayTicks == 0) {
                nextTask = taskTable[0];
            } else if (currentTask->delayTicks != 0) {
                nextTask = idleTask;
            } else {
                return;
            }
        }
    }

    tTaskSwitch();
}

void tTaskSystemTickHandler(void) {
    int i;
    for(i = 0; i < 2; i++) {
        if(taskTable[i]->delayTicks > 0) {
            taskTable[i]->delayTicks--;
        }
    }

    tTaskSched();
}

void tTaskDelay(uint32_t ticks) {
    currentTask->delayTicks = ticks;
    tTaskSched();
}