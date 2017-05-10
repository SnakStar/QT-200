/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: setwindow.h
 *  简要描述: 设置界面头文件，实现操作系统、仪器打印、时间、Lis等设置
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef SETWINDOW_H
#define SETWINDOW_H
#include <QWidget>
#include<QObject>
#include<QMessageBox>
#include<QDebug>
#include<QDate>
#include<QTime>
#include<QTranslator>
#include<QDir>
#ifdef Q_OS_WIN32
#include<windows.h>
#endif
#include"cutilsettings.h"
#include"renfvalue.h"
#include"cqtprodb.h"
#include"upgrading.h"
#include"hardgrading.h"
#include"querywindow.h"
#include"userinterface.h"
#include"testwindow.h"

#define RENFVALUE_COLUMN 4

namespace Ui {
class SetWindow;
}

class SetWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SetWindow(QWidget *parent = 0);
    ~SetWindow();

private slots:
    void RecvAddRenfValue(RenfInfo& ri);
    void RecvModifyRenfValue(RenfInfo& ri);
    void AutoUnlock();

private slots:

    void on_btnPrintSave_clicked();

    void on_btnPrintCancel_clicked();

    void on_btnAdd_clicked();

    void on_btnModify_clicked();

    void on_btnDelete_clicked();

    void on_btnSysSave_clicked();

    void on_btnSysCancel_clicked();

    void on_btnSoftUpdate_clicked();

    void on_btnSetDateSave_clicked();

    void on_cBoxLanguage_activated(int index);

    void on_btnDebugOk_clicked();

    void on_btnDebugLogout_clicked();

    void on_btnHardUpdate_clicked();
    void on_btnDebugInterface_clicked();


    void on_btnRFCQuery_clicked();

public:
    //设置数据库对象
    void SetDbObj(CQtProDB* db);
    //更新数据库内参考值内容到表格上
    void UpdateDbToRenf();
    //更新硬件版本信息
    void UpdateHardVersion(QString strVersion);
//Attribute
private:
    Ui::SetWindow *ui;
    //配置文件对象
    CUtilSettings* m_settings;
    //配置文件参数表
    QMap<QString,QString>* m_SetParam;
    //数据库对象引用
    CQtProDB* m_db;
    //参考值对话框对象
    RenfValue* m_RenfValueDlg;
    //导航栏字段内容
    QStringList m_ListContent;
    //查询界面对象
    QueryWindow* m_QueryWin;
    //测试界面对象
    TestWindow* m_TestWin;
    //射频串口
    QextSerialPort* m_RFSerialPort;
    //仪器串口
    QextSerialPort* m_HL7SerialPort;
//function
private:
    //语言选择
    void LanguageSel(int index);
    //更新语言选择
    void UpdateLanguage();
    //初始化左边列表框
    void InitListWidget();
    //更新导航栏内容
    void UpdateListContent();
    //根据配置文件更新控件状态
    void UpdateControl();
    //初始化参考值表单
    void InitRenfVTb();
    //初始化参考值表头名称
    void InitRenfHeader();
    //获取列表控件一行数据
    QStringList GetRowValue(QTableWidget *tw);
    //初始化设置界面时间控件
    void InitDateToSetUi();
    //设置系统时间
    void SetSysDateTime(QDate &date, QTime time);
    //
    //void changeEvent(QEvent *e);
    //隐藏功能区域
    void HideDebugFunc(bool bHide);
    //隐藏登陆区域
    void HideDebugLogin(bool bHide);
    //
    void InitRFCardInfoControl();
};

#endif // SETWINDOW_H
