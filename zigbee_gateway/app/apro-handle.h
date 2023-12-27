#ifndef _APRO_HANDLER_H_
#define _APRO_HANDLER_H_

#define APRO_MAX_NODE 32

typedef struct _tag_child
{
    u8 eui64[8];
    u8 type;
    u16 id;
    u8 phy;
    u8 power;
    u8 timeout;
} child_t;

typedef struct _tag_child_data
{
    u8 count;
    child_t child[APRO_MAX_NODE];
} child_data_t;

typedef struct _tag_neighbor_data
{
    u16 net_id;
    u8 average_lqi;
    u8 in_cost;
    u8 out_cost;
    u8 age;
    u8 eui64[8];
} neighbor_t;

typedef struct _tag_neighbor_table
{
    u8 count;
    neighbor_t neighbor[APRO_MAX_NODE];
} nei_table_t;

typedef struct _tag_route_dest
{
    u8 count;
    u16 node_id[APRO_MAX_NODE];
} dest_node_t;

int apro_handle_version(u8 *major, u8 *minor, u8 *patch, u16 *build);
int apro_handle_eui64(void);
int apro_handle_node_id(void);
int apro_handle_channel(void);
int apro_handle_set_confing(u8 id, u16 val);

int apro_handle_get_net_st(u8 *state);
int apro_handle_get_net_param(void);
int apro_handle_get_node_type(void);
int apro_handle_get_ncp_config(u8 id);
int apro_handle_get_route_table(dest_node_t *data);
int apro_handle_get_route_size(void);
int apro_handle_get_child(child_data_t *data);
int apro_handle_get_neighbor(nei_table_t *data);
int apro_handle_get_node_info(void);
int apro_handle_get_node_eui64(u16 node, u8* eui64);
int apro_handle_get_device_ep(u16 id);
int apro_handle_get_device_cluster_id(u16 id, u8 ep);

int apro_handle_bind_req(u8 endpoint);
int apro_handle_bind_clear(void);
int apro_handle_bind_print(void);

int apro_handle_net_form(u8 security, u16 pan_id, s8 power, u8 ch);
int apro_handle_net_start(void);
int apro_handle_net_stop(void);
int apro_handle_net_open(void);
int apro_handle_net_close(void);
int apro_handle_remove_device(u16 net_id);

int apro_handle_net_leave(void);
int apro_handle_send_unicast(zb_frame_t frame);
int apro_handle_send_broadcast(zb_frame_t frame);
int apro_handle_send_multicast(void);

int apro_handle_ncp_reboot(void);
int apro_handle_exit(void);


#endif
