#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "typedef.h"
#include "main.h"
#include "app.h"
#include "cli.h"

void print_help(void)
{
    printf("\n");
    printf("=============================================\n");
    printf("mkimg [input file] [model name] [version] [is_crypt(0 or 1)] [offset(0x27000)]\n");
    printf("ex) mkimg nrf52840_xxaa.bin AT-EV100 1.1.1 0 0x27000\n");
    printf("=============================================\n");
    printf("mkimg [test mode] \n");
    printf("ex) mkimg -t \n");
    printf("=============================================\n");
}

int main(int argc, char **argv)
{
    int ret_val = 0;
    printf("UniEv Firmware Manager\n");

    if(argc == 6)
    {
        info_t info = {0,};
        strncpy(info.in_file, argv[1], sizeof(info.in_file));
        strncpy(info.model, argv[2], sizeof(info.model));

        char *ptr = strtok(argv[3], "."); 
        if (ptr != NULL)
        {
            info.ver_major = atoi(ptr);
            ptr = strtok(NULL, ".");
            if (ptr != NULL)
            {
                info.ver_minor = atoi(ptr);
                ptr = strtok(NULL, ".");
                if (ptr != NULL)
                    info.ver_build = atoi(ptr);
            }
        }
        info.is_crypto = atoi(argv[4]);
        info.offset = strtol(argv[5], NULL, 16);

        snprintf(info.out_file, sizeof(info.out_file), "%s_fwup_v%u.%u.%u.img",
            info.model, info.ver_major, info.ver_minor, info.ver_build);

        printf("input[%s] model[%s] version[%u.%u.%u] is_cryp[%u] out[%s] offset[0x%X]\n",
            info.in_file, info.model, info.ver_major, info.ver_minor, info.ver_build,
            info.is_crypto, info.out_file, info.offset);

        if(strlen(info.in_file) > 0 && strlen(info.model) > 0 )
        {
            app_proc(&info);
        }
        else
        {
            printf("Invalid Input\n");
        }

    }
    else  if(argc == 2 && strcmp(argv[1], "-t") == 0)
    {
        cli_proc();
    }
    else
    {
        printf("Invalid Input !!!!!!!!!! \n");
        print_help();
    }

    printf("UniEv Firmware Manager Done\n");
    return ret_val;
}

