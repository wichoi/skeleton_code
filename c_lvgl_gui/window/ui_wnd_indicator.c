#include <stdio.h>
#include <string.h>

#include <lvgl.h>
#include <lvgl_input_device.h>

#include <zephyr/logging/log.h>

#include "ui_task.h"
#include "ui_wnd.h"

LOG_MODULE_REGISTER(ui_indicator, CONFIG_DISPLAY_LOG_LEVEL);
LV_IMG_DECLARE(img_logo);
LV_IMG_DECLARE(img_signal);
LV_IMG_DECLARE(img_ethernet);
LV_IMG_DECLARE(img_connection);
LV_IMG_DECLARE(img_call_number);

#define UI_IND_LOGO_X           15
#define UI_IND_LOGO_Y           15

#define UI_IND_SIGNAL_X         430
#define UI_IND_SIGNAL_Y         15

#define UI_IND_CONN_X           450
#define UI_IND_CONN_Y           15

#define UI_IND_CALL_X           360
#define UI_IND_CALL_Y           290

static lv_obj_t *_icon_logo = NULL;
static lv_obj_t *_icon_signal = NULL;
static lv_obj_t *_icon_connection = NULL;
static lv_obj_t *_icon_call = NULL;

int ui_wnd_indicator_create(ui_ev_t *ev)
{
    uint8_t is_modem = 0; // 0(ethernet) 1(modem)
    _icon_logo = lv_img_create(lv_scr_act());
    lv_img_set_src(_icon_logo, &img_logo);
    lv_obj_set_pos(_icon_logo, UI_IND_LOGO_X, UI_IND_LOGO_Y);
    lv_obj_set_size(_icon_logo, img_logo.header.w, img_logo.header.h);

    _icon_signal = lv_img_create(lv_scr_act());
    if(is_modem)
    {
        lv_img_set_src(_icon_signal, &img_signal);
    }
    else
    {
        lv_img_set_src(_icon_signal, &img_ethernet);
    }
    lv_obj_set_pos(_icon_signal, UI_IND_SIGNAL_X, UI_IND_SIGNAL_Y);
    lv_obj_set_size(_icon_signal, img_ethernet.header.w, img_ethernet.header.h);

    _icon_connection = lv_img_create(lv_scr_act());
    lv_img_set_src(_icon_connection, &img_connection);
    lv_obj_set_pos(_icon_connection, UI_IND_CONN_X, UI_IND_CONN_Y);
    lv_obj_set_size(_icon_connection, img_connection.header.w, img_connection.header.h);

    _icon_call = lv_img_create(lv_scr_act());
    lv_img_set_src(_icon_call, &img_call_number);
    lv_obj_set_pos(_icon_call, UI_IND_CALL_X, UI_IND_CALL_Y);
    lv_obj_set_size(_icon_call, img_call_number.header.w, img_call_number.header.h);

    return 0;
}

int ui_wnd_indicator_destory(ui_ev_t *ev)
{
    lv_obj_del(_icon_logo);
    lv_obj_del(_icon_signal);
    lv_obj_del(_icon_connection);
    lv_obj_del(_icon_call);
    return 0;
}

