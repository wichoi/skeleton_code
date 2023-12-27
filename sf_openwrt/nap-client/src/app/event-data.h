#ifndef __EVENT_DATA_QUEUE_H__
#define __EVENT_DATA_QUEUE_H__

#include "cmd-service.grpc.pb.h"
#include "msg-service.grpc.pb.h"

#include "common.h"
#include "timer.h"

class dat_c {};

class dat_string : public dat_c
{
public:
    string data;
};

class dat_int : public dat_c
{
public:
    int value;
};

class dat_timer : public dat_c
{
public:
    u32 id;
    u32 interval; // ms
    timer_listener *p_timer;
};

class dat_atcmd : public dat_c
{
public:
    typedef enum
    {
        TYPE_STR = 0,
        TYPE_INT = 1,
        TYPE_NUM = 2,
    } at_type_e;

    class at_param_c
    {
    public:
        at_type_e type;
        string value_str;
        int value_int;
        double value_num;

        at_param_c() {};
        at_param_c(string str)
        {
            type = TYPE_STR;
            value_str = str;
            value_int = 0;
            value_num = 0;
        };
        at_param_c(int value)
        {
            type = TYPE_INT;
            value_str = "";
            value_int = value;
            value_num = 0;
        };
        at_param_c(double value)
        {
            type = TYPE_NUM;
            value_str = "";
            value_int = 0;
            value_num = value;
        };
    };

public:
    string atcmd;
    list<at_param_c> param;
    string stream_data;
};

#endif
