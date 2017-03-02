/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: UDiskShow.h
 *  简要描述: 数据导出和软件升级界面声明文件
 *
 *  创建日期: 2016-10-14
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef UDISKSHOW_H
#define UDISKSHOW_H

#include <QDialog>
#include<QDebug>
#include<QMessageBox>
#include<QProcess>

namespace Ui {
class UDiskShow;
}

class UDiskShow : public QDialog
{
    Q_OBJECT

public:
    explicit UDiskShow(QWidget *parent = 0);
    ~UDiskShow();

protected slots:
    virtual void on_btnOK_clicked();

protected:
    virtual void ExportQuery();
    virtual void UpdateSoftware();
    virtual int QueryUDisk();
    //virtual void ShowUDiskLabel();

protected:
    bool MountUDisk(QString strDvice , QString strDestDir);
    bool UMountUDisk(QString strDestDir);
    void HideHardUpdateControl(bool bHide);


protected:
    Ui::UDiskShow *ui;
    QMap<QString,QString> m_mapDiskName;
private slots:
    virtual void on_btnRefresh_clicked();
};

#endif // UDISKSHOW_H
