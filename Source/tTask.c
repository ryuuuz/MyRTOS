//
// Created by ryzcq on 24-8-7.
//
#include "MyRTOS.h"

void tTaskInit(tTask * task, void (*entry)(void *), void * param, uint32_t prio,tTaskStack * stack) {
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
    task->prio = prio;
    task->state = MYRTOS_TASK_STATE_RDY;
    task->slice = MYRTOS_SLICE_MAX;

    tNodeInit(&task->linkNode);
    tNodeInit(&task->delayNode);

    tTaskSchedRdy(task);
}
