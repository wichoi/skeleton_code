#include <stdio.h>
#include <string.h>
#include <time.h>

#include <lvgl.h>
#include <lvgl_input_device.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ui_task.h"
#include "ui_wnd.h"

LOG_MODULE_REGISTER(ui_charging, CONFIG_DISPLAY_LOG_LEVEL);
LV_IMG_DECLARE(img_charging);

static void on_timer(struct k_timer *timer_id);
K_TIMER_DEFINE(wnd_charging_timer, on_timer,NULL);

#define UI_CHARGING_LABEL_X         55
#define UI_CHARGING_LABEL_Y         80

#define UI_CHARGING_ICON_X          35
#define UI_CHARGING_ICON_Y          105

#define UI_CHARGING_PROC_X          45
#define UI_CHARGING_PROC_Y          140
#define UI_CHARGING_PROC_W          28
#define UI_CHARGING_PROC_H          37

#define UI_CHARGING_TIME_X          190
#define UI_CHARGING_TIME_Y          80

#define UI_CHARGING_METER_X         190
#define UI_CHARGING_METER_Y         120 // UI_CHARGING_TIME_Y + 40

#define UI_CHARGING_COST_X          190
#define UI_CHARGING_COST_Y          160 // UI_CHARGING_COST_Y + 40

#define UI_CHARGING_BOTTOM_X        40
#define UI_CHARGING_BOTTOM_Y        240

static ui_wnd_handle _wnd_handle = {0,};

static lv_obj_t *_label_charging = NULL;
static lv_obj_t *_img_charging = NULL;
static lv_obj_t *_rect_charging = NULL;

static lv_obj_t *_label_time = NULL;
static lv_obj_t *_label_meter = NULL;
static lv_obj_t *_label_cost = NULL;
static lv_obj_t *_label_bottom = NULL;

static uint32_t _charging_cnt = 0;

static void on_timer(struct k_timer *timer_id)
{
    //LOG_DBG("%s", __func__);
    _charging_cnt++;
    ui_event_put(UI_EVENT_WND_PAINT, 0, NULL, 0);
}

static int on_create(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    ui_wnd_indicator_create(ev);

    _label_charging = lv_label_create(lv_scr_act());
    lv_label_set_text(_label_charging, "충전중");
    lv_obj_set_pos(_label_charging, UI_CHARGING_LABEL_X, UI_CHARGING_LABEL_Y);

    _img_charging = lv_img_create(lv_scr_act());
    lv_img_set_src(_img_charging, &img_charging);
    lv_obj_set_pos(_img_charging, UI_CHARGING_ICON_X, UI_CHARGING_ICON_Y);
    lv_obj_set_size(_img_charging, img_charging.header.w, img_charging.header.h);

    _rect_charging = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(_rect_charging, UI_CHARGING_PROC_X, UI_CHARGING_PROC_Y);
    lv_obj_set_size(_rect_charging, UI_CHARGING_PROC_W, UI_CHARGING_PROC_H);
    lv_obj_set_style_bg_color(_rect_charging, lv_color_white(), LV_PART_MAIN);

    _label_time = lv_label_create(lv_scr_act());
    lv_label_set_text(_label_time, "충전시간");
    lv_obj_set_pos(_label_time, UI_CHARGING_TIME_X, UI_CHARGING_TIME_Y);

    _label_meter = lv_label_create(lv_scr_act());
    lv_label_set_text(_label_meter, "충전량");
    lv_obj_set_pos(_label_meter, UI_CHARGING_METER_X, UI_CHARGING_METER_Y);

    _label_cost = lv_label_create(lv_scr_act());
    lv_label_set_text(_label_cost, "충전금액");
    lv_obj_set_pos(_label_cost, UI_CHARGING_COST_X, UI_CHARGING_COST_Y);

    _label_bottom = lv_label_create(lv_scr_act());
    lv_label_set_text(_label_bottom, "충전 중단하려면 중단버튼을 누르세요.");
    lv_obj_set_pos(_label_bottom, UI_CHARGING_BOTTOM_X, UI_CHARGING_BOTTOM_Y);
    //lv_obj_align(_label_bottom, LV_ALIGN_TOP_MID, 0, UI_CHARGING_BOTTOM_Y);

    _charging_cnt = 0;

    k_timer_init(&wnd_charging_timer, on_timer, NULL);
    k_timer_start(&wnd_charging_timer, K_MSEC(1000), K_MSEC(1000));
    return 0;
}

static int on_destory(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    k_timer_stop(&wnd_charging_timer);

    lv_obj_del(_label_charging);
    lv_obj_del(_img_charging);
    lv_obj_del(_rect_charging);

    lv_obj_del(_label_time);
    lv_obj_del(_label_meter);
    lv_obj_del(_label_cost);
    lv_obj_del(_label_bottom);

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
    lv_obj_set_size(_rect_charging, UI_CHARGING_PROC_W*(_charging_cnt%4), UI_CHARGING_PROC_H);

    char str_buf[32] = {0,};
    char time_buf[16] = {0,};
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_buf, sizeof(time_buf), "%H:%M:%S", timeinfo);
    sprintf(str_buf, "충전시간  %s", time_buf);
    lv_label_set_text(_label_time, str_buf);

    sprintf(str_buf, "충전량     %d kWh", _charging_cnt);
    lv_label_set_text(_label_meter, str_buf);

    sprintf(str_buf, "충전금액  %d 원", _charging_cnt*2);
    lv_label_set_text(_label_cost, str_buf);

    lv_obj_invalidate(lv_scr_act());
    lv_task_handler();
    return 0;
}

static int on_event(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    return 0;
}

ui_wnd_handle* get_handle_wnd_charging(void)
{
    _wnd_handle.name = "charging window";
    _wnd_handle.create = on_create;
    _wnd_handle.destory = on_destory;
    _wnd_handle.suspend = on_suspend;
    _wnd_handle.resume = on_resume;
    _wnd_handle.paint = on_paint;
    _wnd_handle.event = on_event;
    return &_wnd_handle;
}

