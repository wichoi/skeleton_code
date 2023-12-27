#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "typedef.h"
#include "cli.h"
#include "app.h"

static bool g_exit = false;
static void cli_help(void);

static void cli_exit(void)
{
    g_exit = true;
}

static void cli_crc32(void)
{
    const u32 crc_key = CRC32_KEY;
    u32 crc_result = 0;
    char dat[128] = {0,};
    crc_result = app_crc32(dat, sizeof(dat), &crc_key);
    printf("%s crc_key[0x%08X] crc_result[0x%08X]\n", __func__, crc_key, crc_result);
}

static void cli_encrypt(void)
{
    app_encrypt((char*)"main.c", (char*)"main.c.en");
}

static void cli_decrypt(void)
{
    app_decrypt((char*)"main.c.en", (char*)"main.c.de");
}

static void cli_mkimg(void)
{
    info_t info = {"mkimg.exe", "AT-EV100", 0, 0, 1, 0, "release.img"};
    printf("%s input[%s] model[%s] version[%u.%u.%u] is_cryp[%u] out[%s]\n",
        __func__, info.in_file, info.model,
        info.ver_major, info.ver_minor, info.ver_build,
        info.is_crypto, info.out_file);
    app_proc(&info);
}

typedef struct
{
    const char *str;
    void (*pf)(void);
    const char *help;
} t_cmd;

const t_cmd cmd_tbl[] =
{
    { "help",       cli_help,       "print command list" },
    { "exit",       cli_exit,       "exit app" },
    { "crc",        cli_crc32,      "test crc" },
    { "en",         cli_encrypt,    "test aes encrypt" },
    { "de",         cli_decrypt,    "test aes decrypt" },
    { "mk",         cli_mkimg,      "test make image" },
};

int cli_proc(void)
{
    int ret_val = 0;
    int i = 0;
    char buf[128];
    cli_help();
    do
    {
        gets(buf);
        for(i = 0; i<sizeof(cmd_tbl)/sizeof(cmd_tbl[0]); i++)
        {
            if(strcmp(cmd_tbl[i].str, buf) == 0)
            {
                printf("input cmd : %s\n", cmd_tbl[i].str);
                (*(cmd_tbl[i].pf))();
                break;
            }
        }
    }while (g_exit != true);
    return ret_val;
}

static void cli_help(void)
{
    printf("\n===== command list =====\n");
    int i;
    for(i = 0; i<sizeof(cmd_tbl)/sizeof(cmd_tbl[0]); i++)
    {
        printf("%s\t:\t%s\n", cmd_tbl[i].str, cmd_tbl[i].help);
    }
    printf("========================\n");
}

