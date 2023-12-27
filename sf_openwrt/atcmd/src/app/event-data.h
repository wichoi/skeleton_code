#ifndef __EVENT_DATA_H__
#define __EVENT_DATA_H__

#include <memory>
#include <list>

#include "common.h"
#include "timer.h"

class dat_c {};

class dat_hello : public dat_c
{
public:
    string data;
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
        TYPE_FLO = 2,
    } at_type_e;

    class at_param_c
    {
    public:
        at_type_e type;
        string value_str;
        int value_int;
        double value_fl;

        at_param_c() {};
        at_param_c(string str)
        {
            type = TYPE_STR;
            value_str = str;
            value_int = 0;
            value_fl = 0;
        };
        at_param_c(int value)
        {
            type = TYPE_INT;
            value_str = "";
            value_int = value;
            value_fl = 0;
        };
        at_param_c(double value)
        {
            type = TYPE_INT;
            value_str = "";
            value_int = 0;
            value_fl = value;
        };
    };

public:
    string atcmd;
    list<at_param_c> param;
    string stream_data;
    int pid;
};

#endif
