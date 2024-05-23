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

#include "ui_wnd.h"

LOG_MODULE_REGISTER(ui_power_off, CONFIG_DISPLAY_LOG_LEVEL);

static ui_wnd_handle _wnd_handle = {0,};



static lv_obj_t * meter1;
//static lv_obj_t * meter2;



static void set_value_meter1(void * indic, int32_t v)
{
    lv_meter_set_indicator_end_value(meter1, indic, v);
}

/*
static void set_value_meter2(void * indic, int32_t v)
{
    lv_meter_set_indicator_end_value(meter2, indic, v);
}
*/


static void lv_example_meter_1(void)
{
    meter1 = lv_meter_create(lv_scr_act());
    
    lv_obj_center(meter1);
    lv_obj_set_size(meter1, 200, 200);

    /*Add a scale first*/
    lv_meter_scale_t * scale = lv_meter_add_scale(meter1);
    lv_meter_set_scale_ticks(meter1, scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter1, scale, 8, 4, 15, lv_color_black(), 10);

    lv_meter_indicator_t * indic;

    /*Add a blue arc to the start*/
    indic = lv_meter_add_arc(meter1, scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(meter1, indic, 0);
    lv_meter_set_indicator_end_value(meter1, indic, 20);

    /*Make the tick lines blue at the start of the scale*/
    indic = lv_meter_add_scale_lines(meter1, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE),
                                     false, 0);
    lv_meter_set_indicator_start_value(meter1, indic, 0);
    lv_meter_set_indicator_end_value(meter1, indic, 20);

    /*Add a red arc to the end*/
    indic = lv_meter_add_arc(meter1, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(meter1, indic, 80);
    lv_meter_set_indicator_end_value(meter1, indic, 100);

    /*Make the tick lines red at the end of the scale*/
    indic = lv_meter_add_scale_lines(meter1, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false,
                                     0);
    lv_meter_set_indicator_start_value(meter1, indic, 80);
    lv_meter_set_indicator_end_value(meter1, indic, 100);

    /*Add a needle line indicator*/
    indic = lv_meter_add_needle_line(meter1, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

    /*Create an animation to set the value*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_value_meter1);
    lv_anim_set_var(&a, indic);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

/*
static void lv_example_meter_2(void)
{
    lv_obj_t *meter2 = lv_meter_create(lv_scr_act());

    lv_obj_center(meter2);
    lv_obj_set_size(meter2, 200, 200);

    // Remove the circle from the middle
    lv_obj_remove_style(meter2, NULL, LV_PART_INDICATOR);

    // Add a scale first
    lv_meter_scale_t * scale = lv_meter_add_scale(meter2);
    lv_meter_set_scale_ticks(meter2, scale, 11, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter2, scale, 1, 2, 30, lv_color_hex3(0xeee), 15);
    lv_meter_set_scale_range(meter2, scale, 0, 100, 270, 90);

    // Add a three arc indicator
    lv_meter_indicator_t * indic1 = lv_meter_add_arc(meter2, scale, 10, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_indicator_t * indic2 = lv_meter_add_arc(meter2, scale, 10, lv_palette_main(LV_PALETTE_GREEN), -10);
    lv_meter_indicator_t * indic3 = lv_meter_add_arc(meter2, scale, 10, lv_palette_main(LV_PALETTE_BLUE), -20);

    // Create an animation to set the value
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_value_meter2);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    lv_anim_set_time(&a, 2000);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_var(&a, indic1);
    lv_anim_start(&a);

    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 1000);
    lv_anim_set_var(&a, indic2);
    lv_anim_start(&a);

    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_var(&a, indic3);
    lv_anim_start(&a);
}
*/

static int on_create(void)
{
    LOG_INF("%s", __func__);

    lv_example_meter_1();
//    lv_example_meter_2();

    return 0;
}

static int on_destory(void)
{
    LOG_INF("%s", __func__);
    lv_anim_del_all();
    lv_obj_del(meter1);
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
    LOG_INF("%s", __func__);
    lv_task_handler();
    return 0;
}

static int on_event(void)
{
    LOG_INF("%s", __func__);
    return 0;
}

ui_wnd_handle* get_handle_wnd_power_off(void)
{
    _wnd_handle.name = "power off window";
    _wnd_handle.create = on_create;
    _wnd_handle.destory = on_destory;
    _wnd_handle.suspend = on_suspend;
    _wnd_handle.resume = on_resume;
    _wnd_handle.paint = on_paint;
    _wnd_handle.event = on_event;
    return &_wnd_handle;
}

