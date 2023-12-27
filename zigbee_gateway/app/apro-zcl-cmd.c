#include "af.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"

#include "apro-common.h"
#include "apro-log.h"
#include "apro-zcl-cmd.h"

#define LEN_0       0
#define LEN_1       1
#define LEN_2       2
#define LEN_3       3
#define LEN_4       4
#define LEN_5       5
#define LEN_6       6
#define LEN_7       7
#define LEN_8       8
#define LEN_16      16
#define LEN_READ    0xAF // first two octets
#define LEN_READ_2  0xBF // first two octets + 2
#define SUM_LEN     0xCF // sum of lengths of contents

typedef struct _tag_attr_data_type{
  u8 type;
  u8 len; // octets
} data_type_t;

const data_type_t d_type[] = {
    { ZCL_NO_DATA_ATTRIBUTE_TYPE,       LEN_0 },
    { ZCL_DATA8_ATTRIBUTE_TYPE,         LEN_1 },
    { ZCL_DATA16_ATTRIBUTE_TYPE,        LEN_2 },
    { ZCL_DATA24_ATTRIBUTE_TYPE,        LEN_3 },
    { ZCL_DATA32_ATTRIBUTE_TYPE,        LEN_4 },
    { ZCL_DATA40_ATTRIBUTE_TYPE,        LEN_5 },
    { ZCL_DATA48_ATTRIBUTE_TYPE,        LEN_6 },
    { ZCL_DATA56_ATTRIBUTE_TYPE,        LEN_7 },
    { ZCL_DATA64_ATTRIBUTE_TYPE,        LEN_8 },
    { ZCL_BOOLEAN_ATTRIBUTE_TYPE,       LEN_1 },
    { ZCL_BITMAP8_ATTRIBUTE_TYPE,       LEN_1 },
    { ZCL_BITMAP16_ATTRIBUTE_TYPE,      LEN_2 },
    { ZCL_BITMAP24_ATTRIBUTE_TYPE,      LEN_3 },
    { ZCL_BITMAP32_ATTRIBUTE_TYPE,      LEN_4 },
    { ZCL_BITMAP40_ATTRIBUTE_TYPE,      LEN_5 },
    { ZCL_BITMAP48_ATTRIBUTE_TYPE,      LEN_6 },
    { ZCL_BITMAP56_ATTRIBUTE_TYPE,      LEN_7 },
    { ZCL_BITMAP64_ATTRIBUTE_TYPE,      LEN_8 },
    { ZCL_INT8U_ATTRIBUTE_TYPE,         LEN_1 },
    { ZCL_INT16U_ATTRIBUTE_TYPE,        LEN_2 },
    { ZCL_INT24U_ATTRIBUTE_TYPE,        LEN_3 },
    { ZCL_INT32U_ATTRIBUTE_TYPE,        LEN_4 },
    { ZCL_INT40U_ATTRIBUTE_TYPE,        LEN_5 },
    { ZCL_INT48U_ATTRIBUTE_TYPE,        LEN_6 },
    { ZCL_INT56U_ATTRIBUTE_TYPE,        LEN_7 },
    { ZCL_INT64U_ATTRIBUTE_TYPE,        LEN_8 },
    { ZCL_INT8S_ATTRIBUTE_TYPE,         LEN_1 },
    { ZCL_INT16S_ATTRIBUTE_TYPE,        LEN_2 },
    { ZCL_INT24S_ATTRIBUTE_TYPE,        LEN_3 },
    { ZCL_INT32S_ATTRIBUTE_TYPE,        LEN_4 },
    { ZCL_INT40S_ATTRIBUTE_TYPE,        LEN_5 },
    { ZCL_INT48S_ATTRIBUTE_TYPE,        LEN_6 },
    { ZCL_INT56S_ATTRIBUTE_TYPE,        LEN_7 },
    { ZCL_INT64S_ATTRIBUTE_TYPE,        LEN_8 },
    { ZCL_ENUM8_ATTRIBUTE_TYPE,         LEN_1 },
    { ZCL_ENUM16_ATTRIBUTE_TYPE,        LEN_2 },
    { ZCL_FLOAT_SEMI_ATTRIBUTE_TYPE,    LEN_2 },
    { ZCL_FLOAT_SINGLE_ATTRIBUTE_TYPE,  LEN_4 },
    { ZCL_FLOAT_DOUBLE_ATTRIBUTE_TYPE,  LEN_8 },
    { ZCL_OCTET_STRING_ATTRIBUTE_TYPE,  LEN_READ },
    { ZCL_CHAR_STRING_ATTRIBUTE_TYPE,   LEN_READ },
    { ZCL_LONG_OCTET_STRING_ATTRIBUTE_TYPE, LEN_READ },
    { ZCL_LONG_CHAR_STRING_ATTRIBUTE_TYPE,  LEN_READ },
    { ZCL_ARRAY_ATTRIBUTE_TYPE,         LEN_READ_2 },
    { ZCL_STRUCT_ATTRIBUTE_TYPE,        LEN_READ_2 },
    { ZCL_SET_ATTRIBUTE_TYPE,           SUM_LEN },
    { ZCL_BAG_ATTRIBUTE_TYPE,           SUM_LEN },
    { ZCL_TIME_OF_DAY_ATTRIBUTE_TYPE,   LEN_4 },
    { ZCL_DATE_ATTRIBUTE_TYPE,          LEN_4 },
    { ZCL_UTC_TIME_ATTRIBUTE_TYPE,      LEN_4 },
    { ZCL_CLUSTER_ID_ATTRIBUTE_TYPE,    LEN_2 },
    { ZCL_ATTRIBUTE_ID_ATTRIBUTE_TYPE,  LEN_2 },
    { ZCL_BACNET_OID_ATTRIBUTE_TYPE,    LEN_4 },
    { ZCL_IEEE_ADDRESS_ATTRIBUTE_TYPE,  LEN_8 },
    { ZCL_SECURITY_KEY_ATTRIBUTE_TYPE,  LEN_16 },
    { ZCL_UNKNOWN_ATTRIBUTE_TYPE,       LEN_0 },
};

// ZCL_READ_ATTRIBUTES_COMMAND_ID
// Attr ID(2byte) ...
int apro_zcl_cmd_rd_attr(u8 *buf, u32 len, rd_attr_t *payload)
{
    int ret_val = RET_ERROR;
    memset(payload, 0, sizeof(rd_attr_t));

    if((len != 0) && (len%2 == 0))
    {
        int i = 0;
        for(i = 0; i < len; i+=2)
        {
            memcpy((char*)&payload->attr_id[payload->cnt], (char*)&buf[i], 2);
            payload->cnt++;
        }
        ret_val = RET_SUCCESS;
    }

    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID
// Attr ID(2byte) + status(1byte) + data type(1byte) + value(variable) ...
int apro_zcl_cmd_rd_attr_resp(u8 *buf, u32 len, rd_resp_t *payload)
{
    int ret_val = RET_ERROR;
    memset(payload, 0, sizeof(rd_attr_t));
    if(len != 0)
    {
        u32 ix = 0;
        int i = 0;
        while(ix < len)
        {
            // attribute id (2byte)
            memcpy((char*)&payload->field[payload->cnt].attr_id, (char*)&buf[ix], 2);
            ix += 2;

            // status (1byte)
            payload->field[payload->cnt].state = buf[ix];
            ix++;

            if(ix < len) // 0/1
            {
                for(i = 0; i < sizeof(d_type)/sizeof(d_type[0]); i++)
                {
                    if(d_type[i].type == buf[ix])
                    {
                        // attribyte data type (1byte)
                        payload->field[payload->cnt].data_type = buf[ix];
                        ix++;

                        // attribute value (variable)
                        switch(buf[ix])
                        {
                        case ZCL_OCTET_STRING_ATTRIBUTE_TYPE:
                        case ZCL_CHAR_STRING_ATTRIBUTE_TYPE:
                        case ZCL_LONG_OCTET_STRING_ATTRIBUTE_TYPE:
                        case ZCL_LONG_CHAR_STRING_ATTRIBUTE_TYPE:
                            log_w("%s result[%d] not implemented yet !!!\n", __func__, ret_val);
                            return ret_val;
                            break;

                        case ZCL_ARRAY_ATTRIBUTE_TYPE:
                        case ZCL_STRUCT_ATTRIBUTE_TYPE:
                            log_w("%s result[%d] not implemented yet !!!\n", __func__, ret_val);
                            return ret_val;
                            break;

                        case ZCL_SET_ATTRIBUTE_TYPE:
                        case ZCL_BAG_ATTRIBUTE_TYPE:
                            log_w("%s result[%d] not implemented yet !!!\n", __func__, ret_val);
                            return ret_val;
                            break;

                        default:
                            memcpy((char*)&payload->field[payload->cnt].data, (char*)&buf[ix], d_type[i].len);
                            ix += d_type[i].len;
                            break;
                        }
                    }
                }
            }
            log_d("%s len[%d] ix[%d]\n", __func__, len, ix);
            payload->cnt++;
            ret_val = RET_SUCCESS;
        };
    }

    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_WRITE_ATTRIBUTES_COMMAND_ID
// Attr ID(2byte) + data type(1byte) + data(variable) ...
int apro_zcl_cmd_wt_attr(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_WRITE_ATTRIBUTES_UNDIVIDED_COMMAND_ID
// status(1byte) + attr id(2byte) ...
int apro_zcl_cmd_wt_attr_undivided(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_WRITE_ATTRIBUTES_RESPONSE_COMMAND_ID
// status(1byte) + attr id(2byte) ...
int apro_zcl_cmd_wt_attr_resp(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_WRITE_ATTRIBUTES_NO_RESPONSE_COMMAND_ID
// Attr ID(2byte) + data type(1byte) + data(variable) ...
int apro_zcl_cmd_wt_attr_no_resp(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_CONFIGURE_REPORTING_COMMAND_ID
// direction(1byte) + attr id(2byte) + data type(1byte) +
// min report interval(2byte) + max report interval(2byte) +
// reportable change(variable) + tomeout period(2byte) ...
int apro_zcl_cmd_conf_rpt(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_CONFIGURE_REPORTING_RESPONSE_COMMAND_ID
// status(1byte) + direction(1byte) + attr id(2byte) ...
int apro_zcl_cmd_conf_rpt_resp(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_READ_REPORTING_CONFIGURATION_COMMAND_ID
// command id(1byte) + status code(1byte) ...
int apro_zcl_cmd_rd_rpt_conf(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_READ_REPORTING_CONFIGURATION_RESPONSE_COMMAND_ID
// status(1byte) + direction(1byte) + attr id(2byte) + data type(1byte) +
// min report interval(2byte) + max report interval(2byte) +
// reportable change(variable) + tomeout period(2byte) ...
int apro_zcl_cmd_rd_rpt_conf_resp(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_REPORT_ATTRIBUTES_COMMAND_ID
// attr id(2byte) + data type(1byte) + data(veriable) ...
int apro_zcl_cmd_rpt_attr(u8 *buf, u32 len, rpt_attr_r *payload)
{
    int ret_val = RET_ERROR;
    memset(payload, 0, sizeof(rd_attr_t));
    if(len != 0)
    {
        u32 ix = 0;
        int i = 0;
        while(ix < len)
        {
            for(i = 0; i < sizeof(d_type)/sizeof(d_type[0]); i++)
            {
                // attribute id (2byte)
                memcpy((char*)&payload->field[payload->cnt].attr_id, (char*)&buf[ix], 2);
                ix += 2;

                if(d_type[i].type == buf[ix])
                {
                    // attribyte data type (1byte)
                    payload->field[payload->cnt].type = buf[ix];
                    ix++;

                    // attribute value (variable)
                    switch(buf[ix])
                    {
                    case ZCL_OCTET_STRING_ATTRIBUTE_TYPE:
                    case ZCL_CHAR_STRING_ATTRIBUTE_TYPE:
                    case ZCL_LONG_OCTET_STRING_ATTRIBUTE_TYPE:
                    case ZCL_LONG_CHAR_STRING_ATTRIBUTE_TYPE:
                        log_w("%s result[%d] not implemented yet !!!\n", __func__, ret_val);
                        return ret_val;
                        break;

                    case ZCL_ARRAY_ATTRIBUTE_TYPE:
                    case ZCL_STRUCT_ATTRIBUTE_TYPE:
                        log_w("%s result[%d] not implemented yet !!!\n", __func__, ret_val);
                        return ret_val;
                        break;

                    case ZCL_SET_ATTRIBUTE_TYPE:
                    case ZCL_BAG_ATTRIBUTE_TYPE:
                        log_w("%s result[%d] not implemented yet !!!\n", __func__, ret_val);
                        return ret_val;
                        break;

                    default:
                        memcpy((char*)&payload->field[payload->cnt].data, (char*)&buf[ix], d_type[i].len);
                        ix += d_type[i].len;
                        break;
                    }
                }
            }
            log_d("%s len[%d] ix[%d]\n", __func__, len, ix);
            payload->cnt++;
            ret_val = RET_SUCCESS;
        };
    }

    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}


// ZCL_DEFAULT_RESPONSE_COMMAND_ID
// cmd id(1byte) + status code(1byte)
int apro_zcl_cmd_def_resp(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_DISCOVER_ATTRIBUTES_COMMAND_ID
// start attr id(2byte) + maximun attr id(1byte)
int apro_zcl_cmd_discover_attr(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_DISCOVER_ATTRIBUTES_RESPONSE_COMMAND_ID
// discover complete(1byte) + (attr id(2byte) + data type(1byte) ... )
int apro_zcl_cmd_discover_attr_resp(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_READ_ATTRIBUTES_STRUCTURED_COMMAND_ID
// attr id(2byte) + (indicator(m) + index 1 ... index m) ...
int apro_zcl_cmd_rd_attr_st(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_WRITE_ATTRIBUTES_STRUCTURED_COMMAND_ID
// attr id(2byte) + selector(indicator(m) + index 1 ... index m) + data type(1byte) + value(variable) ...
int apro_zcl_cmd_wt_attr_st(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_WRITE_ATTRIBUTES_STRUCTURED_RESPONSE_COMMAND_ID
// status(1byte) + attr id(2byte) + selector() ...
int apro_zcl_cmd_wt_attr_st_resp(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_DISCOVER_COMMANDS_RECEIVED_COMMAND_ID
// start cmd id(1byte) + max cmd id(1byte)
int apro_zcl_cmd_discover_cmd_recv(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_DISCOVER_COMMANDS_RECEIVED_RESPONSE_COMMAND_ID
// start cmd id(1byte) + max cmd id(1byte)
int apro_zcl_cmd_discover_cmd_recv_resp(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_DISCOVER_COMMANDS_GENERATED_COMMAND_ID
// start cmd id(1byte) + max cmd id(1byte)
int apro_zcl_cmd_discover_cmd_gen(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_DISCOVER_COMMANDS_GENERATED_RESPONSE_COMMAND_ID
// start cmd id(1byte) + max cmd id(1byte)
int apro_zcl_cmd_discover_cmd_gen_resp(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_DISCOVER_ATTRIBUTES_EXTENDED_COMMAND_ID
// start attr id (2byte) + max attr id(1byte)
int apro_zcl_cmd_discover_attr_ex(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

// ZCL_DISCOVER_ATTRIBUTES_EXTENDED_RESPONSE_COMMAND_ID
// discovery complete(1byte) + ex attr info(4byte) + ... ex attr info(4byte)
// ex attr info(attr id(2byte) + data type(1byte) + control (1byte))
int apro_zcl_cmd_discover_attr_ex_resp(void)
{
    int ret_val = RET_SUCCESS;
    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

u8 apro_zcl_cmd_get_attr_sz(u8 type)
{
    int i = 0;
    for(i = 0; i < sizeof(d_type)/sizeof(d_type[0]); i++)
    {
        if(d_type[i].type == type)
        {
            return d_type[i].len;
        }
    }

    return LEN_0;
}

