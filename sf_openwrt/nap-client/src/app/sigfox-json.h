#ifndef __SIGFOX_JSON_MANAGER_H__
#define __SIGFOX_JSON_MANAGER_H__

#include "common.h"
#include "json-manager.h"

class sigfox_json
{
public:
    sigfox_json();
    ~sigfox_json();
    int init(void);
    int deinit(void);

    int json_create(string &json_body, const event_c &ev);
    int json_parse(string &src_data, event_c &ev);
};

#endif
