/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: querywindow.h
 *  简要描述: 查询界面头文件，实现病人信息显示、打印等操作
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef QUERYWINDOW_H
#define QUERYWINDOW_H

#include <QWidget>
#include<QDate>
#include<QDebug>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QTextCodec>
#include"cchecktablewidget.h"
#include"cutilsettings.h"
//#include"udiskshow.h"
#include"exportdata.h"
#include"cqtprodb.h"
#include"csimplehl7.h"
#include"resultmodify.h"

namespace Ui {
class QueryWindow;
}

class QueryWindow : public QWidget
{
    Q_OBJECT

public:
    explicit QueryWindow(QWidget *parent = 0);
    ~QueryWindow();
public:
    void SetExportHide(bool bHide);
    void SetDbModifyFuncHide(bool bHide);
//function
private:
    //初始化时间控件
    void InitQueryDate();
    //初始化列表控件
    void InitPatientInfo();

private slots:
    void on_btnPrint_clicked();

    void on_btnExport_clicked();

    void on_btnPrePage_clicked();

    void on_btnNextPage_clicked();

    void on_btnModify_clicked();

    void on_btnDelete_clicked();

    void on_btnQueryData_clicked();

    void on_twPatientInfo_itemClicked(QTableWidgetItem *item);

    void on_btnPostback_clicked();

private:
    //
    void changeEvent(QEvent *e);
    //字节编码转换
    QByteArray Utf8ToGbk(QString str);

private:
    Ui::QueryWindow *ui;
    CCheckTableWidgetModel* m_CheckTableWidget;
    QSqlDatabase*           m_SqlDB;
    QSqlQuery*              m_Query;
    CQtProDB*               m_db;
    CUtilSettings*          m_settings;
    //配置文件参数表
    QMap<QString,QString>* m_SetParam;
    //打印串口对象
    QextSerialPort*        m_SerialPrint;
    //HL7串口对象
    QextSerialPort*        m_SerialHL7;
    //
    CSimpleHL7             m_SimpleHL7;

    int m_CurrentPage;
    int m_CountPage;
};

#endif // QUERYWINDOW_H
