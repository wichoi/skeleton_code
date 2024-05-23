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

#include "ui_wnd.h"

LOG_MODULE_REGISTER(ui_idle, CONFIG_DISPLAY_LOG_LEVEL);

static ui_wnd_handle _wnd_handle = {0,};
static lv_obj_t *_title_label = NULL;

static int on_create(void)
{
    LOG_INF("%s", __func__);
    _title_label = lv_label_create(lv_scr_act());
    lv_label_set_text(_title_label, _wnd_handle.name);
    lv_obj_align(_title_label, LV_ALIGN_TOP_MID, 0, 0);
    return 0;
}

static int on_destory(void)
{
    LOG_INF("%s", __func__);
    lv_obj_del(_title_label);
    //lv_obj_clean(lv_scr_act());
    return 0;
}

static int on_suspend(void)
{
    LOG_INF("%s", __func__);
    return 0;
}

static int on_resume(void)
{
    LOG_INF("%s", __func__);
    return 0;
}

static int on_paint(void)
{
    LOG_INF("%s", __func__);
    lv_task_handler();
    return 0;
}

static int on_event(void)
{
    LOG_INF("%s", __func__);
    return 0;
}

ui_wnd_handle* get_handle_wnd_idle(void)
{
    _wnd_handle.name = "idle window";
    _wnd_handle.create = on_create;
    _wnd_handle.destory = on_destory;
    _wnd_handle.suspend = on_suspend;
    _wnd_handle.resume = on_resume;
    _wnd_handle.paint = on_paint;
    _wnd_handle.event = on_event;
    return &_wnd_handle;
}

