#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"
#include "log.h"
#include "utils.h"
#include "main-client.h"

// sync  pid  len  data
// (4)   (4)  (2) (var)

// sync (4byte) : sync
// pid  (4byte) : pid
// len  (2byte) : data len (network order)
// data (var  ) : variable data

// syncword
#define IPC_SYNC_0          's'
#define IPC_SYNC_1          'y'
#define IPC_SYNC_2          'n'
#define IPC_SYNC_3          'c'

#define IPC_PIPE_WRITE      "/tmp/ipc_console_to_at"
#define IPC_PIPE_READ       "/tmp/ipc_at_to_console"

// state
#define IPC_ST_SYNC_0       0
#define IPC_ST_SYNC_1       1
#define IPC_ST_SYNC_2       2
#define IPC_ST_SYNC_3       3
#define IPC_ST_PID_0        4
#define IPC_ST_PID_1        5
#define IPC_ST_PID_2        6
#define IPC_ST_PID_3        7
#define IPC_ST_LEN_0        8
#define IPC_ST_LEN_1        9
#define IPC_ST_DATA         10

typedef struct ipc_payload_tag_
{
    u8 _data[256];
    u16 _len;
    int _pid;
} ipc_pay_t;

static char arg_parser(int argc, char **argv)
{
    // fifo handle
    s32 fd_write = 0;
    s32 fd_read = 0;

    // tx buffer
    char buf[256] = {0,};
    char *p_buf = (char*)&buf[10];
    u16 i = 0;
    u16 len = 0;
    int write_len = 0;

    // rx buffer
    int timeout = 0;
    u8 _ipc_st = 0;
    u8 _recv_buf[512] = {0,};
    u16 _recv_head = 0;
    u16 _recv_tail = 0;
    ipc_pay_t _recv_data = {0,};
    u16 _recv_ix = 0;
    int read_len = 0;
    int urc_flag = 0;
    int pid = (int)getpid(); // pid_t

    if(argc < 2)
    {
        printf("atcmd invalid input !!! \n");
        printf("daemon    : atcmd -d & \n");
        printf("cmd input : atcmd ATI \n");
        printf("urc       : atcmd -u AT+QPING=1,\\\"www.google.com\\\" \n");
        return RET_OK;
    }
    else if(argc == 2)
    {
        if(strcmp((char*)argv[1], "-d") == 0)
        {
            return RET_ERROR;
        }
    }

    if(strcmp((char*)argv[1], "-u") == 0)
    {
        urc_flag = 1;
    }

    if(utils::read_uptime() < 60)
    {
        printf("atcmd waiting initialize ... \n");
        return RET_OK;
    }

    // ============================================================
    // open TX pipe
    mkfifo(IPC_PIPE_WRITE, 0666);
    fd_write = open(IPC_PIPE_WRITE, O_RDWR | O_NONBLOCK);
    if(fd_write < 0)
    {
        printf("%s fail to open pipe %s \n", __func__, IPC_PIPE_WRITE);
        return RET_OK;
    }

    // ============================================================
    // open RX pipe
    mkfifo(IPC_PIPE_READ, 0666);
    fd_read = open(IPC_PIPE_READ, O_RDWR | O_NONBLOCK);
    if(fd_read < 0)
    {
        close(fd_write);
        printf("%s fail to open pipe %s \n", __func__, IPC_PIPE_READ);
        return RET_OK;
    }

    while(read(fd_read, buf, sizeof(buf)) > 0)
    {
        // fifo clear
    }

    // ============================================================
    // atcmd TX
    for(i = (1 + urc_flag); i < argc; i++)
    {
        sprintf((char*)&p_buf[len], "%s", (char*)argv[i]);
        len += strlen((char*)argv[i]);
        if(i < argc - 1)
        {
            sprintf((char*)&p_buf[len], " ");
            len += 1;
        }
    }
    sprintf((char*)&p_buf[len], "\r\n");
    len += 2;
    //printf("len[%d] %s\n", len, p_buf);

    buf[0] = IPC_SYNC_0;
    buf[1] = IPC_SYNC_1;
    buf[2] = IPC_SYNC_2;
    buf[3] = IPC_SYNC_3;
    buf[4] = (pid >> 24) & 0xFF;
    buf[5] = (pid >> 16) & 0xFF;
    buf[6] = (pid >>  8) & 0xFF;
    buf[7] = (pid      ) & 0xFF;
    buf[8] = (len >> 8) & 0xFF;
    buf[9] = len & 0xFF;

    write_len = write(fd_write, buf, len + 10);
    if(write_len < 0 || write_len != (len + 10))
    {
        printf("%s pipe write fail !!! \n", __func__);
    }
    close(fd_write);

    // ============================================================
    // atcmd RX
    while(1)
    {
        memset(buf, 0, sizeof(buf));
        read_len = read(fd_read, buf, sizeof(buf));
        for(i = 0; i < read_len; i++)
        {
            _recv_buf[_recv_head++] = buf[i];
            if(_recv_head >= sizeof(_recv_buf))
            {
                _recv_head = 0;
            }
        }

        while(_recv_head != _recv_tail)
        {
            u8 rx = _recv_buf[_recv_tail++];
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
                    memset((char*)&_recv_data, 0, sizeof(ipc_pay_t));
                    _ipc_st = IPC_ST_PID_0;
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

            case IPC_ST_PID_0:
                _recv_data._pid = rx << 24;
                _ipc_st = IPC_ST_PID_1;
                break;

            case IPC_ST_PID_1:
                _recv_data._pid |= rx << 16;
                _ipc_st = IPC_ST_PID_2;
                break;

            case IPC_ST_PID_2:
                _recv_data._pid |= rx << 8;
                _ipc_st = IPC_ST_PID_3;
                break;

            case IPC_ST_PID_3:
                _recv_data._pid |= rx;
                _ipc_st = IPC_ST_LEN_0;
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
                    _ipc_st = IPC_ST_SYNC_0;
                    //printf("pid %d, %d", pid, _recv_data._pid);
                    if(pid == _recv_data._pid)
                    {
                        printf("%s", _recv_data._data);
                    }

                    if(urc_flag == 0)
                    {
                        return RET_OK;
                    }
                }
                break;

            default:
                _ipc_st = IPC_ST_SYNC_0;
                break;
            }
        }

        // AT_RECV_TIMEOUT
        usleep(100 * 1000); // 100 msec
        if(timeout++ > 20)
        {
            //printf("atcmd timeout !!! \n");
            return RET_OK;
        }
    }

    close(fd_read);
    return RET_OK;
}

int main(int argc, char** argv)
{
    int ret_val = RET_OK;

    // 3(lte not support), 5(wifi not support), 7(all support)
    FILE *fp = popen("nvram get 2860 NAP_Model", "r");
    if(fp != NULL)
    {
        int nap_model = 0;
        fscanf(fp, "%d", &nap_model);
        pclose(fp);
        if(nap_model == 3)
        {
            printf("NAP3 device !!! atcmd not support !!! \n");
            return ret_val;
        }
    }

    if(arg_parser(argc, argv) != RET_OK)
    {
        char log_level = LOG_INFO;

        // Ignore signal
        signal(SIGTTIN, SIG_IGN); // [1]+ Stopped (tty input)
        debug_init(LOG_ENABLE, log_level, NULL);
        log_print(log_level, "atcmd daemon start !!! \n");

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

        log_print(log_level, "atcmd daemon exit !!! \n");
        debug_finish();
    }
    return ret_val;
}

