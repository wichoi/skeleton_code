#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

static timer_t timer_1;
static timer_t timer_2;

static void apro_timer_cb( int sig, siginfo_t *si, void *uc )
{
    timer_t *tidp;
    tidp = (void**)(si->si_value.sival_ptr);

    switch(*tidp)
    {
    case timer_1:
        log_i("%s timer_1\n", __func__);
        break;

    case timer_2:
        log_i("%s timer_2\n", __func__);
        break;

    default:
        log_i("%s unknown\n", __func__);
        break;
    }
}

static int apro_timer_create(timer_t *timer_id, int sec, int msec)
{
    struct sigevent         te;
    struct itimerspec       its;
    struct sigaction        sa;
    int                     sigNo = SIGRTMIN;

    /* Set up signal handler. */
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = apro_timer_cb;
    sigemptyset(&sa.sa_mask);

    if(sigaction(sigNo, &sa, NULL) == -1)
    {
        log_e("sigaction error\n");
        return -1;
    }

    /* Set and enable alarm */
    te.sigev_notify = SIGEV_SIGNAL;
    te.sigev_signo = sigNo;
    te.sigev_value.sival_ptr = timer_id;
    timer_create(CLOCK_REALTIME, &te, timer_id);

    its.it_interval.tv_sec = sec;
    its.it_interval.tv_nsec = msec * 1000000;
    its.it_value.tv_sec = sec;
    its.it_value.tv_nsec = msec * 1000000;
    timer_settime(*timer_id, 0, &its, NULL);

    return 0;
}

void apro_timer_create_1(void)
{
    int ret_val = apro_timer_create(&timer_1, 1, 0); // 1000ms
    log_i("%s result[%d] \n", ret_val);
}

void apro_timer_delete_1(void)
{
    int ret_val = timer_delete(timer_1); // 0:success , -1:fail
    log_i("%s result[%d] \n", ret_val);
}

void apro_timer_create_2(void)
{
    int ret_val = apro_timer_create(&timer_2, 0, 100); // 100ms
    log_i("%s result[%d] \n", ret_val);
}

void apro_timer_delete_2(void)
{
    int ret_val = timer_delete(timer_2); // 0:success , -1:fail
    log_i("%s result[%d] \n", ret_val);
}

