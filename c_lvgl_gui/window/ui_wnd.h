#ifndef __UI_WND_H__
#define __UI_WND_H__

typedef int (*fp)(void);

typedef struct ui_wnd_tag
{
    char *name;
    fp create;
    fp destory;
    fp suspend;
    fp resume;
    fp paint;
    fp event;
} ui_wnd_handle;

#define UI_WND_NONE             0
#define UI_WND_IDLE             1
#define UI_WND_CHARGING         2

#define UI_WND_POWER_ON         10
#define UI_WND_POWER_OFF        11

int ui_wnd_init(void);
int ui_wnd_deinit(void);
int ui_wnd_proc(int event, int param, char *data, int len);

ui_wnd_handle* get_handle_wnd_idle(void);
ui_wnd_handle* get_handle_wnd_charging(void);

ui_wnd_handle* get_handle_wnd_power_on(void);
ui_wnd_handle* get_handle_wnd_power_off(void);

#endif // __UI_WND_IDLE_H__