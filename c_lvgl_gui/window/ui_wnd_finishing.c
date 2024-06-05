#include <stdio.h>
#include <string.h>
#include <time.h>

#include <lvgl.h>
#include <lvgl_input_device.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ui_task.h"
#include "ui_wnd.h"

LOG_MODULE_REGISTER(ui_finishing, CONFIG_DISPLAY_LOG_LEVEL);
LV_IMG_DECLARE(img_ellipse_full);
LV_IMG_DECLARE(img_ellipse_empty);

static void on_timer(struct k_timer *timer_id);
K_TIMER_DEFINE(wnd_finishing_timer, on_timer,NULL);

#define UI_FINISH_LABEL_X       55
#define UI_FINISH_LABEL_Y       80

#define UI_FINISH_ARCH_X        40
#define UI_FINISH_ARCH_Y        140
#define UI_FINISH_ARCH_OFFSET   23

#define UI_FINISH_TIME_X        190
#define UI_FINISH_TIME_Y        80

#define UI_FINISH_METER_X       190
#define UI_FINISH_METER_Y       120 // UI_FINISH_TIME_Y + 40

#define UI_FINISH_COST_X        190
#define UI_FINISH_COST_Y        160 // UI_FINISH_COST_Y + 40

#define UI_FINISH_BOTTOM_X      40
#define UI_FINISH_BOTTOM_Y      240

static ui_wnd_handle _wnd_handle = {0,};

static lv_obj_t *_label_finishing = NULL;
static lv_obj_t *_arch_01;
static lv_obj_t *_arch_02;
static lv_obj_t *_arch_03;
static lv_obj_t *_arch_04;
static lv_obj_t *_arch_05;

static lv_obj_t *_label_time = NULL;
static lv_obj_t *_label_meter = NULL;
static lv_obj_t *_label_cost = NULL;
static lv_obj_t *_label_bottom = NULL;

static uint32_t _finishing_cnt = 0;

static void on_timer(struct k_timer *timer_id)
{
    //LOG_DBG("%s", __func__);
    _finishing_cnt++;
    ui_event_put(UI_EVENT_WND_PAINT, 0, NULL, 0);
}

static int on_create(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    ui_wnd_indicator_create(ev);

    _label_finishing = lv_label_create(lv_scr_act());
    lv_label_set_text(_label_finishing, "종료중");
    lv_obj_set_pos(_label_finishing, UI_FINISH_LABEL_X, UI_FINISH_LABEL_Y);

    _arch_01 = lv_img_create(lv_scr_act());
    lv_img_set_src(_arch_01, &img_ellipse_full);
    lv_obj_set_pos(_arch_01, UI_FINISH_ARCH_X, UI_FINISH_ARCH_Y);
    lv_obj_set_size(_arch_01, img_ellipse_full.header.w, img_ellipse_full.header.h);

    _arch_02 = lv_img_create(lv_scr_act());
    lv_img_set_src(_arch_02, &img_ellipse_empty);
    lv_obj_set_pos(_arch_02, UI_FINISH_ARCH_X + UI_FINISH_ARCH_OFFSET, UI_FINISH_ARCH_Y);
    lv_obj_set_size(_arch_02, img_ellipse_empty.header.w, img_ellipse_empty.header.h);

    _arch_03 = lv_img_create(lv_scr_act());
    lv_img_set_src(_arch_03, &img_ellipse_empty);
    lv_obj_set_pos(_arch_03, UI_FINISH_ARCH_X + UI_FINISH_ARCH_OFFSET*2, UI_FINISH_ARCH_Y);
    lv_obj_set_size(_arch_03, img_ellipse_empty.header.w, img_ellipse_empty.header.h);

    _arch_04 = lv_img_create(lv_scr_act());
    lv_img_set_src(_arch_04, &img_ellipse_empty);
    lv_obj_set_pos(_arch_04, UI_FINISH_ARCH_X + UI_FINISH_ARCH_OFFSET*3, UI_FINISH_ARCH_Y);
    lv_obj_set_size(_arch_04, img_ellipse_empty.header.w, img_ellipse_empty.header.h);

    _arch_05 = lv_img_create(lv_scr_act());
    lv_img_set_src(_arch_05, &img_ellipse_empty);
    lv_obj_set_pos(_arch_05, UI_FINISH_ARCH_X + UI_FINISH_ARCH_OFFSET*4, UI_FINISH_ARCH_Y);
    lv_obj_set_size(_arch_05, img_ellipse_empty.header.w, img_ellipse_empty.header.h);

    _label_time = lv_label_create(lv_scr_act());
    lv_label_set_text(_label_time, "충전시간");
    lv_obj_set_pos(_label_time, UI_FINISH_TIME_X, UI_FINISH_TIME_Y);

    _label_meter = lv_label_create(lv_scr_act());
    lv_label_set_text(_label_meter, "충전량");
    lv_obj_set_pos(_label_meter, UI_FINISH_METER_X, UI_FINISH_METER_Y);

    _label_cost = lv_label_create(lv_scr_act());
    lv_label_set_text(_label_cost, "충전금액");
    lv_obj_set_pos(_label_cost, UI_FINISH_COST_X, UI_FINISH_COST_Y);

    _label_bottom = lv_label_create(lv_scr_act());
    lv_label_set_text(_label_bottom, "충전종료 중입니다. 잠시만 기다려주세요.");
    //lv_obj_set_pos(_label_bottom, UI_FINISH_BOTTOM_X, UI_FINISH_BOTTOM_Y);
    lv_obj_align(_label_bottom, LV_ALIGN_TOP_MID, 0, UI_FINISH_BOTTOM_Y);

    _finishing_cnt = 0;

    k_timer_init(&wnd_finishing_timer, on_timer, NULL);
    k_timer_start(&wnd_finishing_timer, K_MSEC(1000), K_MSEC(1000));
    return 0;
}

static int on_destory(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    k_timer_stop(&wnd_finishing_timer);

    lv_obj_del(_label_finishing);
    lv_obj_del(_arch_01);
    lv_obj_del(_arch_02);
    lv_obj_del(_arch_03);
    lv_obj_del(_arch_04);
    lv_obj_del(_arch_05);

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
    int progress = _finishing_cnt % 5;
    if(progress == 0)
    {
        lv_img_set_src(_arch_05, &img_ellipse_empty);
        lv_img_set_src(_arch_01, &img_ellipse_full);
    }
    else if(progress == 1)
    {
        lv_img_set_src(_arch_01, &img_ellipse_empty);
        lv_img_set_src(_arch_02, &img_ellipse_full);
    }
    else if(progress == 2)
    {
        lv_img_set_src(_arch_02, &img_ellipse_empty);
        lv_img_set_src(_arch_03, &img_ellipse_full);
    }
    else if(progress == 3)
    {
        lv_img_set_src(_arch_03, &img_ellipse_empty);
        lv_img_set_src(_arch_04, &img_ellipse_full);
    }
    else if(progress == 4)
    {
        lv_img_set_src(_arch_04, &img_ellipse_empty);
        lv_img_set_src(_arch_05, &img_ellipse_full);
    }

    char str_buf[32] = {0,};
    char time_buf[16] = {0,};
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_buf, sizeof(time_buf), "%H:%M:%S", timeinfo);
    sprintf(str_buf, "충전시간  %s", time_buf);
    lv_label_set_text(_label_time, str_buf);

    sprintf(str_buf, "충전량     %d kWh", _finishing_cnt);
    lv_label_set_text(_label_meter, str_buf);

    sprintf(str_buf, "충전금액  %d 원", _finishing_cnt*2);
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

ui_wnd_handle* get_handle_wnd_finishing(void)
{
    _wnd_handle.name = "finishing window";
    _wnd_handle.create = on_create;
    _wnd_handle.destory = on_destory;
    _wnd_handle.suspend = on_suspend;
    _wnd_handle.resume = on_resume;
    _wnd_handle.paint = on_paint;
    _wnd_handle.event = on_event;
    return &_wnd_handle;
}


