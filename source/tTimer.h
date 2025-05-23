#ifndef TTIMER_H
#define TTIMER_H

#include "tEvent.h"

#define TIMER_CONFIG_TYPE_HARD  (1 << 0)
#define TIMER_CONFIG_TYPE_SOFT  (0 << 0)

typedef enum _tTimreState
{
    tTimerCreated,
    tTimerStarted,
    tTimerRunning,
    tTimerStoped,
    tTimerDestroyed,
}tTimerState;

typedef struct _tTimer
{
    tNode linkNode;
    uint32_t startDelayTicks;
    uint32_t durationTicks;
    uint32_t delayTicks;
    void (* timerFunc) (void * arg);
    void * arg;
    uint32_t config;

    tTimerState state;
}tTimer;

typedef struct _tTimerInfo
{
    uint32_t startDelayTicks;
    uint32_t durationTicks;
    void (* timerFunc) (void * arg);
    void * arg;
    uint32_t config;

    tTimerState state;

}tTimerInfo;

void tTimerInit(tTimer * timer, uint32_t delayTicks, uint32_t durationTicks,
        void (* timerFunc) (void * arg), void * arg, uint32_t config);
void tTimerModuleInit(void);
void tTimerInitTask(void);
void tTimerStart (tTimer * timer);
void tTimerStop (tTimer * timer);
static void tTimerCallFuncList(tList * timerList);
static void tTimerSoftTask(void* param);
void tTimerModuleTickNotify(void);

void tTimerDestroy (tTimer * timer);
void tTimerGetInfo(tTimer * timer, tTimerInfo * info);
#endif