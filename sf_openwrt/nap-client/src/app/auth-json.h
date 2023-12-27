#ifndef __AUTH_JSON_MANAGER_H__
#define __AUTH_JSON_MANAGER_H__

#include "common.h"
#include "json-manager.h"

class auth_json
{
public:
    auth_json();
    ~auth_json();
    int init(void);
    int deinit(void);

    int token_create(string &request, string &iss, string &sub, string &aud, string &jti, string &exp);
    int token_parse(string &response, string &token, int &expired, string &type, string &error, string &description, string &message);
};

#endif
