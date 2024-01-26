#ifndef __TIMER_SERVICE_H__
#define __TIMER_SERVICE_H__

typedef void(*timer_fp)(const u32);

enum
{
    TID_NONE            = 0,

    TID_HELLO           = 9001,
    TID_TIMER_TEST1     = 9002,
    TID_TIMER_TEST2     = 9003,
    TID_TIMER_TEST3     = 9004,
};

int timer_init(void);
int timer_deinit(void);
int timer_set(u32 id, u32 interval_ms, timer_fp callback);
int timer_kill(u32 id);
int timer_print(void);

#endif
