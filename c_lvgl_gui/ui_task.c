#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>

#include <lvgl.h>
#include <lvgl_input_device.h>

#include "ui_task.h"
#include "window/ui_wnd.h"

static void ui_main(void *arg1, void *arg2, void *arg3);
K_THREAD_DEFINE(ui_task, 4096, ui_main, NULL, NULL, NULL, CONFIG_MAIN_THREAD_PRIORITY, 0, 0);
LOG_MODULE_REGISTER(ui, CONFIG_DISPLAY_LOG_LEVEL);

#define Q_INACTIVE              0
#define Q_ACTIVE                1
#define UI_Q_CNT                0x0F

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
        ui_wnd_proc(&ev);
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

    // set default style
    static lv_style_t def_style;
    lv_style_init(&def_style);
    LV_FONT_DECLARE(kr_font_ChosunKg_24);
    lv_style_set_text_font(&def_style, &kr_font_ChosunKg_24);
    lv_style_set_bg_color(&def_style, lv_color_black());
    //lv_style_set_opa(&def_style, LV_OPA_COVER);
    lv_obj_add_style(lv_scr_act(), &def_style, 0);
    //lv_obj_add_style(lv_layer_top(), &def_style, 0);
    //lv_obj_add_style(lv_layer_sys(), &def_style, 0);

    lv_task_handler();
    display_blanking_off(display_dev);
    ui_event_put(UI_EVENT_WND_CREATE, UI_WND_BOOTING, NULL, 0);

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
    ui_event_put(UI_EVENT_WND_CREATE_EX, UI_WND_IDLE, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_booting(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_CREATE_EX, UI_WND_BOOTING, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_error(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_CREATE_EX, UI_WND_ERROR, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_credit_card(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_CREATE_EX, UI_WND_CREDIT_CARD, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_charging(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_CREATE_EX, UI_WND_CHARGING, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_finishing(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_CREATE_EX, UI_WND_FINISHING, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_complete(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_CREATE_EX, UI_WND_COMPLETE, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_stop(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_CREATE_EX, UI_WND_STOP, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_auth(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_CREATE_EX, UI_WND_AUTH, NULL, 0);
    return 0;
}

static int cmd_ui_wnd_cable(const struct shell *sh, size_t argc, char **argv, void *data)
{
    shell_fprintf(sh, SHELL_NORMAL, "%s\n", __func__);
    ui_event_put(UI_EVENT_WND_CREATE_EX, UI_WND_CABLE, NULL, 0);
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
    SHELL_CMD(boot,     NULL,   "cmd_ui_wnd_booting",   cmd_ui_wnd_booting),
    SHELL_CMD(err,      NULL,   "cmd_ui_wnd_error",     cmd_ui_wnd_error),
    SHELL_CMD(credit,   NULL,   "cmd_ui_wnd_credit_card", cmd_ui_wnd_credit_card),
    SHELL_CMD(charging, NULL,   "cmd_ui_wnd_charging",  cmd_ui_wnd_charging),
    SHELL_CMD(finishing,NULL,   "cmd_ui_wnd_finishing", cmd_ui_wnd_finishing),
    SHELL_CMD(complete ,NULL,   "cmd_ui_wnd_complete",  cmd_ui_wnd_complete),
    SHELL_CMD(stop,     NULL,   "cmd_ui_wnd_stop",      cmd_ui_wnd_stop),
    SHELL_CMD(auth,     NULL,   "cmd_ui_wnd_auth",      cmd_ui_wnd_auth),
    SHELL_CMD(cable,    NULL,   "cmd_ui_wnd_cable",     cmd_ui_wnd_cable),
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

