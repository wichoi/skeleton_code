#include "af.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"

#include "apro-common.h"
#include "apro-log.h"
#include "apro-event.h"
#include "apro-zcl-basic.h"

int apro_zcl_basic_attribute(u16 net_id, u8 ep, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = EMBER_ZDO_PROFILE_ID;
    frame->cluster_id = ZCL_BASIC_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = CLUSTER_MASK_CLIENT;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_READ_ATTRIBUTES_COMMAND_ID;
    frame->msg[frame->msg_len++] = ZCL_VERSION_ATTRIBUTE_ID >> 8;
    frame->msg[frame->msg_len++] = ZCL_VERSION_ATTRIBUTE_ID;

    return ret_val;
}

