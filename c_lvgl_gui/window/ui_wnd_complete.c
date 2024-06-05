#include <stdio.h>
#include <string.h>
#include <time.h>

#include <lvgl.h>
#include <lvgl_input_device.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ui_task.h"
#include "ui_wnd.h"

LOG_MODULE_REGISTER(ui_complete, CONFIG_DISPLAY_LOG_LEVEL);
LV_IMG_DECLARE(img_complete);

static void on_timer(struct k_timer *timer_id);
K_TIMER_DEFINE(wnd_complete_timer, on_timer,NULL);

#define UI_COMPLETE_LABEL_X         45
#define UI_COMPLETE_LABEL_Y         70

#define UI_COMPLETE_ICON_X          50
#define UI_COMPLETE_ICON_Y          110

#define UI_COMPLETE_PROC_X          45
#define UI_COMPLETE_PROC_Y          140
#define UI_COMPLETE_PROC_W          28
#define UI_COMPLETE_PROC_H          37

#define UI_COMPLETE_TIME_X          190
#define UI_COMPLETE_TIME_Y          80

#define UI_COMPLETE_METER_X         190
#define UI_COMPLETE_METER_Y         120 // UI_COMPLETE_TIME_Y + 40

#define UI_COMPLETE_COST_X          190
#define UI_COMPLETE_COST_Y          160 // UI_COMPLETE_COST_Y + 40

#define UI_COMPLETE_BOTTOM_X        40
#define UI_COMPLETE_BOTTOM_Y        240

static ui_wnd_handle _wnd_handle = {0,};

static lv_obj_t *_label_complete = NULL;
static lv_obj_t *_img_complete = NULL;

static lv_obj_t *_label_time = NULL;
static lv_obj_t *_label_meter = NULL;
static lv_obj_t *_label_cost = NULL;
static lv_obj_t *_label_bottom = NULL;

static uint32_t _complete_cnt = 0;

static void on_timer(struct k_timer *timer_id)
{
    //LOG_DBG("%s", __func__);
    _complete_cnt++;
    ui_event_put(UI_EVENT_WND_PAINT, 0, NULL, 0);
}

static int on_create(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    ui_wnd_indicator_create(ev);

    _label_complete = lv_label_create(lv_scr_act());
    lv_label_set_text(_label_complete, "충전완료");
    lv_obj_set_pos(_label_complete, UI_COMPLETE_LABEL_X, UI_COMPLETE_LABEL_Y);

    _img_complete = lv_img_create(lv_scr_act());
    lv_img_set_src(_img_complete, &img_complete);
    lv_obj_set_pos(_img_complete, UI_COMPLETE_ICON_X, UI_COMPLETE_ICON_Y);
    lv_obj_set_size(_img_complete, img_complete.header.w, img_complete.header.h);

    _label_time = lv_label_create(lv_scr_act());
    lv_label_set_text(_label_time, "충전시간");
    lv_obj_set_pos(_label_time, UI_COMPLETE_TIME_X, UI_COMPLETE_TIME_Y);

    _label_meter = lv_label_create(lv_scr_act());
    lv_label_set_text(_label_meter, "충전량");
    lv_obj_set_pos(_label_meter, UI_COMPLETE_METER_X, UI_COMPLETE_METER_Y);

    _label_cost = lv_label_create(lv_scr_act());
    lv_label_set_text(_label_cost, "충전금액");
    lv_obj_set_pos(_label_cost, UI_COMPLETE_COST_X, UI_COMPLETE_COST_Y);

    _label_bottom = lv_label_create(lv_scr_act());
    lv_label_set_text(_label_bottom, "커넥터를 분리해주세요.");
    //lv_obj_set_pos(_label_bottom, UI_COMPLETE_BOTTOM_X, UI_COMPLETE_BOTTOM_Y);
    lv_obj_align(_label_bottom, LV_ALIGN_TOP_MID, 0, UI_COMPLETE_BOTTOM_Y);

    _complete_cnt = 0;

    k_timer_init(&wnd_complete_timer, on_timer, NULL);
    k_timer_start(&wnd_complete_timer, K_MSEC(1000), K_MSEC(1000));
    return 0;
}

static int on_destory(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    k_timer_stop(&wnd_complete_timer);

    lv_obj_del(_label_complete);
    lv_obj_del(_img_complete);

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
    char str_buf[32] = {0,};
    char time_buf[16] = {0,};
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_buf, sizeof(time_buf), "%H:%M:%S", timeinfo);
    sprintf(str_buf, "충전시간  %s", time_buf);
    lv_label_set_text(_label_time, str_buf);

    sprintf(str_buf, "충전량     %d kWh", _complete_cnt);
    lv_label_set_text(_label_meter, str_buf);

    sprintf(str_buf, "충전금액  %d 원", _complete_cnt*2);
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

ui_wnd_handle* get_handle_wnd_complete(void)
{
    _wnd_handle.name = "complete window";
    _wnd_handle.create = on_create;
    _wnd_handle.destory = on_destory;
    _wnd_handle.suspend = on_suspend;
    _wnd_handle.resume = on_resume;
    _wnd_handle.paint = on_paint;
    _wnd_handle.event = on_event;
    return &_wnd_handle;
}

