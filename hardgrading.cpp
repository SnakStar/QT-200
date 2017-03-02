/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: HardGrading.cpp
 *  简要描述: 固件升级功能
 *
 *  创建日期: 2016-11-16
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/

#include "hardgrading.h"
#include"ui_udiskshow.h"
#include"mainwindow.h"

HardGrading::HardGrading(QObject *parent)
{
    HideHardUpdateControl(false);
    m_MainWin = NULL;
    ui->pBarUDiskHardPB->setValue(0);
    QueryUDisk();
    if(m_mapDiskName.size() == 0)
    {
        ui->btnOK->setEnabled(false);
    }
}

HardGrading::~HardGrading()
{
    m_MainWin = NULL;
}

/********************************************************
 *@Name:        SetMainWin
 *@Author:      HuaT
 *@Description: 设置主窗口对象
 *@Param:       主窗口对象指针
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-11-17
********************************************************/
void HardGrading::SetMainWin(QObject *obj)
{
    m_MainWin = obj;
}

/********************************************************
 *@Name:        SetProgressValue
 *@Author:      HuaT
 *@Description: 设置进度条当前值
 *@Param:       当前值
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-11-23
********************************************************/
void HardGrading::SetProgressValue(quint32 nValue)
{
    ui->pBarUDiskHardPB->setValue(nValue);
}

/********************************************************
 *@Name:        SetProgressRange
 *@Author:      HuaT
 *@Description: 设置进度条当前范围
 *@Param1:      最小值
 *@Param2:      最大值
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-11-23
********************************************************/
void HardGrading::SetProgressRange(quint32 nMin, quint32 nMax)
{
    ui->pBarUDiskHardPB->setRange(nMin, nMax);
}

/********************************************************
 *@Name:        on_btnOK_clicked
 *@Author:      HuaT
 *@Description: 固件升级接口实现-实现确定按钮
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-11-16
********************************************************/
void HardGrading::on_btnOK_clicked()
{
    UpdateHardWare();
}

/********************************************************
 *@Name:        on_btnRefresh_clicked
 *@Author:      HuaT
 *@Description: 刷新按钮事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-11-16
********************************************************/
void HardGrading::on_btnRefresh_clicked()
{
    int nSize = QueryUDisk();
    if(nSize != 0){
        ui->lbUDiskTip->setText(QObject::tr("The current total number of disk: %1").arg(nSize));
        ui->btnOK->setEnabled(true);
    }else{
        ui->lbUDiskTip->setText(QObject::tr("The current total number of disk: 0"));
        ui->btnOK->setEnabled(false);
    }
}

/********************************************************
 *@Name:        UpdateHardWare
 *@Author:      HuaT
 *@Description: 软件升级接口实现-实现确定按钮
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-10-14
********************************************************/
void HardGrading::UpdateHardWare()
{
    if(ui->lwUDiskInfo->currentRow() == -1){
        QString strTilte = QObject::tr("Note");
        QString strContain = QObject::tr("Please choose Update File");
        QMessageBox::information(this,strTilte,strContain,QMessageBox::Ok);
        return;
    }
    //挂载U盘
     QString strMountDvice = m_mapDiskName.value(ui->cBoxUDiskTag->currentText());
    QString strMountDestDir = "/media/hdd";
    MountUDisk(strMountDvice,strMountDestDir);
    //读取文件
    QString strFileName = QString("%1/%2").arg(strMountDestDir).arg(ui->lwUDiskInfo->currentItem()->text());
    QByteArray byteResult;
    byteResult = ReadFileData(strFileName);
    //卸载U盘
    UMountUDisk(strMountDestDir);
    //设置发送数据
    ((MainWindow*)m_MainWin)->SetHardUpdateData(byteResult,this);
}

/********************************************************
 *@Name:        QueryUDisk
 *@Author:      HuaT
 *@Description: 查询U盘并显示可升级文件目录
 *@Param:       无
 *@Return:      U盘个数
 *@Version:     1.0
 *@Date:        2016-11-17
********************************************************/
int HardGrading::QueryUDisk()
{
    ui->lwUDiskInfo->clear();
    ui->cBoxUDiskTag->clear();
    m_mapDiskName.clear();
    QProcess proc;
    QByteArray byteResult ;
    proc.start("/bin/bash",QStringList()<<"-c"<<"blkid -s LABEL|grep '/dev/sd*' ");
    //proc.waitForFinished(-1);
    //byteResult = proc.readAll();
    if(proc.waitForStarted(-1)){
        while(proc.waitForReadyRead(-1)){
            byteResult += proc.readAllStandardOutput();
            //byteError += proc.readAllStandardError();
        }
    }
    QString strUDisk = byteResult.data();
    QStringList strlistUdisk = strUDisk.split('\n');
    QStringList strDiskID;
    for(int n=0; n<strlistUdisk.size()-1; n++){
        strDiskID = strlistUdisk.at(n).split(":");
        m_mapDiskName[strDiskID.at(1)] = strDiskID.at(0);
        //ui->lwUDiskInfo->addItem(strDiskID.at(1));
        ui->cBoxUDiskTag->addItem(strDiskID.at(1));
        strDiskID.clear();
    }
    if(ui->cBoxUDiskTag->count() == 0){
        return 0;
    }else{
        ui->cBoxUDiskTag->setCurrentIndex(0);
    }
    ShowHardFile();
    return m_mapDiskName.size();
}


/********************************************************
 *@Name:        ShowHardFile
 *@Author:      HuaT
 *@Description: 显示可升级文件清单
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-11-17
********************************************************/
void HardGrading::ShowHardFile()
{
    if(ui->cBoxUDiskTag->currentText().isEmpty()){
        return;
    }
    //挂载U盘
    QString strMountDvice = m_mapDiskName.value(ui->cBoxUDiskTag->currentText());
    QString strMountDestDir = "/media/hdd";
    MountUDisk(strMountDvice,strMountDestDir);
    //
    QDir hardfile("/media/hdd");
    if(!hardfile.exists()){
        return;
    }
    QStringList filters;
    filters<<QString("*.bin");
    hardfile.setNameFilters(filters);
    foreach(QFileInfo mfi ,hardfile.entryInfoList())
    {
        if(mfi.isFile())
        {
            //qDebug()<< "File :" << mfi.filePath();
            ui->lwUDiskInfo->addItem(mfi.fileName());
        }else
        {
            if(mfi.fileName()=="." || mfi.fileName() == "..")
                continue;
            /* qDebug() << "Entry Dir" << mfi.absoluteFilePath(); */
        }
    }
    //卸载U盘
    UMountUDisk("/media/hdd");
}

/********************************************************
 *@Name:        ShowHardFile
 *@Author:      HuaT
 *@Description: 读取指定升级文件数据
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-10-14
********************************************************/
QByteArray HardGrading::ReadFileData(QString strFilePath)
{
    QFile file(strFilePath);
    QByteArray byteResult;
    if(file.open(QIODevice::ReadOnly)){
        byteResult = file.readAll();
    }
    //qDebug()<<strResult;
    return byteResult;
}
