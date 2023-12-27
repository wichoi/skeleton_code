#ifndef _TIMER_H_
#define _TIMER_H_

typedef void timerCb(int signum);

int set_timer(u64 tv_sec, u64 tv_usec, timerCb* cb);

//int set_timer(timer_t *timerID, int expireMS, int intervalMS)

#endif
