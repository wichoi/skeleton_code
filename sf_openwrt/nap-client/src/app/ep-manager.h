#ifndef __ENDPOINT_MANAGER_H__
#define __ENDPOINT_MANAGER_H__

#include "common.h"
#include "event-data.h"

class ep_manager
{
public:
    ep_manager();
    ~ep_manager();
    int init(void);
    int deinit(void);

    // ep log file manager
    int ep_log_read(void);
    int ep_log_write(dat_grpc_frame::item_list &ul_dat);
    int ep_log_delete(void);

private:
    list<string> _ep_item;
};

#endif
