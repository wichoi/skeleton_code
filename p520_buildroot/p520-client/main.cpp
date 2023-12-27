#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <curl/curl.h>

#include "common.h"
#include "log.h"
#include "main-client.h"

typedef struct _log_level
{
    char level;
    const char *str;
} log_table_t;

const static log_table_t log_tbl[] =
{
    { PRINT_NONE,       "-N" },
    { PRINT_VERBOSE,    "-V" },
    { PRINT_DEBUG,      "-D" },
    { PRINT_INFO,       "-I" },
    { PRINT_WARN,       "-W" },
    { PRINT_ERROR,      "-E" },
    { PRINT_FATAL,      "-F" },
};

#if 0
static void sig_cb(int sig)
{
    printf("sig_cb %d \n", sig);
    exit(0);
}
#endif

static int read_nvram_client_enable(void)
{
    int ret_val = RET_OK;
    char *cmd = "nvram_get 2860 AproCloudEnable";
    FILE *fp = popen(cmd, "r");
    if(fp != NULL)
    {
        while(!feof(fp))
        {
            char buf[128] = {0,};
            int len = fread(buf, 1, sizeof(buf) - 1, fp);
            if(!ferror(fp))
            {
                if(strncmp(buf, "0", strlen("0")) == 0)
                {
                    ret_val = RET_ERROR;
                    break;
                }
            }
        }
        pclose(fp);
    }

    return ret_val;
}

static void read_nvram_loglevel(char *log_level)
{
    char *cmd = "nvram_get 2860 AproCloudLog";
    FILE *fp = popen(cmd, "r");
    if(fp != NULL)
    {
        while(!feof(fp))
        {
            char level[128] = {0,};
            int len = fread(level, 1, sizeof(level) - 1, fp);
            if(!ferror(fp))
            {
                int i = 0;
                for(i = 0; i < sizeof(log_tbl)/sizeof(log_tbl[0]); i++)
                {
                    if(strncmp(log_tbl[i].str, level, strlen(log_tbl[i].str)) == 0)
                    {
                        *log_level = log_tbl[i].level;
                        break;
                    }
                }
            }
        }
        pclose(fp);
    }
}

static char arg_parser(int argc, char **argv, char *log_level)
{
    int ret_val = RET_ERROR;
    if(argc < 2)
    {
        read_nvram_loglevel(log_level);
        return ret_val;
    }

    char *level = (char*)argv[1];
    int i = 0;
    for(i = 0; i < sizeof(log_tbl)/sizeof(log_tbl[0]); i++)
    {
        if(strcmp(log_tbl[i].str, level) == 0)
        {
            *log_level = log_tbl[i].level;
            ret_val = RET_OK;
            break;
        }
    }

    return ret_val;
}

int main(int argc, char** argv)
{
    int ret_val = RET_OK;
    char log_level = PRINT_INFO;
    main_client client;

    if(read_nvram_client_enable() != RET_OK)
    {
        printf("p520-client disabled. AproCloudEnable nvitem is '0' !!!\n");
        return ret_val;
    }

    // Ignore signal
    signal(SIGTTIN, SIG_IGN); // [1]+ Stopped (tty input)
    //signal(SIGBUS, sig_cb); // curl Bus error
    //signal(SIGILL, sig_cb); // curl Illegal instruction
    //signal(SIGSEGV, sig_cb); // curl Segmentation fault

    curl_global_init(CURL_GLOBAL_ALL);
    arg_parser(argc, argv, &log_level);
    debug_init(LOG_ENABLE, log_level, "/tmp/p520-client.log");
    log_print(log_level, "p520 client start !!! \n");

    while(1)
    {
        if(client.proc() != 0)
        {
            break;
        }
        usleep(10 * 1000); // 10 msec
    }

    log_print(log_level, "p520 client exit !!! \n");
    debug_finish();
    curl_global_cleanup();
    return ret_val;
}

