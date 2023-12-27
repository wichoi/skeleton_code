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
#include <sys/ipc.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>

#include "md5.h"

struct sockaddr_un {
    sa_family_t sun_family;	/* AF_UNIX */
    char sun_path[UNIX_PATH_MAX];	/* pathname */
};

static int recv_unsolicited(char *unsol)
{

UNSOL_ERR:
    return ret_val;
}

static int send_data(int fd, const void *data, int data_len)
{

    return 0;
}

static int recv_data(int fd, void *data, int *data_len, int timeout)
{

cleanup:
    return err;
}

static int rild_connect(void)
{

    return fd;
}

int sendAtCommand(char *at_cmd, char *at_resp, char *unsol_resp, int urc_read)
{

    close(fd);

    return ret_val;
}

static int modemFwWrite(char *filename, int file_begin, int file_end)
{

WR_ERR:
    fclose(fp);
    close(wfp);
    return ret_val;
}

static int calMD5Hash(char *filename, int file_begin, int file_end, char *buf)
{

MD5_ERR:
    fclose(fp);
    return ret_val;
}

int modemFwUpgrade(char *filename, int file_begin, int file_end, char *err_msg)
{

FW_ERR:
    return ret_val;
}

