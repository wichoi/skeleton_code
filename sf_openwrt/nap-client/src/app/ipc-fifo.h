#ifndef __IPC_FIFO_H__
#define __IPC_FIFO_H__

#include <string>
#include "common.h"

#define IPC_RECV_BUF_SZ             1024
#define IPC_POLLING_BUF_SZ          2048 // polling buf sz * 2

typedef struct ipc_payload_tag_
{
    u8 _data[IPC_RECV_BUF_SZ];
    u16 _len;
} ipc_pay_t;

class ipc_fifo
{
public:
    ipc_fifo();
    ~ipc_fifo();
    int ipc_init(string &server, string &client);
    int ipc_deinit(void);
    int ipc_proc(void);
    int ipc_send(string &frame);

private:
    int ipc_polling(void);
    virtual int ipc_parser(ipc_pay_t *recv_data) = 0;
    int ipc_open_server(string &server);
    int ipc_open_client(string &client);

private:
    s32 _ipc_server;
    s32 _ipc_client;

    u8 _polling_st;
    u8 _poll_buf[IPC_POLLING_BUF_SZ];
    u16 _poll_head;
    u16 _poll_tail;

    ipc_pay_t _recv_data;
    u16 _recv_ix;
};

#endif
