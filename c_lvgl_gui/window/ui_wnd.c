#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>
#include <lvgl.h>
#include <lvgl_input_device.h>
#include <stdio.h>
#include <string.h>

#include "ui_task.h"
#include "ui_wnd.h"

LOG_MODULE_REGISTER(ui_wnd, CONFIG_DISPLAY_LOG_LEVEL);

#define WND_ITEM_MAX    32
#define WND_STACK_MAX   16

typedef struct ui_wnd_item_tag
{
    int id;
    ui_wnd_handle *handle;
} ui_wnd_item;

typedef struct ui_wnd_list_tag
{
    ui_wnd_item wnd_list[WND_ITEM_MAX];
    int wnd_cnt;
} ui_wnd_list;

typedef struct ui_wnd_stack_tag
{
    ui_wnd_item wnd_stack[WND_STACK_MAX];
    int cnt;
} ui_wnd_stack;

ui_wnd_list _list_handle = {0,};
ui_wnd_stack _wnd_stack = {0,};

static int ui_wnd_add(int id, ui_wnd_handle *handle)
{
    int cnt = _list_handle.wnd_cnt;
    if(cnt < WND_ITEM_MAX)
    {
        _list_handle.wnd_list[cnt].id = id;
        _list_handle.wnd_list[cnt].handle = handle;
        _list_handle.wnd_cnt++;
    }
    return 0;
}

static int ui_wnd_find(int id)
{
    int cnt = _list_handle.wnd_cnt;
    for(int i = 0; i < cnt; i++)
    {
        if(_list_handle.wnd_list[i].id == id)
        {
            return i;
        }
    }

    return -1;
}

static int ui_wnd_info(void)
{
    int i = 0;
    LOG_INF("%s", __func__);

    LOG_INF("window list cnt[%d]", _list_handle.wnd_cnt);
    for(i = 0; i < _list_handle.wnd_cnt; i++)
    {
        LOG_INF("id[%02d] name[%s]",
            _list_handle.wnd_list[i].id,
            _list_handle.wnd_list[i].handle->name);
    }

    LOG_INF("window stack cnt[%d]", _wnd_stack.cnt);
    for(i = 0; i < _wnd_stack.cnt; i++)
    {
        LOG_INF("id[%02d] name[%s]",
            _wnd_stack.wnd_stack[i].id,
            _wnd_stack.wnd_stack[i].handle->name);
    }

    return 0;
}

int ui_wnd_init(void)
{
    LOG_INF("%s", __func__);
    memset((char*)&_list_handle, 0, sizeof(_list_handle));
    memset((char*)&_wnd_stack, 0, sizeof(_wnd_stack));

    ui_wnd_add(UI_WND_IDLE, get_handle_wnd_idle());
    ui_wnd_add(UI_WND_CHARGING, get_handle_wnd_charging());
    ui_wnd_add(UI_WND_POWER_ON, get_handle_wnd_power_on());
    ui_wnd_add(UI_WND_POWER_OFF, get_handle_wnd_power_off());

    return 0;
}

int ui_wnd_deinit(void)
{
    LOG_INF("%s", __func__);
    return 0;
}

int ui_wnd_proc(int event, int param, char *data, int len)
{
    //LOG_INF("%s", __func__);
    switch(event)
    {
    case UI_EVENT_WND_CREATE:
        {
            int index = ui_wnd_find(param);
            if(index != -1 && _wnd_stack.cnt < WND_STACK_MAX)
            {
                if(_wnd_stack.cnt > 0)
                {
                    _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle->suspend();
                    _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle->destory();
                }

                _wnd_stack.wnd_stack[_wnd_stack.cnt].id = _list_handle.wnd_list[index].id;
                _wnd_stack.wnd_stack[_wnd_stack.cnt].handle = _list_handle.wnd_list[index].handle;
                _wnd_stack.wnd_stack[_wnd_stack.cnt].handle->create();
                _wnd_stack.wnd_stack[_wnd_stack.cnt].handle->paint();
                _wnd_stack.cnt++;
            }
            else
            {
                LOG_ERR("%s wnd create error !!!", __func__);
            }
        }
        break;
    case UI_EVENT_WND_CREATE_EX:
        {
            if(_wnd_stack.cnt > 0)
            {
                _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle->destory();
                _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].id = 0;
                _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle = NULL;
                _wnd_stack.cnt--;
            }

            int index = ui_wnd_find(param);
            if(index != -1 && _wnd_stack.cnt < WND_STACK_MAX)
            {
                _wnd_stack.wnd_stack[_wnd_stack.cnt].id = _list_handle.wnd_list[index].id;
                _wnd_stack.wnd_stack[_wnd_stack.cnt].handle = _list_handle.wnd_list[index].handle;
                _wnd_stack.wnd_stack[_wnd_stack.cnt].handle->create();
                _wnd_stack.wnd_stack[_wnd_stack.cnt].handle->paint();
                _wnd_stack.cnt++;
            }
            else
            {
                LOG_ERR("%s wnd create error !!!", __func__);
            }
        }
        break;
    case UI_EVENT_WND_DESTORY:
        if(_wnd_stack.cnt > 0)
        {
            _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle->destory();
            _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].id = 0;
            _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle = NULL;
            _wnd_stack.cnt--;

            if(_wnd_stack.cnt > 0)
            {
                _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle->create();
                _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle->resume();
                _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle->paint();
            }
            else
            {
                _wnd_stack.wnd_stack[0].id = UI_WND_IDLE;
                _wnd_stack.wnd_stack[0].handle = get_handle_wnd_idle();
                _wnd_stack.wnd_stack[0].handle->create();
                _wnd_stack.wnd_stack[0].handle->paint();
                _wnd_stack.cnt = 1;
            }
        }
        break;
    case UI_EVENT_WND_DESTORY_ALL:
        if(_wnd_stack.cnt > 0)
        {
            _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle->destory();
            _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].id = 0;
            _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle = NULL;
            _wnd_stack.cnt--;
        }

        memset((char*)&_wnd_stack, 0, sizeof(_wnd_stack));
        _wnd_stack.wnd_stack[0].id = UI_WND_IDLE;
        _wnd_stack.wnd_stack[0].handle = get_handle_wnd_idle();
        _wnd_stack.wnd_stack[0].handle->create();
        _wnd_stack.wnd_stack[0].handle->paint();
        _wnd_stack.cnt = 1;
        break;
    case UI_EVENT_WND_SUSPEND:
        if(_wnd_stack.cnt > 0)
        {
            _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle->suspend();
            _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle->destory();
        }
        break;
    case UI_EVENT_WND_RESUME:
        if(_wnd_stack.cnt > 0)
        {
            _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle->create();
            _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle->resume();
            _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle->paint();
        }
        break;
    case UI_EVENT_WND_PAINT:
        if(_wnd_stack.cnt > 0)
        {
            _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle->paint();
        }
        break;
    case UI_EVENT_WND_EVENT:
        if(_wnd_stack.cnt > 0)
        {
            _wnd_stack.wnd_stack[_wnd_stack.cnt - 1].handle->event();
        }
        break;
    case UI_EVENT_WND_LIST:
        ui_wnd_info();
        break;
    default:
        break;
    }
    return 0;
}

