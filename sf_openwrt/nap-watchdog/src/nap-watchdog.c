#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ipc.h"

#define STR_SIGFOX_ACMD     "sigfox_acmd"
#define STR_NAP_CLIENT      "nap-client"
#define STR_ATCMD           "atcmd"

#define RETRY_CNT           1 // 3

// sigfox_acmd exist
//root@OpenWrt:/data# ps | grep sigfox_acmd
// 2587 root      1120 S    sigfox_acmd
// 2844 root      1564 S    grep sigfox_acmd

// sigfox_acmd not exist
//root@OpenWrt:/data# ps |grep sigfox_acmd
// 2854 root      1564 S    grep sigfox_acmd

static void process_check(char *p_cmd, int *cnt)
{
    char str_grep[128] = {0,};
    char str_kill[128] = {0,};
    char str_exec[128] = {0,};
    sprintf(str_grep, "ps | grep %s", p_cmd);   // "ps | grep sigfox_acmd"
    sprintf(str_kill, "killall -9 %s", p_cmd);  // "killall -9 sigfox_acmd"

    if(strcmp(p_cmd, STR_ATCMD) == 0)
    {
        sprintf(str_exec, "%s -d &", p_cmd);           // "atcmd -d &"
    }
    else
    {
        sprintf(str_exec, "%s &", p_cmd);           // "sigfox_acmd &"
    }

    FILE* fp = NULL;
    char *line = NULL;
    size_t len = 0;
    int ps_flag = 0;

    fp = popen(str_grep, "r");
    if(fp != NULL)
    {
        while(getline(&line, &len, fp) != -1)
        {
            char *cmd = strstr(line, p_cmd);
            char *grep = strstr(line, "grep");
            if(cmd != NULL)
            {
                if(grep == NULL)
                {
                    //fprintf(stderr, "%s exist\n", p_cmd);
                    ps_flag = 1;
                }
            }
        }
        free(line);

        if(ps_flag == 1)
        {
            *cnt = 0;
        }
        else
        {
            (*cnt)++;
            fprintf(stderr, "%s check[%d] !!!\n", p_cmd, *cnt);
            if(*cnt >= RETRY_CNT)
            {
                *cnt = 0;
                fprintf(stderr, "%s &\n", p_cmd);
                system(str_kill);
                system(str_exec);
            }
        }

        pclose(fp);
    }
}

int main(int argc, char *const argv[])
{
    fprintf(stderr, "NAP SW Watchdog Daemon Start !!!\n");
    int acmd_cnt = RETRY_CNT;
    int nap_cnt = RETRY_CNT;
    int atcmd_cnt = RETRY_CNT;
    int uptime = 0;
    int watch_cnt = 0;
    int nap_model = 0;

    // 3(lte not support), 5(wifi not support), 7(all support)
    FILE *fd = popen("nvram get 2860 NAP_Model", "r");
    if(fd != NULL)
    {
        fscanf(fd, "%d", &nap_model);
        pclose(fd);
    }

    ipc_init();

    while(uptime < 60)
    {
        FILE *fp = fopen("/proc/uptime", "r");
        if(fp != NULL)
        {
            fscanf(fp, "%u", &uptime);
            fclose(fp);
        }
        sleep(1);
    }

    while(1)
    {
        if(++watch_cnt > 10)
        {
            process_check(STR_SIGFOX_ACMD, &acmd_cnt);
            process_check(STR_NAP_CLIENT, &nap_cnt);

            if(nap_model != 3)
            {
                process_check(STR_ATCMD, &atcmd_cnt);
            }

            watch_cnt = 0;
        }

        ipc_proc();
        sleep(1);
    }

    ipc_deinit();
    fprintf(stderr, "NAP SW Watchdog Daemon End !!!\n");
    return 0;
}


