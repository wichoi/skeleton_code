#ifndef _APRO_ZCL_CMD_H_
#define _APRO_ZCL_CMD_H_

typedef struct _tab_zcl_read_attr
{
    u32 cnt;
    u16 attr_id[16];
}rd_attr_t;

typedef struct _tab_zcl_read_attr_resp_fields
{
    u16 attr_id;
    u8 state;
    u8 data_type;
    u8 data[16];
}resp_field_t;

typedef struct _tab_zcl_read_attr_resp
{
    u32 cnt;
    resp_field_t field[16];
}rd_resp_t;


typedef struct _tab_zcl_rpt_fields
{
    u16 attr_id;
    u8 type;
    u8 data[16];
}rpt_field_t;

typedef struct _tab_zcl_report_attr
{
    u32 cnt;
    rpt_field_t field[16];
}rpt_attr_r;

int apro_zcl_cmd_rd_attr(u8 *buf, u32 len, rd_attr_t *payload);
int apro_zcl_cmd_rd_attr_resp(u8 *buf, u32 len, rd_resp_t *payload);
int apro_zcl_cmd_rpt_attr(u8 *buf, u32 len, rpt_attr_r *payload);
u8 apro_zcl_cmd_get_attr_sz(u8 type);

#endif
