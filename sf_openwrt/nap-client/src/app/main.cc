#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <curl/curl.h>

#include "common.h"
#include "log.h"
#include "main-client.h"

#if 0
static void sig_cb(int sig)
{
    printf("sig_cb %d \n", sig);
    exit(0);
}
#endif

static char arg_parser(int argc, char **argv, char *log_level)
{
    int ret_val = RET_ERROR;

    typedef struct _log_level
    {
        char level;
        const char *str;
    } log_table_t;

    static const log_table_t log_tbl[] =
    {
        { LOG_NONE,     "-N" },
        { LOG_VERBOSE,  "-V" },
        { LOG_DEBUG,    "-D" },
        { LOG_INFO,     "-I" },
        { LOG_WARN,     "-W" },
        { LOG_ERROR,    "-E" },
        { LOG_FATAL,    "-F" },
    };

    if(argc < 2)
    {
        return ret_val;
    }

    char *level = (char*)argv[1];
    int i = 0;
    for(i = 0; i<sizeof(log_tbl)/sizeof(log_tbl[0]); i++)
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
    char log_level = LOG_INFO;

    // Ignore signal
    signal(SIGTTIN, SIG_IGN); // [1]+ Stopped (tty input) nap-client
    //signal(SIGBUS, sig_cb); // curl Bus error
    //signal(SIGILL, sig_cb); // curl Illegal instruction
    //signal(SIGSEGV, sig_cb); // curl Segmentation fault

    curl_global_init(CURL_GLOBAL_ALL);

    arg_parser(argc, argv, &log_level);
    debug_init(LOG_ENABLE, log_level, NAP_LOG_FILE);
    log_print(log_level, "skeleton client start !!! \n");

    main_client client;

    while(1)
    {
        if(client.proc() != 0)
        {
            break;
        }
        usleep(10 * 1000); // 10 msec
        //this_thread::yield();
    }

    log_print(log_level, "skeleton client exit !!! \n");
    debug_finish();
    curl_global_cleanup();
    return ret_val;
}
