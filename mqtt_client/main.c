#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include "common.h"
#include "debug/dbg-log.h"
#include "cli/cli-menu.h"
#include "thyme-client.h"
#include "event/event.h"

#define CLI_MENU

typedef struct _log_level
{
    char level;
    char *str;
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

static int exit_flag = 0;

//static pthread_mutex_t  main_mutex = PTHREAD_MUTEX_INITIALIZER;
//    pthread_mutex_lock(&main_mutex);
//    pthread_mutex_unlock(&main_mutex);

static char arg_parser(int argc, char **argv, char *log_level)
{
    if(argc < 2)
    {
        return 0;
    }

    char *level = (char*)argv[1];
    int i = 0;
    for(i = 0; i<sizeof(log_tbl)/sizeof(log_tbl[0]); i++)
    {
        if(strcmp(log_tbl[i].str, level) == 0)
        {
            *log_level = log_tbl[i].level;
            return 1;
        }
    }
    return 0;
}

static void sig_cb(int sig)
{
    pthread_t self;
    self = pthread_self();
    printf("Sighandler... sig[%d], self [%ld] \n",sig, (long)self);
#if 0 // crash stack dump
    printf("calle function = 0x%x\n", __builtin_return_address(0)); 
    printf("caller function = 0x%x\n", __builtin_return_address(1)); 

    FILE *outfp = fopen("/etc/stackdump.txt", "wt");
    if (outfp != NULL)
    {
        fprintf(outfp, "signo_num:%d, thread_id:%d\n", sig, (int)pthread_self());
        fprintf(outfp,
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
        );
        fflush(outfp);
        fclose(outfp);
    }
    printf(
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
                "%08X %08X %08X %08X %08X %08X %08X %08X\n"
    );
    // #include <execinfo.h>
    // size = backtrace(array, 10);
    //size = backtrace(array, 10);
#endif
    exit(0);
}

static void *main_proc(void)
{
    while(1)
    {
        if(thyme_main_proc() == PROC_EXIT)
        {
            exit_flag = PROC_EXIT;
        }

        if(exit_flag == PROC_EXIT)
        {
            destroy_event();
            thyme_destroy();
            pthread_exit(0);
            break;
        }
        usleep(10 * 1000); // 10 msec
    }

    return NULL;
}

static void *cli_menu(void)
{
    while(1)
    {
        if(cli_proc() == PROC_EXIT)
        {
            exit_flag = PROC_EXIT;
        }

        if(exit_flag == PROC_EXIT)
        {
            pthread_exit(0);
            break;
        }
        usleep(10 * 1000); // 10 msec
    }
}

int main(int argc, char **argv)
{
    int ret_val = 0;
    char log_level = LOG_DEBUG; // LOG_NONE;

    signal(SIGINT, sig_cb);
    signal(SIGTERM, sig_cb);
    signal(SIGHUP, sig_cb);
    signal(SIGABRT, sig_cb);

    arg_parser(argc, argv, &log_level);
    debug_init(LOG_ENABLE, log_level, NULL);
    log_print(log_level, "nCube Thyme start !!!\n");

    init_event();
    put_event(AE_INIT, NULL, 0);

    pthread_t main_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if(pthread_create(&main_thread, &attr, (void*)main_proc, NULL) < 0)
    {
        log_E("pthread_create failed\n");
        exit(0);
    }

#ifdef CLI_MENU
    pthread_t cli_thread;
    pthread_attr_t cli_attr;
    pthread_attr_init(&cli_attr);
    if(pthread_create(&cli_thread, &cli_attr, (void*)cli_menu, NULL) < 0)
    {
        log_E("cli thread create failed \n");
        exit(0);
    }
#endif

    pthread_join(main_thread, (void*)&ret_val);
    log_print(log_level, "main_thread exit (%d)\n", ret_val);

#ifdef CLI_MENU
    pthread_join(cli_thread, (void*)&ret_val);
    log_print(log_level, "cli_menu exit (%d)\n", ret_val);
#endif

    log_print(log_level, "nCube Thyme exit (%d)\n", ret_val);
    return ret_val;
}

