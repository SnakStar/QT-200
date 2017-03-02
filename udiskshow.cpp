/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: UDiskShow.cpp
 *  简要描述: 数据导出和软件升级界面实现文件
 *
 *  创建日期: 2016-10-14
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "udiskshow.h"
#include "ui_udiskshow.h"

UDiskShow::UDiskShow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UDiskShow)
{
    ui->setupUi(this);
    QString strTip = QObject::tr("The current total number of disk: %1").arg(m_mapDiskName.size());
    ui->lbUDiskTip->setText(strTip);
}

UDiskShow::~UDiskShow()
{
    delete ui;
    m_mapDiskName.clear();
}

void UDiskShow::on_btnOK_clicked()
{
}

/********************************************************
 *@Name:        ExportQuery
 *@Author:      HuaT
 *@Description: 数据导出接口虚函数
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-10-14
********************************************************/
void UDiskShow::ExportQuery()
{
}

/********************************************************
 *@Name:        UpdateSoftware
 *@Author:      HuaT
 *@Description: 软件更新接口虚函数
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-10-14
********************************************************/
void UDiskShow::UpdateSoftware()
{
}

/********************************************************
 *@Name:        QueryUDisk
 *@Author:      HuaT
 *@Description: 检测U盘，并进行列表显示
 *@Param:       无
 *@Return:      返回检测到的磁盘数量
 *@Version:     1.0
 *@Date:        2016-10-14
********************************************************/
int UDiskShow::QueryUDisk()
{
    ui->lwUDiskInfo->clear();
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
        ui->lwUDiskInfo->addItem(strDiskID.at(1));
        strDiskID.clear();
    }
    return m_mapDiskName.size();
}

/********************************************************
 *@Name:        MountUDisk
 *@Author:      HuaT
 *@Description: 挂载U盘
 *@Param1:       U盘的设备路径
 * @Param2:    需要挂载的U盘路径
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-10-14
********************************************************/
bool UDiskShow::MountUDisk(QString strDvice, QString strDestDir)
{
    QString strErrInfo;
    QByteArray byteErr;
    //QString strMount = QString("echo th|sudo -S mount %1 %2").arg(strDvice).arg(strDestDir);
    QString strMount = QString("echo th|mount %1 %2").arg(strDvice).arg(strDestDir);
    QProcess pc;
    pc.start("/bin/bash", QStringList()<<"-c"<<strMount);
    pc.waitForFinished(-1);
    byteErr = pc.readAllStandardError();
    strErrInfo = byteErr.data();
    //qDebug()<<strErrInfo;  //output : "[sudo] password for th: " 24
    if(strErrInfo.size() == 24){
        return true;
    }else{
        return false;
    }
}

/********************************************************
 *@Name:        UMountUDisk
 *@Author:      HuaT
 *@Description: 挂载U盘
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-10-14
********************************************************/
bool UDiskShow::UMountUDisk(QString strDestDir)
{
    QString strErrInfo;
    QByteArray byteErr;
    QProcess pc;
    //QString strUMount = QString("echo th | sudo -S umount %1").arg(strDestDir);
    QString strUMount = QString("echo th |umount %1").arg(strDestDir);
    pc.start("/bin/bash",QStringList()<<"-c"<<strUMount);
    pc.waitForFinished(-1);
    byteErr = pc.readAllStandardError();
    strErrInfo = byteErr.data();
    //qDebug()<<strErrInfo;  //output : "[sudo] password for th: " 24
    if(strErrInfo.size() == 24){
        return true;
    }else{
        return false;
    }
}

/********************************************************
 *@Name:        HideHardUpdateInfo
 *@Author:      HuaT
 *@Description: 隐藏固件升级需要的控件
 *@Param:       是否隐藏
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-10-14
********************************************************/
void UDiskShow::HideHardUpdateControl(bool bHide)
{
    ui->lbUDiskTag->setHidden(bHide);
    ui->cBoxUDiskTag->setHidden(bHide);
    ui->pBarUDiskHardPB->setHidden(bHide);
}

/********************************************************
 *@Name:        on_btnRefresh_clicked
 *@Author:      HuaT
 *@Description: 更新U盘列表框
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-10-14
********************************************************/
void UDiskShow::on_btnRefresh_clicked()
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
