#include "MyRTOS.h"
#include "cmsis_gcc.h"

#define NVIC_INT_CTRL 0xE000ED04
#define NVIC_PENDSVSET 0x10000000
#define NVIC_SYSPRI2 0xE000ED22
#define NVIC_PENDSV_PRI 0x000000FF

#define MEM32(addr)  *(volatile unsigned long *)(addr)
#define MEM8(addr)  *(volatile unsigned char *)(addr)


tTask * tTaskHighestReady(void) {
    uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
    tNode * node = tListFirst(&taskTable[highestPrio]);
    return tNodeParent(node, tTask, linkNode);
}

void tTaskSchedInit(void) {
    schedLockCount = 0;
    tBitmapInit(&taskPrioBitmap);
    for (int i = 0; i < MYRTOS_PRO_COUNT; i++) {
        tListInit(&taskTable[i]);
    }
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

void tTaskSchedRdy(tTask * task) {
    tListAddFirst(&taskTable[task->prio], &task->linkNode);
    tBitmapSet(&taskPrioBitmap, task->prio);
}

void tTaskSchedUnRdy(tTask * task) {
    tListRemove(&taskTable[task->prio], &task->linkNode);
    if (tListCount(&taskTable[task->prio]) == 0) {
        tBitmapClear(&taskPrioBitmap, task->prio);
    }
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

    if (--currentTask->slice == 0) {
        if (tListCount(&taskTable[currentTask->prio]) > 0) {
            tListRemoveFirst(&taskTable[currentTask->prio]);
            tListAddLast(&taskTable[currentTask->prio], &currentTask->linkNode);
            currentTask->slice = MYRTOS_SLICE_MAX;
        }
    }

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
