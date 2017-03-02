#include <QApplication>
#include "mainwindow.h"
#include <QFile>
#include <QDir>
#include<QMessageBox>
#include<QTextCodec>
#include<frminput.h>

int main(int argc, char *argv[])
{
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
