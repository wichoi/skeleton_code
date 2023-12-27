#ifndef __EVENT_DATA_QUEUE_H__
#define __EVENT_DATA_QUEUE_H__

#include "common.h"
#include "timer.h"

class ev_data : public CFSPRefObject
{};

class data_string : public ev_data
{
public:
    string str_val;
};

class data_int : public ev_data
{
public:
    int value;
};

class data_timer : public ev_data
{
public:
    u32 id;
    u32 interval; // ms
    timer_listener *p_timer;
};

#endif
