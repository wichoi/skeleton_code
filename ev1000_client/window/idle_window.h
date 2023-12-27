#ifndef IDLE_WINDOW_H
#define IDLE_WINDOW_H

#include <QMainWindow>

#include "ui_interface.h"

namespace Ui {
class idle_window;
}

class idle_window :
    public QMainWindow,
    public ui_interface
{
    Q_OBJECT

public:
    explicit idle_window(QWidget *parent = nullptr);
    ~idle_window();

public:
    // main_interface
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
    Ui::idle_window *ui;
    main_interface *_p_main;
};

#endif // IDLE_WINDOW_H
