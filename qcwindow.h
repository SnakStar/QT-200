/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: qcwindow.h
 *  简要描述: 头文件，实现质控品的图形显示与存储
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef QCWINDOW_H
#define QCWINDOW_H

#include <QWidget>
#include<QMessageBox>
#include<QDebug>
#include<QMap>
#include<QSqlQuery>
#include"cutilsettings.h"
#ifdef Q_OS_LINUX
#include"canbus.h"
#endif
#include"qcvalue.h"

namespace Ui {
class QCWindow;
}

class QCWindow : public QWidget
{
    Q_OBJECT

public:
    explicit QCWindow(QWidget *parent = 0);
    ~QCWindow();
public:
    QSqlQuery* GetSqlQuery();
private slots:
    void on_btnQCSetting_clicked();
    //初始化批号下拉框
    void InitBatchNoComboBox(QString strItem);
    //初始化项目下拉框
    void InitItemComboBox();
    //更新下拉框
    void UpdateComboBox();
    //更新结果数据集
    void UpdateResultDataRecord();
    //设置画图数据
    void SetCurveDataAddDraw(QString strItem,QString strBatchNo);
    //更新质控结果数据
    void UpdateResult(QString strResult);
    //更新控件状态
    void UpdateControlStatus();
    //void on_cBoxBatchNum_currentIndexChanged(const QString &arg1);
    void on_btnQcTest_clicked();


    void on_cBoxBatchNum_activated(const QString &arg1);

    void on_cBoxItem_activated(const QString &arg1);

private:
    //
    void changeEvent(QEvent *e);
    //绘图函数
    void DrawGraph(QVector<double> keys,QVector<double> values);
    //画参考值线
    void DrawRenfValue(QString strItem,QString strBatchNo);
    //设置质控测试相关控件状态
    void SetControlStatus(bool bStatus);
private:
    Ui::QCWindow *ui;
    //数据库查询对象
    QSqlQuery* m_Query;
    //质控参考值对象
    QCValue* m_pQCValue;
    //批号结果映射表
    QMap<QString,QString> m_mapResultData;
    //配置文件对象
    CUtilSettings* m_settings;
    //配置文件参数表
    QMap<QString,QString>* m_SetParam;
    //
#ifdef Q_OS_LINUX
    CanBus* m_CanBus;
#endif
    QWidget* m_ParentWidget;
    //通道1运行状态
    bool m_bChannel1Status;
    //通道2运行状态
    bool m_bChannel2Status;
    //通道3运行状态
    bool m_bChannel3Status;
    QVector<double> m_PointX;
    QVector<double> m_PointY;


};

#endif // QCWINDOW_H
