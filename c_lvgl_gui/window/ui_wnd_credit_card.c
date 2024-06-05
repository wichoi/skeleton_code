#include <stdio.h>
#include <string.h>

#include <lvgl.h>
#include <lvgl_input_device.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ui_task.h"
#include "ui_wnd.h"

LOG_MODULE_REGISTER(ui_credit, CONFIG_DISPLAY_LOG_LEVEL);
LV_IMG_DECLARE(img_credit_card);

static void on_timer(struct k_timer *timer_id);
K_TIMER_DEFINE(wnd_card_timer, on_timer,NULL);

#define UI_QR_CODE_X            190
#define UI_QR_CODE_Y            80
#define UI_QR_CODE_SZ           83
#define UI_QR_CODE_BOARD        5

#define UI_QR_LABEL_X           120
#define UI_QR_LABEL_Y           210

#define UI_CREDIT_X             175
#define UI_CREDIT_Y             70

#define UI_CREDIT_LABEL_X       60
#define UI_CREDIT_LABEL_Y       230

static ui_wnd_handle _wnd_handle = {0,};

static lv_obj_t *_icon = NULL;
static lv_obj_t *_label = NULL;

static uint32_t _credit_cnt = 0;

static void on_timer(struct k_timer *timer_id)
{
    //LOG_DBG("%s", __func__);
    if(_credit_cnt++ > 60)
    {
        ui_event_put(UI_EVENT_WND_CREATE_EX, UI_WND_IDLE, NULL, 0);
    }
    else
    {
        ui_event_put(UI_EVENT_WND_PAINT, 0, NULL, 0);
    }
}

static int on_create(ui_ev_t *ev)
{
    uint8_t is_qr = 0; // 0(qr_code) 1(credit_card)
    LOG_INF("%s", __func__);
    ui_wnd_indicator_create(ev);

    if(is_qr)
    {
        const char * data = "https://unicharger.co.kr/";
        _icon = lv_qrcode_create(lv_scr_act(), UI_QR_CODE_SZ, lv_color_black(), lv_color_white());
        lv_qrcode_update(_icon, data, strlen(data));
        lv_obj_set_pos(_icon, UI_QR_CODE_X, UI_QR_CODE_Y);
        //lv_obj_align(_icon, LV_ALIGN_TOP_MID, 0, UI_QR_CODE_Y);
        lv_obj_set_style_border_color(_icon, lv_color_white(), 0);
        lv_obj_set_style_border_width(_icon, UI_QR_CODE_BOARD, 0);

        _label = lv_label_create(lv_scr_act());
        lv_label_set_text(_label, "QR코드를 스켄하세요.");
        lv_obj_set_pos(_label, UI_QR_LABEL_X, UI_QR_LABEL_Y);
        //lv_obj_align(_label, LV_ALIGN_TOP_MID, 0, UI_QR_LABEL_Y);
    }
    else
    {
        _icon = lv_img_create(lv_scr_act());
        lv_img_set_src(_icon, &img_credit_card);
        lv_obj_set_pos(_icon, UI_CREDIT_X, UI_CREDIT_Y);
        //lv_obj_align(_icon, LV_ALIGN_TOP_MID, 0, UI_CREDIT_Y);
        lv_obj_set_size(_icon, img_credit_card.header.w, img_credit_card.header.h);

        _label = lv_label_create(lv_scr_act());
        lv_label_set_text(_label, "신용카드를 결제기에 넣어 주세요.");
        lv_obj_set_pos(_label, UI_CREDIT_LABEL_X, UI_CREDIT_LABEL_Y);
        //lv_obj_align(_label, LV_ALIGN_TOP_MID, 0, UI_CREDIT_LABEL_Y);
    }

    _credit_cnt = 0;

    k_timer_init(&wnd_card_timer, on_timer, NULL);
    //k_timer_start(&wnd_card_timer, K_MSEC(1000), K_MSEC(1000));
    return 0;
}

static int on_destory(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    k_timer_stop(&wnd_card_timer);

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

ui_wnd_handle* get_handle_wnd_credit_card(void)
{
    _wnd_handle.name = "credit_card window";
    _wnd_handle.create = on_create;
    _wnd_handle.destory = on_destory;
    _wnd_handle.suspend = on_suspend;
    _wnd_handle.resume = on_resume;
    _wnd_handle.paint = on_paint;
    _wnd_handle.event = on_event;
    return &_wnd_handle;
}

