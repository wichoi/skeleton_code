#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "apro-common.h"
#include "apro-log.h"
#include "apro-node.h"

#define NODE_FOLDER_CONF    "/mnt/data/config/"
#define NODE_FOLDER_ZB      "/mnt/data/config/zigbee/"
#define NODE_FILE_PATH      "/mnt/data/config/zigbee/zigbee-node.dat"

#define NODE_MGR_VER_MAJOR      0
#define NODE_MGR_VER_MINOR      2

static t_node_mgr g_node;

int apro_node_manager_init(void)
{
    log_i("%s\n", __func__);
    memset((char*)&g_node, 0, sizeof(g_node));
    apro_node_file_read();

    g_node.ver[0] = NODE_MGR_VER_MAJOR;
    g_node.ver[1] = NODE_MGR_VER_MINOR;

    return RET_SUCCESS;
}

int apro_node_add(u16 net_id, u8 *eui64)
{
    int ret_val = RET_ERROR;
    log_d("%s\n", __func__);

    if(net_id == 0x0000)
    {
        log_d("%s coordinator net id 0x0000\n", __func__);
        return ret_val;
    }

    if(0) // invalid eui64 check
    {
        log_w("%s invalid eui[%02x%02x%02x%02x%02x%02x%02x%02x]\n", __func__,
            eui64[0], eui64[1], eui64[2], eui64[3],
            eui64[4], eui64[5], eui64[6], eui64[7]);
        return ret_val;
    }

    if(g_node.node_cnt >= APRO_NODE_MAX)
    {
        log_w("%s node full\n", __func__);
        return ret_val;
    }

    int i = 0;
    for(i = 0; i < APRO_NODE_MAX; i++)
    {
        if(g_node.node[i].net_id == net_id)
        {
            if(memcmp(g_node.node[i].eui64, eui64, sizeof(eui64)) != 0)
            {
                memcpy(g_node.node[i].eui64, eui64, 8);
            }
            log_d("%s already exist [%d][0x%04X]\n", __func__, i, net_id);
            return ret_val;
        }
    }

    for(i = 0; i < APRO_NODE_MAX; i++)
    {
        if(g_node.node[i].net_id == 0)
        {
            // add
            g_node.node[i].net_id = net_id;
            memcpy(g_node.node[i].eui64, eui64, 8);
            g_node.node_cnt++;

            time_t timer = time(NULL);
            localtime(&timer);
            g_node.node[i].last_arrived_time = timer;

            ret_val = RET_SUCCESS;
            log_i("%s add [%d][0x%04X]\n", __func__, i, net_id);
            break;
        }
    }

    log_d("%s result %d\n", __func__, ret_val);
    return ret_val;
}

int apro_node_del(u16 net_id)
{
    int ret_val = RET_ERROR;
    log_i("%s\n", __func__);

    if(net_id == 0x0000)
    {
        log_d("%s coordinator net id 0x0000\n", __func__);
        return ret_val;
    }

    if(g_node.node_cnt == 0)
    {
        log_w("%s node not exist\n", __func__);
        return ret_val;
    }

    int i = 0;
    for(i = 0; i < APRO_NODE_MAX; i++)
    {
        if(g_node.node[i].net_id == net_id)
        {
            // delete
            memset((char*)&g_node.node[i], 0, sizeof(g_node.node[i]));
            g_node.node_cnt--;
            ret_val = RET_SUCCESS;
            log_i("%s delete [%d][0x%04X]\n", __func__, i, net_id);
            break;
        }
    }

    apro_node_file_wirte();
    log_i("%s result %d\n", __func__, ret_val);
    return ret_val;
}

int apro_node_update_ep(u16 net_id, u8 ep)
{
    int ret_val = RET_ERROR;
    log_d("%s net_id[0x%04X] ep[%u]\n", __func__, net_id, ep);

    if(net_id == 0x0000)
    {
        log_d("%s coordinator net id 0x0000\n", __func__);
        return ret_val;
    }

    int i = 0;
    for(i = 0; i < APRO_NODE_MAX; i++)
    {
        if(g_node.node[i].net_id == net_id)
        {
            if(g_node.node[i].ep_cnt >= APRO_MAX_EP)
            {
                log_w("EndPoint Full\n");
                return ret_val;
            }

            int j = 0;
            for(j = 0; j < APRO_MAX_EP; j++)
            {
                if(g_node.node[i].ep[j].ep_id == ep)
                {
                    log_d("%s already exist ep[%d]\n", __func__, j);
                    return ret_val;
                }
            }

            for(j = 0; j < APRO_MAX_EP; j++)
            {
                if(g_node.node[i].ep[j].ep_id == 0)
                {
                    g_node.node[i].ep[j].ep_id = ep;
                    g_node.node[i].ep_cnt++;
                    ret_val = RET_SUCCESS;
                    log_i("%s update ep[%d][%u]\n", __func__, j, ep);
                    return ret_val;
                }
            }
        }
    }

    log_d("%s result %d\n", __func__, ret_val);
    return RET_SUCCESS;
}

int apro_node_update_cl(u16 net_id, u8 ep, u8 in_out, u16 cl_id)
{
    int ret_val = RET_ERROR;
    log_d("%s net_id[0x%04X] ep[%u] in_out[%u] cl_id[0x%04X]\n", __func__, net_id, ep, in_out, cl_id);

    if(net_id == 0x0000)
    {
        log_d("%s coordinator net id 0x0000\n", __func__);
        return ret_val;
    }

    int i = 0;
    for(i = 0; i < APRO_NODE_MAX; i++)
    {
        if(g_node.node[i].net_id == net_id)
        {
            int j = 0;
            for(j = 0; j < APRO_MAX_EP; j++)
            {
                if(g_node.node[i].ep[j].ep_id == ep)
                {
                    if(g_node.node[i].ep[j].cl_cnt >= APRO_CLUSTER_MAX)
                    {
                        log_w("Cluster Full\n");
                        return ret_val;
                    }

                    int k = 0;
                    for(k = 0; k < APRO_CLUSTER_MAX; k++)
                    {
                        if(g_node.node[i].ep[j].cl[k].id == cl_id &&
                            g_node.node[i].ep[j].cl[k].direction == in_out)
                        {
                            log_d("%s already exist cluster[%d]\n", __func__, k);
                            return ret_val;
                        }
                    }

                    for(k = 0; k < APRO_CLUSTER_MAX; k++)
                    {
                        if(g_node.node[i].ep[j].cl[k].id == 0 &&
                            g_node.node[i].ep[j].cl[k].direction == APRO_CL_UNKNOWN)
                        {
                            g_node.node[i].ep[j].cl[k].id = cl_id;
                            g_node.node[i].ep[j].cl[k].direction = in_out;
                            g_node.node[i].ep[j].cl_cnt++;
                            ret_val = RET_SUCCESS;
                            log_i("%s update cl[%d][0x%04X]\n", __func__, k, cl_id);
                            return ret_val;
                        }
                    }
                }
            }
        }
    }

    log_d("%s result %d\n", __func__, ret_val);
    return ret_val;
}

int apro_node_update_latest_time(u16 net_id)
{
    int ret_val = RET_NEW_NODE;
    time_t timer = time(NULL);
    localtime(&timer);

    if(net_id == 0x0000)
    {
        log_d("%s coordinator net id 0x0000\n", __func__);
        return RET_SUCCESS;
    }

    int i = 0;
    for(i = 0; i < APRO_NODE_MAX; i++)
    {
        if(g_node.node[i].net_id == net_id)
        {
            g_node.node[i].last_arrived_time = timer;
            if(g_node.node[i].ep_cnt == 0)
            {
                ret_val = RET_UPDATE_EP; // needed ep update
                log_d("%s RET_UPDATE_EP\n", __func__);
                return ret_val;
            }
            else
            {
                ret_val = RET_SUCCESS;
                int j = 0;
                for(j = 0; j < APRO_MAX_EP; j++)
                {
                    if(g_node.node[i].ep[j].ep_id != 0 &&
                        g_node.node[i].ep[j].cl_cnt == 0)
                    {
                        ret_val = RET_UPDATE_CL; // needed cl update
                        log_d("%s RET_UPDATE_CL\n", __func__);
                        return ret_val;
                    }
                }
            }
            break;
        }
    }

    log_d("%s result[%d] net_id[0x%04x] time[%u]\n", __func__, ret_val, net_id, timer);
    return ret_val;
}

int apro_node_update_rssi_lqi(u16 net_id, u8 lqi, s8 rssi)
{
    int ret_val = RET_ERROR;

    if(net_id == 0x0000)
    {
        log_d("%s coordinator net id 0x0000\n", __func__);
        return RET_SUCCESS;
    }

    int i = 0;
    for(i = 0; i < APRO_NODE_MAX; i++)
    {
        if(g_node.node[i].net_id == net_id)
        {
            g_node.node[i].lqi = lqi;
            g_node.node[i].rssi = rssi;
            ret_val = RET_SUCCESS;
            log_d("net_id[0x%04x] lqi[0x%02x] rssi[%d]\n", net_id, lqi, rssi);
            break;
        }
    }

    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

int apro_node_update_value(u16 net_id, u8 ep, u16 cl, u8 *data, u16 len)
{
    int ret_val = RET_ERROR;
    log_d("%s net_id[0x%04X] ep[%u] cl_id[0x%04X]\n", __func__, net_id, ep, cl);

    if(net_id == 0x0000)
    {
        log_d("%s coordinator net id 0x0000\n", __func__);
        return ret_val;
    }

    if(len <= 0 || len >= APRO_CL_DATA_SZ)
    {
        log_w("%s invalid len = %u\n", __func__, len);
        return ret_val;
    }

    int i = 0;
    for(i = 0; i < APRO_NODE_MAX; i++)
    {
        if(g_node.node[i].net_id == net_id)
        {
            int j = 0;
            for(j = 0; j < APRO_MAX_EP; j++)
            {
                if(g_node.node[i].ep[j].ep_id == ep)
                {
                    int k = 0;
                    for(k = 0; k < APRO_CLUSTER_MAX; k++)
                    {
                        if(g_node.node[i].ep[j].cl[k].id == cl &&
                            g_node.node[i].ep[j].cl[k].direction != APRO_CL_UNKNOWN) // todo direction
                        {
                            g_node.node[i].ep[j].cl[k].data_len = len;
                            memcpy(g_node.node[i].ep[j].cl[k].data, data, len);
                            log_d("%s updata data [0x02x]\n", __func__, data[0]);
                            ret_val = RET_SUCCESS;
                            return ret_val;
                        }
                    }
                }
            }
        }
    }

    log_d("%s result %d\n", __func__, ret_val);
    return ret_val;
}

// Command execute then wait response.
int apro_node_backup_value(u16 net_id, u8 ep, u16 cl, u8 *tmp, u16 len)
{
    int ret_val = RET_ERROR;
    log_d("%s net_id[0x%04X] ep[%u] cl_id[0x%04X]\n", __func__, net_id, ep, cl);

    if(net_id == 0x0000)
    {
        log_d("%s coordinator net id 0x0000\n", __func__);
        return ret_val;
    }

    if(len <= 0 || len >= APRO_CL_DATA_SZ)
    {
        log_w("%s invalid len = %u\n", __func__, len);
        return ret_val;
    }

    int i = 0;
    for(i = 0; i < APRO_NODE_MAX; i++)
    {
        if(g_node.node[i].net_id == net_id)
        {
            int j = 0;
            for(j = 0; j < APRO_MAX_EP; j++)
            {
                if(g_node.node[i].ep[j].ep_id == ep)
                {
                    int k = 0;
                    for(k = 0; k < APRO_CLUSTER_MAX; k++)
                    {
                        if(g_node.node[i].ep[j].cl[k].id == cl &&
                            g_node.node[i].ep[j].cl[k].direction != APRO_CL_UNKNOWN) // todo direction
                        {
                            g_node.node[i].ep[j].cl[k].data_len = len;
                            memcpy(g_node.node[i].ep[j].cl[k].tmp, tmp, len);
                            log_d("%s updata data [0x02x]\n", __func__, tmp[0]);
                            ret_val = RET_SUCCESS;
                            return ret_val;
                        }
                    }
                }
            }
        }
    }

    log_d("%s result %d\n", __func__, ret_val);
    return ret_val;
}

// Command execute and wait response. And received response.
int apro_node_restore_value(u16 net_id, u8 ep, u16 cl, u8 *tmp, u8 *len)
{
    int ret_val = RET_ERROR;
    log_d("%s net_id[0x%04X] ep[%u] cl_id[0x%04X]\n", __func__, net_id, ep, cl);

    if(net_id == 0x0000)
    {
        log_d("%s coordinator net id 0x0000\n", __func__);
        return ret_val;
    }

    int i = 0;
    for(i = 0; i < APRO_NODE_MAX; i++)
    {
        if(g_node.node[i].net_id == net_id)
        {
            int j = 0;
            for(j = 0; j < APRO_MAX_EP; j++)
            {
                if(g_node.node[i].ep[j].ep_id == ep)
                {
                    int k = 0;
                    for(k = 0; k < APRO_CLUSTER_MAX; k++)
                    {
                        if(g_node.node[i].ep[j].cl[k].id == cl &&
                            g_node.node[i].ep[j].cl[k].direction != APRO_CL_UNKNOWN) // todo direction
                        {
                            memcpy(g_node.node[i].ep[j].cl[k].data,
                                g_node.node[i].ep[j].cl[k].tmp,
                                g_node.node[i].ep[j].cl[k].data_len);
                            memcpy(tmp, g_node.node[i].ep[j].cl[k].data,
                                g_node.node[i].ep[j].cl[k].data_len);
                            *len = g_node.node[i].ep[j].cl[k].data_len;
                            log_d("%s copy data \n", __func__);
                            ret_val = RET_SUCCESS;
                            return ret_val;
                        }
                    }
                }
            }
        }
    }

    log_d("%s result %d\n", __func__, ret_val);
    return ret_val;
}

t_node_mgr* apro_node_get(void)
{
    log_v("%s\n", __func__);
    return &g_node;
}

int apro_node_ocf_set(u16 net_id, u32 ocf_id, u32 ocf_type)
{
    int ret_val = RET_ERROR;
    log_d("%s\n", __func__);

    int i = 0;
    for(i = 0; i < APRO_NODE_MAX; i++)
    {
        if(g_node.node[i].net_id == net_id)
        {
            g_node.node[i].ocf_id = ocf_id;
            g_node.node[i].ocf_type = ocf_type;
            ret_val = RET_SUCCESS;
            break;
        }
    }

    log_d("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

int apro_node_ocf_get(u16 net_id, u32 *ocf_id, u32 *ocf_type)
{
    int ret_val = RET_ERROR;
    log_d("%s\n", __func__);

    int i = 0;
    for(i = 0; i < APRO_NODE_MAX; i++)
    {
        if(g_node.node[i].net_id == net_id)
        {
            *ocf_id = g_node.node[i].ocf_id;
            *ocf_type = g_node.node[i].ocf_type;
            ret_val = RET_SUCCESS;
            break;
        }
    }

    log_d("%s net_id[0x0%4x] ocf_id[%u] type[%u]\n", __func__, net_id, *ocf_id, *ocf_type);
    return ret_val;
}

int apro_node_ocf_get_net_id(u32 ocf_id, u16 *net_id)
{
    int ret_val = RET_ERROR;
    log_d("%s\n", __func__);

    int i = 0;
    for(i = 0; i < APRO_NODE_MAX; i++)
    {
        if(g_node.node[i].ocf_id == ocf_id &&
           g_node.node[i].net_id != 0)
        {
            *net_id = g_node.node[i].net_id;
            ret_val = RET_SUCCESS;
            break;
        }
    }

    log_d("%s ocf_id[%u] net_id[0x0%4x] \n", __func__, ocf_id, *net_id);
    return ret_val;
}

int apro_node_print(void)
{
    log_i("%s\n", __func__);
    int i, j, k;
    for(i = 0; i < APRO_NODE_MAX; i++)
    {
        if(g_node.node[i].net_id != 0x0000)
        {
            struct tm *t = localtime((time_t*)&g_node.node[i].last_arrived_time);
            log_i("net_id[0x%04X] ocf_id[%d] ocf_type[%d] "
                  "node_cnt[%d] eui64[%02x %02x %02x %02x %02x %02x %02x %02x] "
                  "latest[%04d-%02d-%02d %02d:%02d:%02d] "
                  "lqi[0x%02X], rssi[%d]\n",
                  g_node.node[i].net_id, g_node.node[i].ocf_id,
                  g_node.node[i].ocf_type, g_node.node[i].ep_cnt,
                  g_node.node[i].eui64[0], g_node.node[i].eui64[1],
                  g_node.node[i].eui64[2], g_node.node[i].eui64[3],
                  g_node.node[i].eui64[4], g_node.node[i].eui64[5],
                  g_node.node[i].eui64[6], g_node.node[i].eui64[7],
                  t->tm_year + 1900,
                  t->tm_mon + 1,
                  t->tm_mday,
                  t->tm_hour,
                  t->tm_min,
                  t->tm_sec,
                  g_node.node[i].lqi, g_node.node[i].rssi);
            for(j = 0; j < APRO_MAX_EP; j++)
            {
                if(g_node.node[i].ep[j].cl_cnt != 0)
                {
                    log_i("    cl_cnt[%d]\n", g_node.node[i].ep[j].cl_cnt);
                }

                if(g_node.node[i].ep[j].ep_id != 0)
                {
                    log_i("        ep[%d]\n", g_node.node[i].ep[j].ep_id);
                    for(k = 0; k < APRO_CLUSTER_MAX; k++)
                    {
                        if(g_node.node[i].ep[j].cl[k].direction != APRO_CL_UNKNOWN)
                        {
                            log_i("            cl[%d][0x%04X] ",
                                    g_node.node[i].ep[j].cl[k].direction,
                                    g_node.node[i].ep[j].cl[k].id);
                            if(g_node.node[i].ep[j].cl[k].data_len > 0)
                            {
                                debug_printf("data[ ");
                                int l = 0;
                                for(l = 0; l < g_node.node[i].ep[j].cl[k].data_len ; l++)
                                {
                                    debug_printf("%02X ", g_node.node[i].ep[j].cl[k].data[l]);
                                }
                                debug_printf("]");
                            }
                            debug_printf("\n");
                        }
                    }
                }
            }
        }
    }

    return RET_SUCCESS;
}

int apro_node_file_wirte(void)
{
    int ret_val = RET_SUCCESS;
    FILE *node_file = NULL;

    mkdir(NODE_FOLDER_CONF, 0x777);
    mkdir(NODE_FOLDER_ZB, 0x777);

    if(node_file == NULL)
    {
        node_file = fopen(NODE_FILE_PATH, "wb");
        if(node_file == NULL)
        {
            log_e("file open error, path(%s)\n", node_file);
            return RET_ERROR;
        }
    }

    size_t result = fwrite((char*)&g_node, 1, sizeof(g_node), node_file);
    if(result != sizeof(g_node))
    {
        log_i("data_size[%u] write_size[%u]\n", sizeof(g_node), result);
        ret_val = RET_ERROR;
    }

    if(node_file)
    {
        fclose(node_file);
        node_file = NULL;
    }

    log_i("%s result [%d]\n", __func__, ret_val);
    return ret_val;
}

int apro_node_file_read(void)
{
    int ret_val = RET_SUCCESS;
    FILE *node_file = NULL;
    if(node_file == NULL)
    {
        node_file = fopen(NODE_FILE_PATH, "rb+");
        if(node_file == NULL)
        {
            log_e("file open error, path(%s)\n", node_file);
            return RET_ERROR;
        }
    }

    size_t result = fread((char*)&g_node, 1, sizeof(g_node), node_file);
    if(result != sizeof(g_node))
    {
        log_i("node_size[%u] read_size[%u]\n", sizeof(g_node), result);
        ret_val = RET_ERROR;
    }

    if(node_file)
    {
        fclose(node_file);
        node_file = NULL;
    }

    if(g_node.ver[0] != NODE_MGR_VER_MAJOR ||
       g_node.ver[1] != NODE_MGR_VER_MINOR )
    {
        log_e("invalid node manager version [%u.%u]\n", g_node.ver[0], g_node.ver[1]);
        memset((char*)&g_node, 0, sizeof(g_node));
        ret_val = RET_ERROR;
    }

    log_i("%s result[%d] \n", __func__, ret_val);
    return ret_val;
}


