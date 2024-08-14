//
// Created by ryzcq on 24-8-14.
//

#include "MyRTOS.h"

void tFlagGroupInit(tFlagGroup * flagGroup, uint32_t flags) {
    tEventInit(&flagGroup->event, tEventTypeFlagGroup);
    flagGroup->flag = flags;
}

static uint32_t tFlagGroupCheckAndConsumed(tFlagGroup * flagGroup, uint32_t type, uint32_t * flag) {
    uint32_t srcFlag = *flag;
    uint32_t isSet = type & TFLAGGROUP_SET;
    uint32_t isAll = type & TFLAGGROUP_ALL;
    uint32_t isConsume = type & TFLAGGROUP_CONSUME;

    uint32_t calFlag = isSet ? (flagGroup->flag & srcFlag) : (~flagGroup->flag & srcFlag);

    if ((isAll && calFlag == srcFlag) || ((!isAll) && calFlag > 0)) {
        if (isConsume) {
            if (isSet) {
                flagGroup->flag &= ~srcFlag;
            } else {
                flagGroup->flag |= srcFlag;
            }
        }

        *flag = calFlag;
        return tErrorNoError;
    }

    *flag = calFlag;
    return tErrorResourceUnavaliable;
}

uint32_t tFlagGroupWait(tFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag, uint32_t timeout) {
    uint32_t result;
    uint32_t flags = requestFlag;
    uint32_t status = tTaskEnterCritical();

    result = tFlagGroupCheckAndConsumed(flagGroup, waitType, &flags);
    if (result != tErrorNoError) {
        currentTask->waitEventResult = result;
        currentTask->eventMsg = (void *)requestFlag;
        tEventWait(&flagGroup->event, currentTask, (void *)0, tEventTypeFlagGroup, timeout);
        tTaskExitCritical(status);

        tTaskSched();

        *resultFlag = currentTask->eventFlags;
        result = currentTask->waitEventResult;
    } else {
        *resultFlag = flags;
        tTaskExitCritical(status);
    }
    return result;
}

uint32_t tFlagGroupNoWaitGet(tFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag) {
    uint32_t flags = requestFlag;
    uint32_t status = tTaskEnterCritical();
    uint32_t result = tFlagGroupCheckAndConsumed(flagGroup, waitType, &flags);

    tTaskExitCritical(status);

    if (result == tErrorNoError) {
        *resultFlag = flags;
    }

    return result;
}

void tFlagGroupNotify(tFlagGroup * flagGroup, uint32_t isSet, uint32_t flag) {
    tList * waitList;
    tNode * node;
    tNode * nextNode;
    uint8_t sched = 0;
    uint32_t status = tTaskEnterCritical();

    if (isSet) {
        flagGroup->flag |= flag;
    } else {
        flagGroup->flag &= ~flag;
    }

    waitList = &flagGroup->event.waitList;
    for (node = waitList->headNode.nextNode; node != &(waitList->headNode); node = nextNode) {
        uint32_t result;
        tTask * task = tNodeParent(node, tTask, linkNode);
        uint32_t flags = task->eventFlags;
        nextNode = node->nextNode;

        result = tFlagGroupCheckAndConsumed(flagGroup, task->waitFlagsType, &flags);
        if (result == tErrorNoError) {
            task->eventFlags = flags;
            tEventWakeUpTask(&flagGroup->event, task, (void *)0, tErrorNoError);
            sched = 1;
        }
    }

    if (sched) {
        tTaskSched();
    }

    tTaskExitCritical(status);
}

void tFlagGroupGetInfo(tFlagGroup * flagGroup, tFlagGroupInfo * info) {
    uint32_t status = tTaskEnterCritical();

    info->flags = flagGroup->flag;
    info->taskCount = tEventWaitCount(&flagGroup->event);

    tTaskExitCritical(status);
}

uint32_t tFlagGroupDestroy(tFlagGroup * flagGroup) {
    uint32_t status = tTaskEnterCritical();
    uint32_t count = tEventRemoveAll(&flagGroup->event, (void *)0, tErrorDel);
    tTaskExitCritical(status);

    if (count > 0) {
        tTaskSched();
    }

    return tErrorNoError;
}
