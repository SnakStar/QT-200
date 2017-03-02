/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: upgrading.cpp
 *  简要描述: 软件升级功能
 *
 *  创建日期: 2016-10-14
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "upgrading.h"


Upgrading::Upgrading()
{
    HideHardUpdateControl(true);
    QueryUDisk();
    if(m_mapDiskName.size() == 0)
    {
        ui->btnOK->setEnabled(false);
    }
}

/********************************************************
 *@Name:        on_btnOK_clicked
 *@Author:      HuaT
 *@Description: 软件升级接口实现-实现确定按钮
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-10-14
********************************************************/
void Upgrading::on_btnOK_clicked()
{
    UpdateSoftware();
}

/********************************************************
 *@Name:        UpdateSoftware
 *@Author:      HuaT
 *@Description: 软件升级功能函数
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-10-14
********************************************************/
void Upgrading::UpdateSoftware()
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
    QString strMountDestDir = "/media/hdd";
    MountUDisk(strMountDvice,strMountDestDir);
    //更新文件
    QProcess pc;
    QString strSrcDir,strDestDir;
    strSrcDir = "/media/hdd/qt200";
    strDestDir = "/home/root";
    QString strUpdateCmd = QString("cp -r -f %1 %2").arg(strSrcDir).arg(strDestDir);//"cp -r -f /media/hdd/testqt200 /home/th/";
    pc.start(strUpdateCmd);
    pc.waitForFinished(-1);
    byteErr = pc.readAllStandardError();
    strErrInfo = byteErr.data();
    QString strTilter,strContain;
    if(strErrInfo.size() == 0){
        strTilter = QObject::tr("Note");
        strContain = QObject::tr("Update is successful,Need to manually restart the instrument to take effect");
    }else{
        strTilter = QObject::tr("Note");
        strContain = strErrInfo;
    }
    QMessageBox::information(this,strTilter,strContain,QMessageBox::Ok);
    //qDebug()<<strErrInfo<<strErrInfo.size();
    //QString strUMount = "echo th | sudo umount /media/hdd";
    //卸载U盘
    UMountUDisk("/media/hdd");
}
