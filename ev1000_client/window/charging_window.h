#ifndef CHARGING_WINDOW_H
#define CHARGING_WINDOW_H

#include <QMainWindow>

#include "ui_interface.h"

namespace Ui {
class charging_window;
}

class charging_window :
    public QMainWindow,
    public ui_interface
{
    Q_OBJECT

public:
    explicit charging_window(QWidget *parent = nullptr);
    ~charging_window();

public:
    // ui_interface
    int ui_start(void);
    int init(main_interface *p_main);
    int deinit(void);

    // main_interface
    int on_event(const event_c &ev);
    int on_timer(u32 id);

signals:
    void show_signal(void);
    void hide_signal(void);
    void event_signal(event_c ev);
    //void close_signal(void);

public slots:
    void show_slot(void);
    void hide_slot(void);
    void event_slot(event_c ev);

protected:
     void closeEvent(QCloseEvent *event);

public:
    int selftest(void);

private:
    Ui::charging_window *ui;
    main_interface *_p_main;
};

#endif // CHARGING_WINDOW_H
