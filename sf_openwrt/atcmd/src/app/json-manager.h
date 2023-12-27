#ifndef __JSON_MANAGER_H__
#define __JSON_MANAGER_H__

#include <string>
#include <list>

#include "common.h"

class json_param
{
public:
    typedef enum
    {
        TYPE_STR =      0,
        TYPE_INT =      1,
        TYPE_FLO =      2
    } json_type_e;

public:
    string key;
    string value_str;
    int value_int;
    double value_fl;
    json_type_e type;

    json_param(string key, string value)
    {
        this->key = key;
        value_str = value;
        value_int = 0;
        value_fl = 0;
        this->type = TYPE_STR;
    };

    json_param(string key, int value)
    {
        this->key = key;
        value_str = "";
        value_int = value;
        value_fl = 0;
        this->type = TYPE_INT;
    };

    json_param(string key, double value)
    {
        this->key = key;
        value_str = "";
        value_int = 0;
        value_fl = value;
        this->type = TYPE_FLO;
    };
};

class json_manager
{
public:
    json_manager();
    ~json_manager();
    int init(void);
    int deinit(void);

    int config_create(list<json_param> &obj_list, string &json_body);
    int config_parse(string &src_data, list<json_param> &obj_list);
};

#endif
