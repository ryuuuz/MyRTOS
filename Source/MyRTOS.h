//
// Created by ryzcq on 24-8-5.
//

#ifndef MYRTOS_H
#define MYRTOS_H

#include <stdint.h>

#include "myLib.h"
#include "tConfig.h"

#define MYRTOS_TASK_STATE_RDY           0
#define MYRTOS_TASK_STATE_DESTORYED     (1 << 1)
#define MYRTOS_TASK_STATE_DELAYED       (1 << 2)
#define MYRTOS_TASK_STATE_SUSPEND       (1 << 3)

typedef uint32_t tTaskStack;

typedef struct _tTask {
    tTaskStack * stack;
    tNode linkNode;
    uint32_t delayTicks;
    tNode delayNode;
    uint32_t prio;
    uint32_t state;
    uint32_t slice;
    uint32_t suspendCount;

    void (*clean)(void * param);
    void * cleanParam;
    uint8_t requestDeleteFlag;
}tTask;

extern tTask * currentTask;
extern tTask * nextTask;

extern tBitmap taskPrioBitmap;
extern tList taskTable[MYRTOS_PRO_COUNT];
extern uint8_t schedLockCount;
extern tList tTaskDelayedList;

uint32_t tTaskEnterCritical(void);
void tTaskExitCritical(uint32_t status);
void tTaskRunFirst(void);
void tTaskSwitch(void);

tTask * tTaskHighestReady(void);
void tTaskSchedInit(void);
void tTaskSchedDisable(void);
void tTaskSchedEnable(void);
void tTaskSchedRdy(tTask * task);
void tTaskSchedUnRdy(tTask * task);
void tTaskSchedRemove(tTask * task);
void tTaskSched(void);

void tTimeTaskWait(tTask * task, uint32_t ticks);
void tTimeTaskWakeUp(tTask * task);
void tTimeTaskRemove(tTask * task);

void tTaskDelayListInit(void);

void tTaskSystemTickHandler(void);
void tTaskDelay(uint32_t delay);

typedef struct _tTaskInfo {
    uint32_t delayTicks;
    uint32_t prio;
    uint32_t state;
    uint32_t slice;
    uint32_t suspendCount;
}tTaskInfo;

void tTaskInit(tTask * task, void (*entry)(void *), void * param, uint32_t prio,tTaskStack * stack);
void tTaskSuspend(tTask * task);
void tTaskWakeUp(tTask * task);
void tTaskSetCleanCallFunc(tTask * task, void (*clean)(void * param), void * param);
void tTaskForceDelete(tTask * task);
void tTaskRequestDelete(tTask * task);
uint8_t tTaskIsRequestedDelete(void);
void tTaskDeleteSelf(void);
void tTaskGetInfo(tTask * task, tTaskInfo * info);

void tSetSysTickPeriod(uint32_t ms);
void tInitApp(void);

#endif //MYRTOS_H
