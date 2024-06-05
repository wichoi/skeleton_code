#include <stdio.h>
#include <string.h>

#include <lvgl.h>
#include <lvgl_input_device.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ui_task.h"
#include "ui_wnd.h"

LOG_MODULE_REGISTER(ui_booting, CONFIG_DISPLAY_LOG_LEVEL);
LV_IMG_DECLARE(img_unicharger);
LV_IMG_DECLARE(img_ellipse_full);
LV_IMG_DECLARE(img_ellipse_empty);

static void on_timer(struct k_timer *timer_id);
K_TIMER_DEFINE(wnd_boot_timer, on_timer, NULL);

#define UI_BOOT_LOGO_X          120
#define UI_BOOT_LOGO_Y          120

#define UI_BOOT_ARCH_X          150
#define UI_BOOT_ARCH_Y          180
#define UI_BOOT_ARCH_OFFSET     40

#define UI_BOOT_TIMEOUT_CNT     5

static ui_wnd_handle _wnd_handle = {0,};
static lv_obj_t *_icon_unicharger = NULL;
static lv_obj_t *_arch_01;
static lv_obj_t *_arch_02;
static lv_obj_t *_arch_03;
static lv_obj_t *_arch_04;
static lv_obj_t *_arch_05;
static int _timeout_cnt = 0;

static void on_timer(struct k_timer *timer_id)
{
    //LOG_DBG("%s", __func__);
    if(_timeout_cnt++ > UI_BOOT_TIMEOUT_CNT)
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
    LOG_INF("%s", __func__);

    _icon_unicharger = lv_img_create(lv_scr_act());
    lv_img_set_src(_icon_unicharger, &img_unicharger);
    lv_obj_set_pos(_icon_unicharger, UI_BOOT_LOGO_X, UI_BOOT_LOGO_Y);
    //lv_obj_align(_icon_unicharger, LV_ALIGN_TOP_MID, 0, UI_BOOT_LOGO_Y);
    lv_obj_set_size(_icon_unicharger, img_unicharger.header.w, img_unicharger.header.h);

    _arch_01 = lv_img_create(lv_scr_act());
    lv_img_set_src(_arch_01, &img_ellipse_full);
    lv_obj_set_pos(_arch_01, UI_BOOT_ARCH_X, UI_BOOT_ARCH_Y);
    lv_obj_set_size(_arch_01, img_ellipse_full.header.w, img_ellipse_full.header.h);

    _arch_02 = lv_img_create(lv_scr_act());
    lv_img_set_src(_arch_02, &img_ellipse_empty);
    lv_obj_set_pos(_arch_02, UI_BOOT_ARCH_X + UI_BOOT_ARCH_OFFSET, UI_BOOT_ARCH_Y);
    lv_obj_set_size(_arch_02, img_ellipse_empty.header.w, img_ellipse_empty.header.h);

    _arch_03 = lv_img_create(lv_scr_act());
    lv_img_set_src(_arch_03, &img_ellipse_empty);
    lv_obj_set_pos(_arch_03, UI_BOOT_ARCH_X + UI_BOOT_ARCH_OFFSET*2, UI_BOOT_ARCH_Y);
    lv_obj_set_size(_arch_03, img_ellipse_empty.header.w, img_ellipse_empty.header.h);

    _arch_04 = lv_img_create(lv_scr_act());
    lv_img_set_src(_arch_04, &img_ellipse_empty);
    lv_obj_set_pos(_arch_04, UI_BOOT_ARCH_X + UI_BOOT_ARCH_OFFSET*3, UI_BOOT_ARCH_Y);
    lv_obj_set_size(_arch_04, img_ellipse_empty.header.w, img_ellipse_empty.header.h);

    _arch_05 = lv_img_create(lv_scr_act());
    lv_img_set_src(_arch_05, &img_ellipse_empty);
    lv_obj_set_pos(_arch_05, UI_BOOT_ARCH_X + UI_BOOT_ARCH_OFFSET*4, UI_BOOT_ARCH_Y);
    lv_obj_set_size(_arch_05, img_ellipse_empty.header.w, img_ellipse_empty.header.h);

    _timeout_cnt = 0;
    k_timer_init(&wnd_boot_timer, on_timer, NULL);
    k_timer_start(&wnd_boot_timer, K_MSEC(1000), K_MSEC(1000));
    return 0;
}

static int on_destory(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    k_timer_stop(&wnd_boot_timer);
    lv_obj_del(_icon_unicharger);
    lv_obj_del(_arch_01);
    lv_obj_del(_arch_02);
    lv_obj_del(_arch_03);
    lv_obj_del(_arch_04);
    lv_obj_del(_arch_05);
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

    int progress = _timeout_cnt % 5;
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

    lv_obj_invalidate(lv_scr_act());
    lv_task_handler();
    return 0;
}

static int on_event(ui_ev_t *ev)
{
    LOG_INF("%s", __func__);
    return 0;
}

ui_wnd_handle* get_handle_wnd_booting(void)
{
    _wnd_handle.name = "booting window";
    _wnd_handle.create = on_create;
    _wnd_handle.destory = on_destory;
    _wnd_handle.suspend = on_suspend;
    _wnd_handle.resume = on_resume;
    _wnd_handle.paint = on_paint;
    _wnd_handle.event = on_event;
    return &_wnd_handle;
}

