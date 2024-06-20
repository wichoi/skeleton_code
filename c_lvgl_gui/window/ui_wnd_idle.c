#include <stdio.h>
#include <string.h>

#include <lvgl.h>
#include <lvgl_input_device.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ui_task.h"
#include "ui_wnd.h"

LOG_MODULE_REGISTER(ui_idle, CONFIG_DISPLAY_LOG_LEVEL);
LV_IMG_DECLARE(img_card);

static void on_timer(struct k_timer *timer_id);
K_TIMER_DEFINE(wnd_idle_timer, on_timer,NULL);

#define UI_IDLE_QR_X            100
#define UI_IDLE_QR_Y            80
#define UI_IDLE_QR_SZ           83
#define UI_IDLE_QR_BOARD        5

#define UI_IDLE_CARD_X          230
#define UI_IDLE_CARD_Y          80

#define UI_IDLE_LABEL_X         60
#define UI_IDLE_LABEL_Y         210

static ui_wnd_handle _wnd_handle = {0,};

static lv_obj_t *_qr = NULL;
static lv_obj_t *_icon = NULL;
static lv_obj_t *_label = NULL;

static uint32_t _idle_cnt = 0;

static void on_timer(struct k_timer *timer_id)
{
    //LOG_DBG("%s", __func__);
    _idle_cnt++;
    ui_event_put(UI_EVENT_WND_PAINT, 0, NULL, 0);
}

static int on_create(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    ui_wnd_indicator_create(ev);

    const char * data = "https://unicharger.co.kr/";
    _qr = lv_qrcode_create(lv_scr_act(), UI_IDLE_QR_SZ, lv_color_black(), lv_color_white());
    lv_qrcode_update(_qr, data, strlen(data));
    lv_obj_set_pos(_qr, UI_IDLE_QR_X, UI_IDLE_QR_Y);
    lv_obj_set_style_border_color(_qr, lv_color_white(), 0);
    lv_obj_set_style_border_width(_qr, UI_IDLE_QR_BOARD, 0);

    _icon = lv_img_create(lv_scr_act());
    lv_img_set_src(_icon, &img_card);
    lv_obj_set_pos(_icon, UI_IDLE_CARD_X, UI_IDLE_CARD_Y);
    lv_obj_set_size(_icon, img_card.header.w, img_card.header.h);

    _label = lv_label_create(lv_scr_act());
    lv_label_set_text(_label, "비회원은 비회원 버튼을 누르세요.");
    lv_obj_set_pos(_label, UI_IDLE_LABEL_X, UI_IDLE_LABEL_Y);
    //lv_obj_align(_label, LV_ALIGN_TOP_MID, 0, UI_IDLE_LABEL_Y);

    _idle_cnt = 0;

    k_timer_init(&wnd_idle_timer, on_timer, NULL);
    //k_timer_start(&wnd_idle_timer, K_MSEC(1000), K_MSEC(1000));
    return 0;
}

static int on_destory(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    k_timer_stop(&wnd_idle_timer);

    lv_obj_del(_qr);
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
#if 0
    if(_idle_cnt % 10)
    {
        lv_obj_clear_flag(_label, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_flag(_label, LV_OBJ_FLAG_HIDDEN);
    }
#endif

    lv_obj_invalidate(lv_scr_act());
    lv_task_handler();
    return 0;
}

static int on_event(ui_ev_t *ev)
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

