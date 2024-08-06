//
// Created by ryzcq on 24-8-5.
//

#ifndef MYRTOS_H
#define MYRTOS_H

#include <stdint.h>

#include "myLib.h"
#include "tConfig.h"

#define MYRTOS_TASK_STATE_RDY       0
#define MYRTOS_TASK_STATE_DELAYED   (1 << 1)

typedef uint32_t tTaskStack;

typedef struct _tTask {
    tTaskStack * stack;
    uint32_t delayTicks;
    tNode delayNode;
    uint32_t prio;
    uint32_t state;
}tTask;

extern tTask * currentTask;
extern tTask * nextTask;
extern tTask * idleTask;

extern tBitmap taskPrioBitmap;
extern tTask * taskTable[MYRTOS_PRO_COUNT];
extern uint8_t schedLockCount;
extern tList tTaskDelayedList;

void tTaskInit(tTask * task, void (*entry)(void *), void * param, uint32_t prio,tTaskStack * stack);
void tTaskRunFirst(void);
void tTaskSwitch(void);
void tTaskSchedRdy(tTask * task);
void tTaskSchedUnRdy(tTask * task);
void tTaskSched(void);
void tTaskSystemTickHandler(void);
void tTaskDelay(uint32_t delay);

uint32_t tTaskEnterCritical(void);
void tTaskExitCritical(uint32_t status);

void tTaskSchedInit(void);
void tTaskSchedDisable(void);
void tTaskSchedEnable(void);

tTask * tTaskHighestReady(void);

void tTaskDelayListInit(void);
void tTimeTaskWait(tTask * task, uint32_t ticks);
void tTimeTaskWakeUp(tTask * task);

#endif //MYRTOS_H
