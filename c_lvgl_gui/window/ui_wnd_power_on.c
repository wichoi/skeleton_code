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

LOG_MODULE_REGISTER(ui_power_on, CONFIG_DISPLAY_LOG_LEVEL);

LV_FONT_DECLARE(kr_font_nanum_gothic_bold_24);

static void timer_callback(struct k_timer *timer_id);
K_TIMER_DEFINE(power_on_timer,timer_callback,NULL);

static ui_wnd_handle _wnd_handle = {0,};
static lv_obj_t *_title_label = NULL;

static char count_str[11] = {0};
static uint32_t count;
static lv_obj_t *count_label;
static lv_obj_t *hello_world_label;

static void timer_callback(struct k_timer *timer_id)
{
    //LOG_INF("%s", __func__);
    ui_event_put(UI_EVENT_WND_PAINT, 0, NULL, 0);
}

static int on_create(void)
{
    LOG_INF("%s", __func__);
    _title_label = lv_label_create(lv_scr_act());
    lv_label_set_text(_title_label, _wnd_handle.name);
    lv_obj_align(_title_label, LV_ALIGN_TOP_MID, 0, 0);


    hello_world_label = lv_label_create(lv_scr_act());
#if 1
    static const lv_font_t * font_medium;
    font_medium = &kr_font_nanum_gothic_bold_24;
    static lv_style_t style_medium;
    lv_style_init(&style_medium);
    //lv_style_reset(&style_medium);
    lv_style_set_text_font(&style_medium, font_medium);
    //lv_style_set_text_color(&style_medium, lv_color_hex(0xffffff));
    lv_obj_add_style(hello_world_label, &style_medium, 0);
#endif
    lv_label_set_text(hello_world_label, "hello world! 가나다 !!!");
    lv_obj_align(hello_world_label, LV_ALIGN_CENTER, 0, 0);

    count_label = lv_label_create(lv_scr_act());
    lv_obj_align(count_label, LV_ALIGN_BOTTOM_MID, 0, 0);


    count = 0;

    k_timer_init(&power_on_timer, timer_callback, NULL);
    k_timer_start(&power_on_timer, K_MSEC(1000), K_MSEC(1000));

    return 0;
}

static int on_destory(void)
{
    LOG_INF("%s", __func__);
    count = 0;
    k_timer_stop(&power_on_timer);

    lv_obj_del(_title_label);
    lv_obj_del(hello_world_label);
    lv_obj_del(count_label);
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
    //LOG_INF("%s", __func__);
    sprintf(count_str, "%d", count);
    lv_label_set_text(count_label, count_str);
    lv_task_handler();
    ++count;
    return 0;
}

static int on_event(void)
{
    LOG_INF("%s", __func__);
    return 0;
}

ui_wnd_handle* get_handle_wnd_power_on(void)
{
    _wnd_handle.name = "power on window";
    _wnd_handle.create = on_create;
    _wnd_handle.destory = on_destory;
    _wnd_handle.suspend = on_suspend;
    _wnd_handle.resume = on_resume;
    _wnd_handle.paint = on_paint;
    _wnd_handle.event = on_event;
    return &_wnd_handle;
}

