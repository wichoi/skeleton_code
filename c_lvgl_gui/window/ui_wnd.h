#ifndef __UI_WND_H__
#define __UI_WND_H__

typedef int (*fp)(ui_ev_t*);

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

#define UI_WND_WIDTH            480
#define UI_WND_HEIGHT           320

#define UI_WND_NONE             0
#define UI_WND_IDLE             1
#define UI_WND_BOOTING          2
#define UI_WND_ERROR            3
#define UI_WND_CREDIT_CARD      4
#define UI_WND_CHARGING         5
#define UI_WND_FINISHING        6
#define UI_WND_COMPLETE         7
#define UI_WND_STOP             8
#define UI_WND_AUTH             9
#define UI_WND_CABLE            10

int ui_wnd_init(void);
int ui_wnd_deinit(void);
int ui_wnd_proc(ui_ev_t *ev);

int ui_wnd_indicator_create(ui_ev_t *ev);
int ui_wnd_indicator_destory(ui_ev_t *ev);

ui_wnd_handle* get_handle_wnd_idle(void);
ui_wnd_handle* get_handle_wnd_booting(void);
ui_wnd_handle* get_handle_wnd_error(void);
ui_wnd_handle* get_handle_wnd_credit_card(void);
ui_wnd_handle* get_handle_wnd_charging(void);
ui_wnd_handle* get_handle_wnd_finishing(void);
ui_wnd_handle* get_handle_wnd_complete(void);
ui_wnd_handle* get_handle_wnd_stop(void);
ui_wnd_handle* get_handle_wnd_auth(void);
ui_wnd_handle* get_handle_wnd_cable(void);

#endif // __UI_WND_IDLE_H__