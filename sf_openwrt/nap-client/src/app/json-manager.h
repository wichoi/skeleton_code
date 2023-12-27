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
        TYPE_NUM =      2
    } json_type_e;

public:
    string key;
    string value_str;
    int value_int;
    double value_num;
    json_type_e type;

    json_param(string key, string value)
    {
        this->key = key;
        value_str = value;
        value_int = 0;
        value_num = 0;
        this->type = TYPE_STR;
    };

    json_param(string key, int value)
    {
        this->key = key;
        value_str = "";
        value_int = value;
        value_num = 0;
        this->type = TYPE_INT;
    };

    json_param(string key, double value)
    {
        this->key = key;
        value_str = "";
        value_int = 0;
        value_num = value;
        this->type = TYPE_NUM;
    };

    void set_param(string key, string value)
    {
        this->key = key;
        value_str = value;
        value_int = 0;
        value_num = 0;
        this->type = TYPE_STR;
    };

    void set_param(string key, int value)
    {
        this->key = key;
        value_str = "";
        value_int = value;
        value_num = 0;
        this->type = TYPE_INT;
    };

    void set_param(string key, double value)
    {
        this->key = key;
        value_str = "";
        value_int = 0;
        value_num = value;
        this->type = TYPE_NUM;
    };

    void set_value(string value)
    {
        value_str = value;
        value_int = 0;
        value_num = 0;
        this->type = TYPE_STR;
    };

    void set_value(int value)
    {
        value_str = "";
        value_int = value;
        value_num = 0;
        this->type = TYPE_INT;
    };

    void set_value(double value)
    {
        value_str = "";
        value_int = 0;
        value_num = value;
        this->type = TYPE_NUM;
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

#if 0
class json_payload
{
public:
    string key;
    string value_str;
    int value_int;
    double value_num;
    const type_info *value_type;

    template <class T>
    json_payload(string key, T value)
    {
        this->key = key;
        this->value_type = &typeid(value);
        if(typeid(value) == typeid(string) ||
            typeid(value) == typeid(char*))
        {
            value_str = value;
        }
        else if(typeid(value) == typeid(int) ||
                typeid(value) == typeid(unsigned int) ||
                typeid(value) == typeid(short) ||
                typeid(value) == typeid(unsigned short) ||
                typeid(value) == typeid(char) ||
                typeid(value) == typeid(unsigned char))
        {
            value_int = value;
        }
        else if(typeid(value) == typeid(double) ||
                typeid(value) == typeid(float))
        {
            value_num = value;
        }
    };

    template <class T>
    void set_payload(string key, T value)
    {
        this->key = key;
        this->value_type = &typeid(value);
        if(typeid(value) == typeid(string) ||
            typeid(value) == typeid(char*))
        {
            value_str = value;
        }
        else if(typeid(value) == typeid(int) ||
                typeid(value) == typeid(unsigned int) ||
                typeid(value) == typeid(short) ||
                typeid(value) == typeid(unsigned short) ||
                typeid(value) == typeid(char) ||
                typeid(value) == typeid(unsigned char))
        {
            value_int = value;
        }
        else if(typeid(value) == typeid(double) ||
                typeid(value) == typeid(float))
        {
            value_num = value;
        }
    };

    template <class T>
    void set_value(T value)
    {
        this->value_type = &typeid(value);
        if(typeid(value) == typeid(string) ||
            typeid(value) == typeid(char*))
        {
            value_str = value;
        }
        else if(typeid(value) == typeid(int) ||
                typeid(value) == typeid(unsigned int) ||
                typeid(value) == typeid(short) ||
                typeid(value) == typeid(unsigned short) ||
                typeid(value) == typeid(char) ||
                typeid(value) == typeid(unsigned char))
        {
            value_int = value;
        }
        else if(typeid(value) == typeid(double) ||
                typeid(value) == typeid(float))
        {
            value_num = value;
        }
    };

    template <class T>
    T get_value(void)
    {
        if(typeid(value_type) == typeid(string) ||
            typeid(value_type) == typeid(char*))
        {
            return value_str;
        }
        else if(typeid(value_type) == typeid(int) ||
                typeid(value_type) == typeid(unsigned int) ||
                typeid(value_type) == typeid(short) ||
                typeid(value_type) == typeid(unsigned short) ||
                typeid(value_type) == typeid(char) ||
                typeid(value_type) == typeid(unsigned char))
        {
            return value_int;
        }
        else if(typeid(value_type) == typeid(double) ||
                typeid(value_type) == typeid(float))
        {
            return value_num;
        }

        return 0;
    };

    const type_info* get_value_type(void)
    {
        return value_type;
    };
};
#endif

#endif
