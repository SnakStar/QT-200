/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: ExportData.cpp
 *  简要描述: 数据导出功能实现文件
 *
 *  创建日期: 2016-10-14
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "exportdata.h"

ExportData::ExportData(QVector<PatientInfo> PatientInfos):m_vecPatientInfo(PatientInfos)
{
    QString strTip = QObject::tr("The current total number of disk: %1").arg(m_mapDiskName.size());
    ui->lbUDiskTip->setText(strTip);
    QueryUDisk();
    if(m_mapDiskName.size() == 0)
    {
        ui->btnOK->setEnabled(false);
    }
}

/********************************************************
 *@Name:        on_btnOK_clicked
 *@Author:      HuaT
 *@Description: 软件数据导出接口实现-实现确定按钮
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-10-14
********************************************************/
void ExportData::on_btnOK_clicked()
{
    ExportQuery();
}

/********************************************************
 *@Name:        ExportQuery
 *@Author:      HuaT
 *@Description: 软件数据导出功能函数
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-10-14
********************************************************/
void ExportData::ExportQuery()
{
    QByteArray byteErr;
    QString strErrInfo;
    //qDebug()<<ui->lwUDiskInfo->currentRow();
    if(ui->lwUDiskInfo->currentRow() == -1){
        QString strTilte = QObject::tr("Note");
        QString strContain = QObject::tr("Please choose U disk");
        QMessageBox::information(this,strTilte,strContain,QMessageBox::Ok);
        return;
    }
    //挂载U盘
    QString strMountDvice = m_mapDiskName.value(ui->lwUDiskInfo->currentItem()->text());
    QString strMountDestPath= "/media/hdd";
    MountUDisk(strMountDvice,strMountDestPath);
    //数据导出
    QString strFileName;
    strFileName = QString("%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"));
    QString strFilePath = QString("/media/hdd/%1").arg(strFileName);
    QFile file(strFilePath);
    QString strTitle,strContain;
    strTitle = QObject::tr("Note");
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        //qDebug()<<"File is written to failure";
        strContain = QObject::tr("File is written to failure");
        QMessageBox::information(this,strTitle,strContain,QMessageBox::Ok);
        UMountUDisk(strMountDestPath);
        return;
    }
    QTextStream stream(&file);
    int nCount = m_vecPatientInfo.count();
    if(0 == nCount){
        if(file.exists()){
            file.remove();
        }
        strContain = QObject::tr("No query results available export");
        QMessageBox::information(this,strTitle,strContain,QMessageBox::Ok);
        UMountUDisk(strMountDestPath);
        return;
    }
    for(int n=0; n<m_vecPatientInfo.count(); n++){
        stream<<m_vecPatientInfo.at(n).m_nNumble<<","<<m_vecPatientInfo.at(n).m_strName<<","
             <<m_vecPatientInfo.at(n).m_nAge<<","<<m_vecPatientInfo.at(n).m_strSex<<","
            <<m_vecPatientInfo.at(n).m_strItems<<","<<m_vecPatientInfo.at(n).m_strResult<<","
           <<m_vecPatientInfo.at(n).m_strTestDate<<"\n";
    }
    file.close();
    strContain = QObject::tr("Data export is successful");
    QMessageBox::information(this,strTitle,strContain,QMessageBox::Ok);
    //卸载U盘
    UMountUDisk(strMountDestPath);
}
