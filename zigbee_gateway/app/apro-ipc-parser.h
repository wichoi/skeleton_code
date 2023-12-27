#ifndef _APRO_IPC_PARSER_H_
#define _APRO_IPC_PARSER_H_

typedef struct _tag_ipc_payload
{
    u8 ver;
    u8 flag;
    u8 type;
    u8 ret;
    u8 src;
    u8 dest;
    u16 msg_id;
    u16 body_len;
    u8 body[128];
} ipc_pay_t;

int apro_ipc_parser(ipc_pay_t *recv_data);

#endif
