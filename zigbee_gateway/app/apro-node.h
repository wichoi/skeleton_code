#ifndef __APRO_NODE_H__
#define __APRO_NODE_H__

#define APRO_CL_UNKNOWN     0
#define APRO_CL_IN          1
#define APRO_CL_OUT         2

#define APRO_CL_DATA_SZ 16

typedef struct _tag_cluster
{
    u8 direction; // in(APRO_CL_IN) out(APRO_CL_OUT)
    u16 id;
    u8 status;
    u8 data_len;
    u8 data[APRO_CL_DATA_SZ];
    u8 tmp[APRO_CL_DATA_SZ]; // Command execute and then wait response.
}t_cluster;

#define APRO_CLUSTER_MAX       20

typedef struct _tag_end_point
{
    u16 profile_id;
    u16 device_id;
    u8 ep_id;
    u16 cl_cnt;
    t_cluster cl[APRO_CLUSTER_MAX];
}t_end_point;

#define APRO_MAX_EP         10

// end devic info
typedef struct _tag_node_info
{
    u16 net_id;
    u8 eui64[8];
    u32 ocf_id;
    u32 ocf_type;
    u32 last_arrived_time; // time_t
    u8 lqi;
    s8 rssi;
    u8 ep_cnt;
    t_end_point ep[APRO_MAX_EP];
}t_node;

#define APRO_NODE_MAX       50

typedef struct _tag_node_manager
{
    u8 ver[2];
    u16 pan_id;
    u8 pan_id_ex[8];
    u8 channel;
    s8 tx_power;

    // coordinator info
    u16 net_id;
    u8 eui64[8];

    u8 node_cnt;
    t_node node[APRO_NODE_MAX];
}t_node_mgr;

int apro_node_manager_init(void);
int apro_node_add(u16 net_id, u8 *eui64);
int apro_node_del(u16 net_id);
int apro_node_update_ep(u16 net_id, u8 ep);
int apro_node_update_cl(u16 net_id, u8 ep, u8 in_out, u16 cl_id);
int apro_node_update_latest_time(u16 net_id);
int apro_node_update_rssi_lqi(u16 net_id, u8 lqi, s8 rssi);
int apro_node_update_value(u16 net_id, u8 ep, u16 cl, u8 *data, u16 len);
int apro_node_backup_value(u16 net_id, u8 ep, u16 cl, u8 *tmp, u16 len);
int apro_node_restore_value(u16 net_id, u8 ep, u16 cl, u8 *tmp, u8 *len);
t_node_mgr* apro_node_get(void);

#ifdef __cplusplus
extern "C" {
#endif
int apro_node_ocf_set(u16 net_id, u32 ocf_id, u32 ocf_type);
int apro_node_ocf_get(u16 net_id, u32 *ocf_id, u32 *ocf_type);
int apro_node_ocf_get_net_id(u32 ocf_id, u16 *net_id);
#ifdef __cplusplus
};
#endif

int apro_node_print(void);
int apro_node_file_wirte(void);
int apro_node_file_read(void);

#endif
