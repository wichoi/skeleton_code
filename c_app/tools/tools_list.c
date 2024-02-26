#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include "common.h"
#include "log_service.h"
#include "tools_list.h"
#include "utils.h"

static int list_push_back(list_t *this, char *data, u32 len)
{
    int ret_val = RET_OK;
    list_item_t *new_item = (list_item_t*)malloc(sizeof(list_item_t));
    new_item->data = malloc(len);
    memcpy(new_item->data, data, len);
    new_item->len = len;
    new_item->tail = NULL;

    if(this->cnt == 0)
    {
        this->item = new_item;
    }
    else
    {
        int i = 0;
        list_item_t *p_item = this->item;
        for(i = 0; i < this->cnt; i++)
        {
            if(p_item->tail != NULL)
            {
                p_item = p_item->tail;
            }
        }
        p_item->tail = new_item;
    }

    this->cnt++;
    return ret_val;
}

static list_item_t* list_get(list_t *this, u32 index)
{
    list_item_t* ret_val = NULL;

    if(this->cnt == 0 || this->cnt < index)
    {
        ret_val = NULL;
    }
    else
    {
        int i = 0;
        list_item_t *p_item = this->item;
        for(i = 0; i < index; i++)
        {
            if(p_item->tail != NULL)
            {
                p_item = p_item->tail;
            }
        }
        ret_val = p_item;
    }

    return ret_val;
}

static int list_erase(list_t *this, u32 index)
{
    int ret_val = RET_OK;

    if(index >= this->cnt)
    {
        ret_val = RET_ERR;
    }
    else
    {
        int i = 0;
        list_item_t *p_item = this->item;
        list_item_t *prev_item = NULL;
        for(i = 0; i < index; i++)
        {
            prev_item = p_item;
            if(p_item->tail != NULL)
            {
                p_item = p_item->tail;
            }
        }

        if(prev_item == NULL)
        {
            this->item = p_item->tail;
        }
        else
        {
            prev_item->tail = p_item->tail;
        }

        p_item->tail = NULL;
        p_item->len = 0;
        free(p_item->data);
        free(p_item);
        this->cnt--;
    }

    return ret_val;
}

static int list_clear(list_t *this)
{
    int ret_val = RET_OK;
    while(this->cnt)
    {
        list_erase(this, 0);
    }
    return ret_val;
}

static int list_free(list_t *this)
{
    int ret_val = RET_OK;
    list_clear(this);
    this->push_back = NULL;
    this->get = NULL;
    this->erase = NULL;
    this->clear = NULL;
    this->free = NULL;
    this->item = NULL;
    this->cnt = 0;
    free(this);
    this = NULL;
    return ret_val;
}

list_t* list_new(void)
{
    list_t *this = (list_t*)malloc(sizeof(list_t));
    this->push_back = list_push_back;
    this->get = list_get;
    this->erase = list_erase;
    this->clear = list_clear;
    this->free = list_free;
    this->item = NULL;
    this->cnt = 0;
    return this;
}

