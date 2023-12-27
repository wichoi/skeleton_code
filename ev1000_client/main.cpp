#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "log_service.h"
#include "main_task.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInstallMessageHandler(log_file_handler);
    log_i("ev1000_client start !!!");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "qt_project_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    shared_ptr<main_task> main_handle = make_shared<main_task>();
    main_handle->task_start(a);

    //log_i("ev1000_client run !!!");
    return a.exec();
}
