#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFile>
#include <QTextStream>

#include "log_service.h"

void log_file_handler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString file_name = "./ev1000_client.log"; // todo file path
    QString backup_name = "./ev1000_client.log.bak";

    static QTextStream ts(stdout);
    ts << msg << endl;

    QFile file(file_name);
    if(!file.open(QIODevice::Append | QIODevice::WriteOnly))
    {
        return;
    }
    QTextStream out(&file);

    //QString curtime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    context; //context.file, context.line, context.function
    switch (type)
    {
    case QtDebugMsg:
        out << msg << "\n";
        break;
    case QtInfoMsg:
        out << msg << "\n";
        break;
    case QtWarningMsg:
        out << msg << "\n";
        break;
    case QtCriticalMsg:
        out << msg << "\n";
        break;
    case QtFatalMsg:
        out << msg << "\n";
        //abort();
        break;
    default:
        break;
    }
    file.close();

    int size = 0;
    QFile myFile(file_name);
    if (myFile.open(QIODevice::ReadOnly))
    {
        size = myFile.size();
        myFile.close();
    }

    if(size > 256 * 1024)
    {
        if(QFile::exists(backup_name) == true)
        {
            QFile::remove(backup_name);
        }

        if(QFile::copy(file_name, backup_name) == true)
        {
            QFile::remove(file_name);
        }
    }
}

