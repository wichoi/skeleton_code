#ifndef __NAP_CONFIG_H__
#define __NAP_CONFIG_H__

#include <string>
#include <list>

#include "common.h"

class config_manager
{
private:
    config_manager();
    ~config_manager();

public:
    static config_manager* instance(void);
    int set_default(void);

// =============================================================
// server url
    string& get_auth_url(void);
    int set_auth_url(string &url);

    string& get_token_url(void);
    int set_token_url(string &url);

private:
    static config_manager *_singleton_instance;

    // server url
    string _auth_url;
    string _token_url;

    // manual option
    u32 _field_test;
    u32 _multi_stream;
};

#endif
