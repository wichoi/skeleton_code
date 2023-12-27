#include <unistd.h>
#include <pthread.h>

#include "apro-common.h"
#include "apro-log.h"
#include "apro-event.h"
#include "apro-test.h"

static u16 g_interval = 0;
static u16 g_count = 0;
static u16 g_curr_cnt = 0;
static pthread_t test_thread;

static void *test_proc(void)
{
    g_curr_cnt = 0;
    while(1)
    {
        g_curr_cnt++;

        // todo execute cmd
        put_event(EV_CONF_VERSION, EV_CMD_NONE, NULL, 0); // example

        log_d("%s curr_cnt[%d] count[%u] interval[%u] \n", __func__, g_curr_cnt, g_count, g_interval);

        if(g_curr_cnt == g_count)
        {
            log_d("Aging Test Finished\n");
            pthread_exit(0);
        }

        usleep(g_interval * 1000 * 1000); // (1 * 1000 * 1000); // 1 sec
    }

    return NULL;
}

int apro_test_aging_start(u16 count, u16 interval)
{
    log_d("%s count[%u] interval[%u]\n", __func__, count, interval);
    g_count = count;
    g_interval = interval;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if(pthread_create(&test_thread, &attr, (void*)test_proc, NULL) < 0)
    {
        log_e("pthread_create failed\n");
        return RET_ERROR;
    }

    return RET_SUCCESS;
}

int apro_test_aging_stop(void)
{
    log_d("%s\n", __func__);
    pthread_cancel(test_thread);
    return RET_SUCCESS;
}



