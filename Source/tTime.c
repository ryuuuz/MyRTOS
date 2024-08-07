//
// Created by ryzcq on 24-8-7.
//
#include "MyRTOS.h"

void tTaskDelay(uint32_t ticks) {
    uint32_t status = tTaskEnterCritical();

    tTimeTaskWait(currentTask, ticks);

    tTaskSchedUnRdy(currentTask);

    tTaskSched();

    tTaskExitCritical(status);
}

