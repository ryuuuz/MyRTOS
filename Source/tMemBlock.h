//
// Created by ryzcq on 24-8-14.
//

#ifndef TMEMBLOCK_H
#define TMEMBLOCK_H

#include "tEvent.h"

#define tMemBlockErrorNoError 0
#define tMemBlockErrorTimeout 1
#define tMemBlockErrorResourceUnavaliable 2
#define tMemBlockErrorNoMemBlock 3

typedef struct _tMemBlock {
    tEvent event;
    void * memStart;
    uint32_t blockSize;
    uint32_t maxCount;
    tList blockList;
}tMemBlock;

typedef struct _tMemBlockInfo {
    uint32_t count;
    uint32_t maxCount;
    uint32_t blockSize;
    uint32_t taskCount;
}tMemBlockInfo;

void tMemBlockInit(tMemBlock * memBlock, uint8_t * memStart, uint32_t blockSize, uint32_t blockTotal);
uint32_t tMemBlockWait(tMemBlock * memBlock, uint8_t ** mem, uint32_t timeout);
uint32_t tMemBlockNoWaitGet(tMemBlock * memBlock, void ** mem);
uint32_t tMemBlockNotify(tMemBlock * memBlock, uint8_t * mem);
void tMemBlockGetInfo(tMemBlock * memBlock, tMemBlockInfo * info);
uint32_t tMemBlockDestroy(tMemBlock * memBlock);

#endif //TMEMBLOCK_H
