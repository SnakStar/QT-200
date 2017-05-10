#include <QApplication>
#include "mainwindow.h"
#include <QFile>
#include <QDir>
#include<QMessageBox>
#include<QTextCodec>
#include<frminput.h>


void myMessageOutput(QtMsgType type, const char* msg)
{
    QString text;
    switch (type)
    {
    case QtDebugMsg:
        text = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+"[Debug]: %1\r\n").arg(msg);
        break;
    case QtWarningMsg:
        text = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+"[Warning]: %1\r\n").arg(msg);
        break;
    case QtCriticalMsg:
        text = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+"[Critical]: %1\r\n").arg(msg);
        break;
    case QtFatalMsg:
        text = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+"[Fatal]: %1\r\n").arg(msg);
        abort();
    }
    QFile file("/home/root/qt200/log.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    if(file.size()/1000 > 1500){
        file.close();
        file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Truncate);
        file.close();
        file.open(QIODevice::WriteOnly | QIODevice::Append);
    }
    QTextStream ts(&file);
    ts<<text<<endl;
}


int main(int argc, char *argv[])
{
    qInstallMsgHandler(myMessageOutput);
    QApplication a(argc, argv);
    MainWindow w;

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));        //支持Tr中文

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8")); //支持中文文件名显示

    frmInput::Instance()->Init("bottom", "black", 12, 10);

    //w.setWindowFlags( Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    QFile qssFile(":/resource/qss/style.qss");
    qssFile.open(QIODevice::ReadOnly);
    if(qssFile.isOpen()){
        w.setStyleSheet(qssFile.readAll());
    }

    w.show();
    return a.exec();
}
