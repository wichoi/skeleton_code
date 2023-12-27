#ifndef __CONFIG_HANDLER_H__
#define __CONFIG_HANDLER_H__

#include "common.h"
#include "json-manager.h"

class config_handler
{
public:
    config_handler();
    ~config_handler();
    int init(void);
    int deinit(void);

    int object_list(list<json_param> &obj_list);
    int read_config(bool ver_check = false);
    int write_config(void);
    int delete_config(void);
    int read_nvram(void);
    int write_nvram(void);

private:
    json_manager _json_mgr;
};

#endif
