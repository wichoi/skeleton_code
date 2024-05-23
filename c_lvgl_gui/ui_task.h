#ifndef __UI_TASK_H__
#define __UI_TASK_H__

#define UI_EVENT_NONE               0

#define UI_EVENT_WND_CREATE         1001
#define UI_EVENT_WND_CREATE_EX      1002
#define UI_EVENT_WND_DESTORY        1003
#define UI_EVENT_WND_DESTORY_ALL    1004
#define UI_EVENT_WND_SUSPEND        1005
#define UI_EVENT_WND_RESUME         1006
#define UI_EVENT_WND_PAINT          1007
#define UI_EVENT_WND_EVENT          1008

#define UI_EVENT_WND_LIST           1100

#define UI_EVENT_HELLO              9000
#define UI_EVENT_TEST               9001

int ui_event_put(int event, int param, char *data, int len);

#endif // __UI_TASK_H__