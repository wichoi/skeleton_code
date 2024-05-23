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
#include "window/ui_wnd.h"

static void ui_main(void *arg1, void *arg2, void *arg3);
K_THREAD_DEFINE(ui_task, 4096, ui_main, NULL, NULL, NULL, CONFIG_MAIN_THREAD_PRIORITY, 0, 0);
LOG_MODULE_REGISTER(ui, CONFIG_DISPLAY_LOG_LEVEL);

#define Q_INACTIVE              0
#define Q_ACTIVE                1
#define UI_Q_CNT                0x0F

typedef struct ui_event_tag
{
    int event;
    int param;
    char data[32];
    int len;
} ui_ev_t;

static ui_ev_t _ev_q[UI_Q_CNT + 1] = {0,};
static uint8_t _ev_head = 0;
static uint8_t _ev_tail = 0;
static uint8_t _is_active = Q_INACTIVE;

static int ui_init(void)
{
    LOG_INF("%s", __func__);

    memset(_ev_q, 0x0, sizeof(_ev_q));
    _ev_head = 0;
    _ev_tail = 0;
    _is_active = Q_ACTIVE;

    return 0;
}

static int ui_deinit(void)
{
    LOG_INF("%s", __func__);

    memset(_ev_q, 0x0, sizeof(_ev_q));
    _ev_head = 0;
    _ev_tail = 0;
    _is_active = Q_INACTIVE;

    return 0;
}

int ui_event_put(int event, int param, char *data, int len)
{
    int ret = 0;
    //LOG_DBG("%s", __func__);

    if(_is_active == Q_ACTIVE)
    {
        if(((_ev_head + 1) & UI_Q_CNT)  == _ev_tail)
        {
            ret = 1;
            LOG_ERR("error ui queue full");
            return ret;
        }

        _ev_head++;
        _ev_head &= UI_Q_CNT;

        memset((char*)&_ev_q[_ev_tail], 0, sizeof(ui_ev_t));
        _ev_q[_ev_tail].event = event;
        _ev_q[_ev_tail].param = param;
        if(len <= sizeof(_ev_q[_ev_tail].data))
        {
            memcpy((char*)&_ev_q[_ev_tail].data, data, len);
        }
        _ev_q[_ev_tail].len = len;
        //LOG_DBG("put_event ev[%u]", _ev_q[_ev_tail].event);
    }
    else
    {
        ret = 1;
        LOG_ERR("ui queue inActive");
    }

    return ret;
}

static int ui_event_get(ui_ev_t *ev)
{
    int ret = 1;
    //LOG_DBG("%s", __func__);

    if(_ev_head != _ev_tail)
    {
        memcpy((char*)ev, (char*)&_ev_q[_ev_tail], sizeof(ui_ev_t));
        _ev_tail++;
        _ev_tail &= UI_Q_CNT;
        ret = 0;
        //LOG_DBG("get_event [%d]", ev->event);
    }

    return ret;
}

static int ui_hello(ui_ev_t *ev)
{
    LOG_INF("%s param[%d] data[%s]", __func__, ev->param, ev->data);
    return 0;
}

static int ui_test(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    return 0;
}

static int ui_event_proc(void)
{
    ui_ev_t ev = {0,};
    if(ui_event_get(&ev) == 1)
    {
        return 1;
    }

    switch(ev.event)
    {
    case UI_EVENT_WND_CREATE:
    case UI_EVENT_WND_CREATE_EX:
    case UI_EVENT_WND_DESTORY:
    case UI_EVENT_WND_DESTORY_ALL:
    case UI_EVENT_WND_SUSPEND:
    case UI_EVENT_WND_RESUME:
    case UI_EVENT_WND_PAINT:
    case UI_EVENT_WND_EVENT:
    case UI_EVENT_WND_LIST:
        ui_wnd_proc(ev.event, ev.param, ev.data, ev.len);
        break;
    case UI_EVENT_HELLO:
        ui_hello(&ev);
        break;
    case UI_EVENT_TEST:
        ui_test(&ev);
        break;
    default:
        break;
    }

    return 0;
}

void ui_main(void *arg1, void *arg2, void *arg3)
{
    const struct device *display_dev;
    LOG_INF("%s start ...", __func__);

    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev))
    {
        LOG_ERR("%s device not ready !!!", __func__);
        return;
    }

    ui_init();
    ui_wnd_init();
    ui_event_put(UI_EVENT_WND_CREATE, UI_WND_POWER_ON, NULL, 0);

    lv_task_handler();
    display_blanking_off(display_dev);

    while (1)
    {
        ui_event_proc();
        lv_task_handler();
        k_sleep(K_MSEC(10));
    }

    ui_deinit();
    ui_wnd_deinit();
    LOG_INF("%s end ...", __func__);
}

//==================================================================================================
// SHELL
//==================================================================================================

static int cmd_ui_wnd_idle(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_CREATE, UI_WND_IDLE, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_charging(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_CREATE, UI_WND_CHARGING, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_power_on(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_CREATE, UI_WND_POWER_ON, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_power_off(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_CREATE_EX, UI_WND_POWER_OFF, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_clear(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_DESTORY, 0, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_clear_all(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_DESTORY_ALL, 0, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_paint(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_PAINT, 0, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_event(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_EVENT, 0, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_list(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_LIST, 0, NULL, 0);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(ui_wnd,
    SHELL_CMD(idle,     NULL,   "cmd_ui_wnd_idle",      cmd_ui_wnd_idle),
    SHELL_CMD(charging, NULL,   "cmd_ui_wnd_charging",  cmd_ui_wnd_charging),
    SHELL_CMD(power_on, NULL,   "cmd_ui_wnd_power_on",  cmd_ui_wnd_power_on),
    SHELL_CMD(power_off, NULL,  "cmd_ui_wnd_power_off", cmd_ui_wnd_power_off),
    SHELL_CMD(clear,    NULL,   "cmd_ui_wnd_clear",     cmd_ui_wnd_clear),
    SHELL_CMD(clear_all,NULL,   "cmd_ui_wnd_clear_all", cmd_ui_wnd_clear_all),
    SHELL_CMD(paint,    NULL,   "cmd_ui_wnd_paint",     cmd_ui_wnd_paint),
    SHELL_CMD(event,    NULL,   "cmd_ui_wnd_event",     cmd_ui_wnd_event),
    SHELL_CMD(list,     NULL,   "cmd_ui_wnd_list",      cmd_ui_wnd_list),
    SHELL_SUBCMD_SET_END
);

static int cmd_ui_hello(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_HELLO, 123, "hello world !!!", strlen("hello world !!!"));
    return 0;
}

static int cmd_ui_test(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_TEST, 0, NULL, 0);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(ui_sub,
    SHELL_CMD(wnd,      &ui_wnd,    "cmd_ui_wnd",       NULL),
    SHELL_CMD(hello,    NULL,       "cmd_ui_hello",     cmd_ui_hello),
    SHELL_CMD(test,     NULL,       "cmd_ui_test",      cmd_ui_test),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(ui, &ui_sub, "ui cli", NULL);

