#include <stdio.h>
#include <string.h>

#include <lvgl.h>
#include <lvgl_input_device.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ui_task.h"
#include "ui_wnd.h"

LOG_MODULE_REGISTER(ui_error, CONFIG_DISPLAY_LOG_LEVEL);
LV_IMG_DECLARE(img_error);

static void on_timer(struct k_timer *timer_id);
K_TIMER_DEFINE(wnd_error_timer, on_timer,NULL);

#define UI_ERR_ICON_X           195
#define UI_ERR_ICON_Y           90

#define UI_ERR_LABEL_X          120
#define UI_ERR_LABEL_Y          210

static ui_wnd_handle _wnd_handle = {0,};

static lv_obj_t *_icon = NULL;
static lv_obj_t *_label = NULL;

static uint32_t _error_cnt = 0;

static void on_timer(struct k_timer *timer_id)
{
    //LOG_DBG("%s", __func__);
    if(_error_cnt++> 5)
    {
        ui_event_put(UI_EVENT_WND_CREATE_EX, UI_WND_IDLE, NULL, 0);
    }
}

static int on_create(ui_ev_t *ev)
{
    int err_code = 0; // 0(network), 1(credit), 2(cable)
    LOG_INF("%s", __func__);
    ui_wnd_indicator_create(ev);

    static lv_style_t def_style;
    lv_style_init(&def_style);
    lv_style_set_text_color(&def_style, lv_palette_main(LV_PALETTE_RED));

    _icon = lv_img_create(lv_scr_act());
    lv_img_set_src(_icon, &img_error);
    lv_obj_set_pos(_icon, UI_ERR_ICON_X, UI_ERR_ICON_Y);
    //lv_obj_align(_icon, LV_ALIGN_TOP_MID, 0, UI_ERR_ICON_Y);
    lv_obj_set_size(_icon, img_error.header.w, img_error.header.h);

    _label = lv_label_create(lv_scr_act());
    if(err_code == 0)
        lv_label_set_text(_label, "서버에 연결할 수 없습니다.");
    else if(err_code == 1)
        lv_label_set_text(_label, "결제에 실패하였습니다.");
    else if(err_code == 2)
        lv_label_set_text(_label, "커넥터 연결 시간이 초과하였습니다.");
    else
        lv_label_set_text(_label, "error");
    //lv_obj_set_pos(_label, UI_ERR_LABEL_X, UI_ERR_LABEL_Y);
    lv_obj_align(_label, LV_ALIGN_TOP_MID, 0, UI_ERR_LABEL_Y);
    lv_obj_add_style(_label, &def_style, 0);

    _error_cnt = 0;

    k_timer_init(&wnd_error_timer, on_timer, NULL);
    //k_timer_start(&wnd_error_timer, K_MSEC(1000), K_MSEC(1000));
    return 0;
}

static int on_destory(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    k_timer_stop(&wnd_error_timer);

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

ui_wnd_handle* get_handle_wnd_error(void)
{
    _wnd_handle.name = "error window";
    _wnd_handle.create = on_create;
    _wnd_handle.destory = on_destory;
    _wnd_handle.suspend = on_suspend;
    _wnd_handle.resume = on_resume;
    _wnd_handle.paint = on_paint;
    _wnd_handle.event = on_event;
    return &_wnd_handle;
}

