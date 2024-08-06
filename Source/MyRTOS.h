//
// Created by ryzcq on 24-8-5.
//

#ifndef MYRTOS_H
#define MYRTOS_H

#include <stdint.h>
#include "cmsis_gcc.h"

#include "myLib.h"

typedef uint32_t tTaskStack;

typedef struct _tTask {
    tTaskStack * stack;
    uint32_t delayTicks;
}tTask;

extern tTask * currentTask;
extern tTask * nextTask;
extern tTask * idleTask;
extern tTask * taskTable[2];
extern uint8_t schedLockCount;
extern uint32_t tickCounter;

void tTaskInit(tTask * task, void (*entry)(void *), void * param, tTaskStack * stack);
void tTaskRunFirst(void);
void tTaskSwitch(void);
void tTaskSched(void);
void tTaskSystemTickHandler(void);
void tTaskDelay(uint32_t delay);

uint32_t tTaskEnterCritical(void);
void tTaskExitCritical(uint32_t status);

void tTaskSchedInit(void);
void tTaskSchedDisable(void);
void tTaskSchedEnable(void);

#endif //MYRTOS_H
