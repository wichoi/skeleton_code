#include <stdio.h>
#include <string.h>

#include <lvgl.h>
#include <lvgl_input_device.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ui_task.h"
#include "ui_wnd.h"

LOG_MODULE_REGISTER(ui_cable, CONFIG_DISPLAY_LOG_LEVEL);
LV_IMG_DECLARE(img_cable);

static void on_timer(struct k_timer *timer_id);
K_TIMER_DEFINE(wnd_cable_timer, on_timer,NULL);

#define UI_CABLE_ICON_X         140
#define UI_CABLE_ICON_Y         70

#define UI_CABLE_LABEL_X        120
#define UI_CABLE_LABEL_Y        230

static ui_wnd_handle _wnd_handle = {0,};

static lv_obj_t *_icon = NULL;
static lv_obj_t *_label = NULL;

static uint32_t _cable_cnt = 0;

static void on_timer(struct k_timer *timer_id)
{
    //LOG_DBG("%s", __func__);
    if(_cable_cnt++> 60)
    {
        ui_event_put(UI_EVENT_WND_CREATE_EX, UI_WND_IDLE, NULL, 0);
    }
}

static int on_create(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    ui_wnd_indicator_create(ev);

    _icon = lv_img_create(lv_scr_act());
    lv_img_set_src(_icon, &img_cable);
    //lv_obj_set_pos(_icon, UI_CABLE_ICON_X, UI_CABLE_ICON_Y);
    lv_obj_align(_icon, LV_ALIGN_TOP_MID, 0, UI_CABLE_ICON_Y);
    lv_obj_set_size(_icon, img_cable.header.w, img_cable.header.h);

    _label = lv_label_create(lv_scr_act());
    lv_label_set_text(_label, "차량에 커넥터를 연결해 주세요.");
    //lv_obj_set_pos(_label, UI_CABLE_LABEL_X, UI_CABLE_LABEL_Y);
    lv_obj_align(_label, LV_ALIGN_TOP_MID, 0, UI_CABLE_LABEL_Y);

    _cable_cnt = 0;
    k_timer_init(&wnd_cable_timer, on_timer, NULL);
    //k_timer_start(&wnd_cable_timer, K_MSEC(1000), K_MSEC(1000));
    return 0;
}

static int on_destory(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    k_timer_stop(&wnd_cable_timer);

    lv_obj_del(_icon);
    lv_obj_del(_label);

    ui_wnd_indicator_destory(ev);
    return 0;
}

static int on_suspend(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    return 0;
}

static int on_resume(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    return 0;
}

static int on_paint(ui_ev_t *ev)
{
    //LOG_DBG("%s", __func__);
    lv_obj_invalidate(lv_scr_act());
    lv_task_handler();
    return 0;
}

static int on_event(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    return 0;
}

ui_wnd_handle* get_handle_wnd_cable(void)
{
    _wnd_handle.name = "cable window";
    _wnd_handle.create = on_create;
    _wnd_handle.destory = on_destory;
    _wnd_handle.suspend = on_suspend;
    _wnd_handle.resume = on_resume;
    _wnd_handle.paint = on_paint;
    _wnd_handle.event = on_event;
    return &_wnd_handle;
}
