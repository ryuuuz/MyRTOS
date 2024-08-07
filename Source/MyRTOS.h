//
// Created by ryzcq on 24-8-5.
//

#ifndef MYRTOS_H
#define MYRTOS_H

#include <stdint.h>

#include "tLib.h"
#include "tConfig.h"
#include "tTask.h"
#include "tEvent.h"

typedef enum _tError {
    tErrorNoError = 0,
    tErrorTimeout,
}tError;

extern tBitmap taskPrioBitmap;
extern tList taskTable[MYRTOS_PRO_COUNT];
extern uint8_t schedLockCount;
extern tList tTaskDelayedList;

extern tTask * currentTask;
extern tTask * nextTask;

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



void tSetSysTickPeriod(uint32_t ms);
void tInitApp(void);

#endif //MYRTOS_H
