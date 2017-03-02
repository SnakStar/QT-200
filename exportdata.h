/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: ExportData.h
 *  简要描述: 数据导出功能头文件
 *
 *  创建日期: 2016-10-14
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef EXPORTDATA_H
#define EXPORTDATA_H
#include<QFile>
#include<QDateTime>
#include"udiskshow.h"
#include"ui_udiskshow.h"
#include"cchecktablewidget.h"

class ExportData : public UDiskShow
{
public:
    ExportData(QVector<PatientInfo>  PatientInfos);
private slots:
    //确定按钮事件
    void on_btnOK_clicked();
private:
    QVector<PatientInfo> m_vecPatientInfo;
protected:
    //数据导出功能实现函数
    void ExportQuery();
};

#endif // EXPORTDATA_H
