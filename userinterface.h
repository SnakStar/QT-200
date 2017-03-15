/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: userinterface.h
 *  简要描述: 软件调试主界面
 *
 *  创建日期: 2017-01-11
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QDialog>
#include<QMessageBox>
#include<QDateTime>
#include"cqtprodb.h"
#include"cutilsettings.h"
#ifdef Q_OS_LINUX
#include"canbus.h"
#endif
#include"CanCmd.h"
#include"QProcess"

#define FRAMESTART 0X5A1AA1A5
#define FRAMEEND   0XA5A11A5A

namespace Ui {
class UserInterface;
}

class UserInterface : public QDialog
{
    Q_OBJECT
    
public:
    explicit UserInterface(QWidget *parent = 0);
    explicit UserInterface(QWidget *parent,CQtProDB* db, CUtilSettings* settings,QextSerialPort* rfSerialPort);
    ~UserInterface();
public:
    //设置CanBus对象
#ifdef Q_OS_LINUX
    void SetCanBusObj(CanBus* cb);
#endif
private slots:
    void RecvRFSerialPortData();
private slots:
    void on_btnUIOK_clicked();
    void on_btnStopAging_clicked();

    void on_btnUIClose_clicked();

    void on_btnAPOffSet1_clicked();

    void on_btnAPOffSet2_clicked();

    void on_btnAPOffSet3_clicked();

    void on_btnClearDb_clicked();

    void on_btnCopyDb_clicked();

    void on_rbRFReadMode_clicked();

    void on_rbRFWriteMode_clicked();

    void on_rbRFCloseMode_clicked();

    void on_btnRFClearWriteTotal_clicked();

signals:
    void StopAgingTest();

private:
    Ui::UserInterface *ui;
//private:
    //virtual void closeEvent(QCloseEvent *event);
private:
    //
#ifdef Q_OS_LINUX
    CanBus* m_CanBus;
#endif
    //数据库对象
    CQtProDB* m_db;
    //配置文件对象
    CUtilSettings* m_settings;
    //配置文件参数表
    QMap<QString,QString>* m_SetParam;
    //射频串口对象
    QextSerialPort* m_RFSerialPort;
    //当前串口发来的数据
    QByteArray m_baRFSerialData;
    //等待复制的串口数据
    QByteArray m_baWaitCopySerialData;
    //烧录成功总数
    quint32 m_nTotal;
private:
    //更新控件
    void UpdateControl();
    //初始化导航栏
    void InitListControl();
    //初始化radio控件
    void InitRadioControl();
    //初始化放大偏移量控件
    void InitAPOffSetShowValue();
    //初始化射频界面控件
    void InitRFUIControl();
    //根据配置文件初始化控件
    void UpdateControlStatus();
    //
    //void TestFunc();
};

#endif // USERINTERFACE_H
