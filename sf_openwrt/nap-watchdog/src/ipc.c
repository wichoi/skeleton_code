#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/file.h>
#include <fcntl.h>
#include <pthread.h>

#include "ipc.h"

// sync  len  data
// (4)   (2) (var)

// sync (4byte) : sync
// len  (2byte) : data len (network order)
// data (var  ) : variable data

// syncword
#define IPC_SYNC_0              's'
#define IPC_SYNC_1              'y'
#define IPC_SYNC_2              'n'
#define IPC_SYNC_3              'c'

// state
#define IPC_ST_SYNC_0           0
#define IPC_ST_SYNC_1           1
#define IPC_ST_SYNC_2           2
#define IPC_ST_SYNC_3           3
#define IPC_ST_LEN_0            4
#define IPC_ST_LEN_1            5
#define IPC_ST_DATA             6

#define IPC_MSG_SZ              256
#define IPC_PIPE_SERVER         "/tmp/ipc_nap_to_watch"
#define IPC_PIPE_CLIENT         "/tmp/ipc_watch_to_nap"

#define IPC_RECV_BUF_SZ         1024
#define IPC_POLLING_BUF_SZ      2048 // polling buf sz * 2

// command
#define IPC_HELLO               '0'
#define IPC_SYSTEM              '1'
#define IPC_POPEN               '2'

#define POPEN_READ 0
#define POPEN_WRITE 1

typedef struct ipc_payload_tag_
{
    u8 _data[IPC_RECV_BUF_SZ];
    u16 _len;
} ipc_pay_t;

static int _fd_server = -1;
static int _fd_client = -1;

static u8 _ipc_st = 0;
static u8 _recv_buf[IPC_POLLING_BUF_SZ] = {0,};
static u16 _recv_head = 0;
static u16 _recv_tail = 0;

static ipc_pay_t _recv_data = {0,};
static u16 _recv_ix = 0;

static int ipc_open_server(void);
static int ipc_open_client(void);
static int ipc_parser(ipc_pay_t *recv_data);

static pid_t popen_proc(const char *command, int *infp, int *outfp)
{
    int p_stdin[2], p_stdout[2];
    pid_t pid;

    if(pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
    {
        return -1;
    }

    pid = fork();
    if(pid < 0)
    {
        return pid;
    }
    else if (pid == 0)
    {
        dup2(p_stdin[POPEN_READ], STDIN_FILENO);
        dup2(p_stdout[POPEN_WRITE], STDOUT_FILENO);

        // close unuse descriptors on child process.
        close(p_stdin[POPEN_READ]);
        close(p_stdin[POPEN_WRITE]);
        close(p_stdout[POPEN_READ]);
        close(p_stdout[POPEN_WRITE]);

        // can change to any exec* function family.
        execl("/bin/sh", "sh", "-c", command, NULL);
        perror("execl");
        exit(1);
    }

    // close unused descriptors on parent process.
    close(p_stdin[POPEN_READ]);
    close(p_stdout[POPEN_WRITE]);

    if(infp == NULL)
    {
        close(p_stdin[POPEN_WRITE]);
    }
    else
    {
        *infp = p_stdin[POPEN_WRITE];
    }

    if(outfp == NULL)
    {
        close(p_stdout[POPEN_READ]);
    }
    else
    {
        *outfp = p_stdout[POPEN_READ];
    }

    return pid;
}

int ipc_init(void)
{
    printf("%s\n", __func__);

    ipc_open_server();
    ipc_open_client();

    _ipc_st = IPC_ST_SYNC_0;
    memset((char*)_recv_buf, 0, sizeof(_recv_buf));
    _recv_head = 0;
    _recv_tail = 0;

    memset((char*)&_recv_data, 0, sizeof(_recv_data));
    _recv_ix = 0;

    return 0;
}

int ipc_deinit(void)
{
    printf("%s\n", __func__);
    close(_fd_server);
    _fd_server = -1;

    close(_fd_client);
    _fd_client = -1;

    _ipc_st = IPC_ST_SYNC_0;
    memset((char*)_recv_buf, 0, sizeof(_recv_buf));
    _recv_head = 0;
    _recv_tail = 0;

    memset((char*)&_recv_data, 0, sizeof(_recv_data));
    _recv_ix = 0;

    return 0;
}

static int ipc_open_server(void)
{
    int ret_val = RET_OK;

    if(_fd_server < 0)
    {
        int st = mkfifo(IPC_PIPE_SERVER, 0666);
        if(st < 0) // EEXIST
        {
           ; //printf("%s make pipe[%d] %s \n", __func__, st, IPC_PIPE_SERVER);
        }

        _fd_server = open(IPC_PIPE_SERVER, O_RDWR | O_NONBLOCK);
        if(_fd_server < 0)
        {
            printf("%s fail to open pipe %s \n", __func__, IPC_PIPE_SERVER);
            ret_val = RET_ERROR;
        }
    }

    u8 read_buf[IPC_MSG_SZ] = {0,};
    while(read(_fd_server, read_buf, sizeof(read_buf)) > 0)
    {
        // fifo clear
    }

    //printf("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

static int ipc_open_client(void)
{
    int ret_val = RET_OK;
    if(_fd_client < 0)
    {
        int st = mkfifo(IPC_PIPE_CLIENT, 0666);
        if(st < 0) // EEXIST
        {
           ; //printf("%s make pipe[%d] %s \n", __func__, st, IPC_PIPE_CLIENT);
        }

        _fd_client = open(IPC_PIPE_CLIENT, O_RDWR | O_NONBLOCK);
        if(_fd_client < 0)
        {
            printf("%s fail to open pipe %s \n", __func__, IPC_PIPE_CLIENT);
            ret_val = RET_ERROR;
        }
    }

    //printf("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int ipc_proc(void)
{
    u8 read_buf[IPC_MSG_SZ] = {0,};
    int read_len = read(_fd_server, read_buf, sizeof(read_buf));
    //printf("%s read_len[%d] \n", __func__, read_len);

    int i = 0;
    for(i = 0; i < read_len; i++)
    {
        _recv_buf[_recv_head++] = read_buf[i];
        if(_recv_head >= sizeof(_recv_buf))
        {
            _recv_head = 0;
        }
    }

    while(_recv_head != _recv_tail)
    {
        u8 rx = _recv_buf[_recv_tail++];
        //printf("%s [0x%02x]\n", __func__, rx);
        if(_recv_tail >= sizeof(_recv_buf))
        {
            _recv_tail = 0;
        }

        switch(_ipc_st)
        {
        case IPC_ST_SYNC_0:
            if(rx == IPC_SYNC_0)
            {
                //printf("%s IPC_ST_SYNC_0[0x%02x]\n", __func__, rx);
                _ipc_st = IPC_ST_SYNC_1;
            }
            break;

        case IPC_ST_SYNC_1:
            if(rx == IPC_SYNC_1)
            {
                //printf("%s IPC_ST_SYNC_1[0x%02x]\n", __func__, rx);
                _ipc_st = IPC_ST_SYNC_2;
            }
            else if(rx == IPC_SYNC_0)
            {
                _ipc_st = IPC_ST_SYNC_1;
            }
            else
            {
                _ipc_st = IPC_ST_SYNC_0;
            }
            break;

        case IPC_ST_SYNC_2:
            if(rx == IPC_SYNC_2)
            {
                //printf("%s IPC_ST_SYNC_2[0x%02x]\n", __func__, rx);
                _ipc_st = IPC_ST_SYNC_3;
            }
            else if(rx == IPC_SYNC_0)
            {
                _ipc_st = IPC_ST_SYNC_1;
            }
            else
            {
                _ipc_st = IPC_ST_SYNC_0;
            }
            break;

        case IPC_ST_SYNC_3:
            if(rx == IPC_SYNC_3)
            {
                //printf("%s IPC_ST_SYNC_3[0x%02x]\n", __func__, rx);
                memset((char*)&_recv_data, 0, sizeof(_recv_data));
                _ipc_st = IPC_ST_LEN_0;
            }
            else if(rx == IPC_SYNC_0)
            {
                _ipc_st = IPC_ST_SYNC_1;
            }
            else
            {
                _ipc_st = IPC_ST_SYNC_0;
            }
            break;

        case IPC_ST_LEN_0:
            //printf("%s data len hi[0x%02x]\n", __func__, rx);
            _recv_data._len = rx << 8;
            _ipc_st = IPC_ST_LEN_1;
            break;

        case IPC_ST_LEN_1:
            //printf("%s data len lo[0x%02x]\n", __func__, rx);
            _recv_data._len |= rx;
            _recv_ix = 0;

            if(_recv_data._len > 0)
            {
                _ipc_st = IPC_ST_DATA;
            }
            else
            {
                //ipc_parser(&_recv_data);
                _ipc_st = IPC_ST_SYNC_0;
            }
            break;

        case IPC_ST_DATA:
            if(_recv_data._len > 0)
            {
                //printf("%s data [0x%02x]\n", __func__, rx);
                _recv_data._len--;
                _recv_data._data[_recv_ix++] = rx;
            }

            if(_recv_data._len == 0)
            {
                _recv_data._len = _recv_ix;
                ipc_parser(&_recv_data);
                _ipc_st = IPC_ST_SYNC_0;
                return RET_OK;
            }
            break;

        default:
            printf("Unknown IPC State !!! \n");
            _ipc_st = IPC_ST_SYNC_0;
            break;
        }
    }

    return RET_WAIT;
}

static int ipc_parser(ipc_pay_t *recv_data)
{
    int ret_val = RET_OK;
    char data[IPC_RECV_BUF_SZ] = {0,};
//    printf("%s len[%d] cmd[0x%02X] data[%s]\n", __func__,
//        recv_data->_len, recv_data->_data[0], &(recv_data->_data[1]));
    switch(recv_data->_data[0])
    {
    case IPC_HELLO:
        {
            data[0] = IPC_HELLO;
            ipc_send(data);
        }
        break;
    case IPC_SYSTEM:
        {
            data[0] = IPC_SYSTEM;
            ipc_send(data);
            system(&(recv_data->_data[1]));
        }
        break;
    case IPC_POPEN:
        {
            int *outfp = NULL;
            FILE* fp = fopen("/tmp/rms-cmd.out", "w");
            pid_t pid = popen_proc(&(recv_data->_data[1]), NULL, &outfp);
            int wait_cnt = 0;
            while(waitpid(pid, NULL, WNOHANG) == 0)
            {
                if(++wait_cnt < 3)
                {
                    //printf("popen_proc waitpid !!!\n");
                    sleep(1);
                }
                else
                {
                    //printf("popen_proc kill !!!\n");
                    kill(pid, 9);
                    //wait(NULL);
                    wait_cnt = 0;
                    while(waitpid(pid, NULL, WNOHANG) == 0)
                    {
                        //printf("popen_proc wait_cnt[%d] !!!\n", wait_cnt);
                        if(++wait_cnt < 5)
                        {
                            sleep(1);
                        }
                        else
                        {
                            break;
                        }
                    }
                    break;
                }
            }

            //printf("popen_proc read !!!\n");
            if(fp != NULL && outfp != NULL)
            {
                int len = 0;
                char buf[4096] = {0,};
                int retval = fcntl(outfp, F_SETFL, fcntl(outfp, F_GETFL) | O_NONBLOCK);
                //printf("popen_proc fcntl: %d\n", retval);
                wait_cnt = 0;
                while(++wait_cnt < 5) // max 4096 * 4
                {
                    len = read(outfp, buf, sizeof(buf));
                    if(len > 0)
                    {
                        fwrite(buf, sizeof(char), len, fp);
                    }
                    else
                    {
                        break;
                    }
                }
            }

            close(outfp);
            fclose(fp);
            //printf("popen_proc done !!!\n");

            data[0] = IPC_POPEN;
            ipc_send(data);
        }
        break;
    }
    return ret_val;
}

int ipc_send(char* data)
{
    int ret_val = RET_OK;

    int write_len = 0;
    char *sync = "sync";
    u16 body_len = (u16)strlen(data);

    write_len = write(_fd_client, sync, strlen(sync));
    if(write_len != strlen(sync))
    {
        ret_val = RET_ERROR;
        printf("%s pipe write fail !!! [sync] \n", __func__);
        goto free_all;
    }

    char len_buf[2];
    len_buf[0] = (body_len >> 8) & 0xFF;
    len_buf[1] = body_len & 0xFF;
    write_len = write(_fd_client, (char*)len_buf, 2);
    if(write_len != 2)
    {
        ret_val = RET_ERROR;
        printf("%s pipe write fail !!! [len] \n", __func__);
        goto free_all;
    }

    write_len = write(_fd_client, data, strlen(data));
    if(write_len != strlen(data))
    {
        ret_val = RET_ERROR;
        printf("%s pipe write fail !!! [data] \n", __func__);
        goto free_all;
    }

    //printf("%s len[%d]\n%s\n", __func__, body_len, data);
free_all:
    //printf("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

