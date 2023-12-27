#ifndef __IPC_CONSOLE_H__
#define __IPC_CONSOLE_H__

#include "common.h"
#include "main-interface.h"

class ipc_console :
    public main_interface,
    public event_listener
{
public:
    ipc_console();
    ~ipc_console();
    int init(main_interface *p_if);
    int deinit(void);
    int ipc_send(char *send_data, u32 len);
    int ipc_proc(void);

private:
    typedef struct ipc_payload_tag_
    {
        u8 _data[512];
        u16 _len;
        int _pid;
    } ipc_pay_t;

private:
    int ipc_open_server(void);
    int ipc_open_client(void);
    int ipc_parser(ipc_pay_t *recv_data);

    // event_listener
    int on_event(const event_c &ev);

private:
    main_interface *_p_main;

    s32 _fd_server;
    s32 _fd_client;

    u8 _ipc_st;
    u8 _recv_buf[512];
    u16 _recv_head;
    u16 _recv_tail;

    ipc_pay_t _recv_data;
    u16 _recv_ix;
};

#endif
