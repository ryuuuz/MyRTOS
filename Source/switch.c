#include "MyRTOS.h"
#include "cmsis_gcc.h"

#define NVIC_INT_CTRL 0xE000ED04
#define NVIC_PENDSVSET 0x10000000
#define NVIC_SYSPRI2 0xE000ED22
#define NVIC_PENDSV_PRI 0x000000FF

#define MEM32(addr)  *(volatile unsigned long *)(addr)
#define MEM8(addr)  *(volatile unsigned char *)(addr)

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

    tNodeInit(&task->delayNode);

    taskTable[prio] = task;
    tBitmapSet(&taskPrioBitmap, prio);
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

void tTaskSchedRdy(tTask * task) {
    taskTable[task->prio] = task;
    tBitmapSet(&taskPrioBitmap, task->prio);
}

void tTaskSchedUnRdy(tTask * task) {
    taskTable[task->prio] = (tTask *)0;
    tBitmapClear(&taskPrioBitmap, task->prio);
}

void tTaskSched(void) {
    tTask * tempTask;
    uint32_t status = tTaskEnterCritical();

    if (schedLockCount > 0) {
        tTaskExitCritical(status);
        return;
    }

    tempTask = tTaskHighestReady();
    if (tempTask != currentTask) {
        nextTask = tempTask;
        tTaskSwitch();
    }

    tTaskExitCritical(status);
}

void tTaskSystemTickHandler(void) {
    tNode * node;

    uint32_t status = tTaskEnterCritical();

    for (node = tTaskDelayedList.headNode.nextNode; node != &(tTaskDelayedList.headNode); node = node->nextNode) {
        tTask * task = tNodeParent(node, tTask, delayNode);
        if (--task->delayTicks == 0) {
            tTimeTaskWakeUp(task);
            tTaskSchedRdy(task);
        }
    }

    tTaskExitCritical(status);

    tTaskSched();
}

void tTaskDelay(uint32_t ticks) {
    uint32_t status = tTaskEnterCritical();

    tTimeTaskWait(currentTask, ticks);

    tTaskSchedUnRdy(currentTask);

    tTaskExitCritical(status);

    tTaskSched();
}

uint32_t tTaskEnterCritical(void) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

void tTaskExitCritical(uint32_t status) {
    __set_PRIMASK(status);
}

void tTaskSchedInit(void) {
    schedLockCount = 0;
    tBitmapInit(&taskPrioBitmap);
}

void tTaskSchedDisable(void) {
    uint32_t status = tTaskEnterCritical();

    if (schedLockCount < 255) {
        schedLockCount++;
    }

    tTaskExitCritical(status);
}

void tTaskSchedEnable(void) {
    uint32_t status = tTaskEnterCritical();

    if (schedLockCount > 0) {
        if (--schedLockCount == 0) {
            tTaskSched();
        }
    }

    tTaskExitCritical(status);
}

tTask * tTaskHighestReady(void) {
    uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
    return taskTable[highestPrio];
}

void tTaskDelayListInit(void) {
    tListInit(&tTaskDelayedList);
}

void tTimeTaskWait(tTask * task, uint32_t ticks) {
    task->delayTicks = ticks;
    tListAddLast(&tTaskDelayedList, &task->delayNode);
    task->state |= MYRTOS_TASK_STATE_DELAYED;
}

void tTimeTaskWakeUp(tTask * task) {
    tListRemove(&tTaskDelayedList, &task->delayNode);
    task->state &= ~MYRTOS_TASK_STATE_DELAYED;
}
