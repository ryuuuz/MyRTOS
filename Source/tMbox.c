//
// Created by ryzcq on 24-8-14.
//

#include "MyRTOS.h"

void tMboxInit(tMbox * mbox, void ** msgBuffer, uint32_t maxCount) {
    tEventInit(&mbox->event, tEventTypeMbox);

    mbox->msgBuffer = msgBuffer;
    mbox->read = 0;
    mbox->write = 0;
    mbox->count = 0;
    mbox->maxCount = maxCount;
}

uint32_t tMboxWait(tMbox * mbox, void ** msg, uint32_t timeout) {
    uint32_t status = tTaskEnterCritical();

    if (mbox->count > 0) {
        --mbox->count;
        *msg = mbox->msgBuffer[mbox->read++];
        if (mbox->read >= mbox->maxCount) {
            mbox->read = 0;
        }

        tTaskExitCritical(status);
        return tErrorNoError;
    } else {
        tEventWait(&mbox->event, currentTask, (void *)0, tEventTypeMbox, timeout);
        tTaskExitCritical(status);

        tTaskSched();

        *msg = currentTask->eventMsg;
        return currentTask->waitEventResult;
    }
}

uint32_t tMboxNoWaitGet(tMbox * mbox, void ** msg) {
    uint32_t status = tTaskEnterCritical();

    if (mbox->count > 0) {
        --mbox->count;
        *msg = mbox->msgBuffer[mbox->read++];
        if (mbox->read >= mbox->maxCount) {
            mbox->read = 0;
        }

        tTaskExitCritical(status);
        return tErrorNoError;
    } else {
        tTaskExitCritical(status);
        return tErrorResourceUnavaliable;
    }
}

uint32_t tMboxNotify(tMbox * mbox, void * msg, uint32_t notifyOption) {
    uint32_t status = tTaskEnterCritical();

    if (tEventWaitCount(&mbox->event) > 0) {
        tTask * task = tEventWakeUp(&mbox->event, msg, tErrorNoError);
        if (task->prio < currentTask->prio) {
            tTaskSched();
        }
    } else {
        if (mbox->count >= mbox->maxCount) {
            tTaskExitCritical(status);
            return tErrorResourceFull;
        }

        if (notifyOption & tMboxSendFront) {
            if (mbox->read <= 0) {
                mbox->read = mbox->maxCount - 1;
            } else {
                --mbox->read;
            }

            mbox->msgBuffer[mbox->read] = msg;
        } else {
            mbox->msgBuffer[mbox->write++] = msg;
            if (mbox->write >= mbox->maxCount) {
                mbox->write = 0;
            }
        }

        mbox->count++;
    }

    tTaskExitCritical(status);
    return tErrorNoError;
}

void tMboxFlush(tMbox * mbox) {
    uint32_t status = tTaskEnterCritical();

    mbox->read = 0;
    mbox->write = 0;
    mbox->count = 0;

    tTaskExitCritical(status);
}

uint32_t tMboxDestroy(tMbox * mbox) {
    uint32_t status = tTaskEnterCritical();
    uint32_t count = tEventRemoveAll(&mbox->event, (void *)0, tErrorDel);

    tTaskExitCritical(status);

    if (count > 0) {
        tTaskSched();
    }

    return tErrorNoError;
}

void tMboxGetInfo(tMbox * mbox, tMboxInfo * info) {
    uint32_t status = tTaskEnterCritical();

    info->count = mbox->count;
    info->maxCount = mbox->maxCount;
    info->taskCount = tEventWaitCount(&mbox->event);

    tTaskExitCritical(status);
}
