#ifndef _APRO_OCF_PARSER_H_
#define _APRO_OCF_PARSER_H_

typedef struct _tag_ocf_data
{
    u32 ocf_id;
    u8 type;
    u8 value[16];
} ocf_data_t;

typedef struct _tag_ocf_recv_data
{
    u32 cnt;
    ocf_data_t data[16];
} ocf_recv_t;

int apro_ocf_parser(u8 *data, u32 len);

#ifdef __cplusplus
extern "C" {
#endif
int apro_ocf_put_event(u8 *data, u32 len);
#ifdef __cplusplus
};
#endif

#endif
