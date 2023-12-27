#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>

#include "router-upgrade.h"

static void write_flash_kernel_version(void)
{
    char cmd[512] = {0,};
    system(cmd);
}

static unsigned int getMTDPartSize(const char *part)
{

    return result;
}

static int mtd_write_firmware(char *filename, int offset, int len)
{
    char cmd[512] = {0,};

    return 0;
}

/*
 *  taken from "mkimage -l" with few modified....
 */
static int check(char *imagefile, int offset, int len, char *err_msg)
{

    munmap(ptr, len);
    close(ifd);

    return 1;
}

int routerFwUpgrade(char *filename, int file_begin, int file_end, char *err_msg)
{

    // flash write
    if(mtd_write_firmware(filename, file_begin, file_end - file_begin) == -1)
    {
        sprintf(err_msg, "mtd_write fatal error! The corrupted image has ruined the flash!!");
        return -1;
    }

    return 0;
}

