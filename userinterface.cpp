/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: userinterface.cpp
 *  简要描述: 软件调试主界面实现文件
 *
 *  创建日期: 2017-01-11
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "userinterface.h"
#include "ui_userinterface.h"

UserInterface::UserInterface(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserInterface)
{
    ui->setupUi(this);
}

UserInterface::UserInterface(QWidget *parent,CQtProDB* db, CUtilSettings* settings) :
    QDialog(parent),
    ui(new Ui::UserInterface)
{
    ui->setupUi(this);
    //
    //this->setWindowFlags(Qt::FramelessWindowHint);
    //this->move(0,0);
    //初始化成员变量
    m_db = db;
    m_settings = settings;
    m_SetParam = m_settings->ReadSettingsInfoToMap();
    //初始化接口导航列表
    InitListControl();
    //初始化radio控件
    InitRadioControl();
    //初始化偏移量参数
    InitAPOffSetShowValue();
    //连接列表控件和栈控件
    connect(ui->lwNavBar,SIGNAL(currentRowChanged(int)) , ui->stackedWidget, SLOT(setCurrentIndex(int)) );
    //根据配置文件更新控件
    //UpdateControl();
}

UserInterface::~UserInterface()
{
    delete ui;
}

/********************************************************
 *@Name:        UpdateControl
 *@Author:      HuaT
 *@Description: 从更新的参数列表中，根据配置参数，让控件显示对应值
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-1-11
********************************************************/
void UserInterface::UpdateControl()
{

}

/********************************************************
 *@Name:        InitListControl
 *@Author:      HuaT
 *@Description: 初始化列表导航栏控件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-11
********************************************************/
void UserInterface::InitListControl()
{
    QStringList listContent;
    listContent.append("常规接口");
    listContent.append("数据库接口");
    listContent.append("日志接口");
    for(int n=0; n<listContent.size(); n++){
        QListWidgetItem *lwItem = new QListWidgetItem;
        lwItem->setSizeHint(QSize(140,50));
        lwItem->setText(listContent.at(n));
        ui->lwNavBar->insertItem(n,lwItem);
    }
}

/********************************************************
 *@Name:        InitRadioControl
 *@Author:      HuaT
 *@Description: 初始化Radio控件状态
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-11
********************************************************/
void UserInterface::InitRadioControl()
{
    //更新老化控件
    if(m_SetParam->contains(AGINGMODE)){
        int nAgingMode = m_SetParam->value(AGINGMODE).toInt();
        if(0 == nAgingMode){
            ui->rbAgingClose->setChecked(true);
        }else{
            ui->rbAgingOpen->setChecked(true);
        }
    }else{
        ui->rbAgingClose->setChecked(true);
    }
    //更新老化间隔时间
    ui->leAgingTime->setText(m_SetParam->value(AGINGTIME));
    //更新原始数据控件
    if(m_SetParam->contains(RAWDATAMODE)){
        int nRawData = m_SetParam->value(RAWDATAMODE).toInt();
        if(0 == nRawData){
            ui->rbRawDataClose->setChecked(true);
        }else{
            ui->rbRawDataOpen->setChecked(true);
        }
    }else{
        ui->rbRawDataClose->setCheckable(true);
    }
}

/********************************************************
 *@Name:        InitAPOffSetShowValue
 *@Author:      HuaT
 *@Description: 初始化放大偏移量控件存储值
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-15
********************************************************/
void UserInterface::InitAPOffSetShowValue()
{
    ui->leAPOffSet1->setText((m_SetParam->value(APOFFSET1)) );
    ui->leAPOffSet2->setText((m_SetParam->value(APOFFSET2)) );
    ui->leAPOffSet3->setText((m_SetParam->value(APOFFSET3)) );
}

/********************************************************
 *@Name:        on_btnUIOK_clicked
 *@Author:      HuaT
 *@Description: 常规接口-确定事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-11
********************************************************/
void UserInterface::on_btnUIOK_clicked()
{
    //老化
    if(ui->rbAgingClose->isChecked()){
        m_settings->SetParam(AGINGMODE,"0");
    }else{
        m_settings->SetParam(AGINGMODE,"1");
    }
    //老化时间
    m_settings->SetParam(AGINGTIME,ui->leAgingTime->text());
    //原始数据
    if(ui->rbRawDataClose->isChecked()){
        m_settings->SetParam(RAWDATAMODE,"0");
    }else{
        m_settings->SetParam(RAWDATAMODE,"1");
    }
    //写入数据至配置文件
    m_settings->WriteSettingsInfoToMap();
    //
    QString strOptTitle = QObject::tr("operating info");
    QString strOptContent = QObject::tr("Configuration saved successfully");
    QMessageBox::information(this,strOptTitle,strOptContent,QMessageBox::Ok);
}


/********************************************************
 *@Name:        on_btnStopAging_clicked
 *@Author:      HuaT
 *@Description: 常规接口-停止老化事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-11
********************************************************/
void UserInterface::on_btnStopAging_clicked()
{
    emit StopAgingTest();
}

/********************************************************
 *@Name:        SetCanBusObj
 *@Author:      HuaT
 *@Description: 设置Can通讯对象
 *@Param:       Can对象
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-15
********************************************************/
#ifdef Q_OS_LINUX
void UserInterface::SetCanBusObj(CanBus* cb)
{
    m_CanBus = cb;
}
#endif

/********************************************************
 *@Name:        on_btnUIClose_clicked
 *@Author:      HuaT
 *@Description: 退出调试对话框
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-15
********************************************************/
void UserInterface::on_btnUIClose_clicked()
{
    this->reject();
}

/********************************************************
 *@Name:        on_btnAPOffSet1_clicked
 *@Author:      HuaT
 *@Description: 发送偏移量大小给通道1
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-15
********************************************************/
void UserInterface::on_btnAPOffSet1_clicked()
{
#ifdef Q_OS_LINUX
    QByteArray byteID,byteData;
    byteID[0] = QT_TESTOPT_SETAPOFFSET;
    byteID[1] = 0x00;
    byteID[2] = QT_HOST_MASTER;
    byteID[3] = QT_HOST_SLAVE1;
    byteData[0] = ui->leAPOffSet1->text().toInt();
    m_CanBus->SendMsg(byteID,byteData.size(),byteData);
#endif
    m_settings->SetParam(APOFFSET1,ui->leAPOffSet1->text());
    //写入数据至配置文件
    m_settings->WriteSettingsInfoToMap();
}

/********************************************************
 *@Name:        on_btnAPOffSet2_clicked
 *@Author:      HuaT
 *@Description: 发送偏移量大小给通道2
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-15
********************************************************/
void UserInterface::on_btnAPOffSet2_clicked()
{
#ifdef Q_OS_LINUX
    QByteArray byteID,byteData;
    byteID[0] = QT_TESTOPT_SETAPOFFSET;
    byteID[1] = 0x00;
    byteID[2] = QT_HOST_MASTER;
    byteID[3] = QT_HOST_SLAVE2;
    byteData[0] = ui->leAPOffSet2->text().toInt();
    m_CanBus->SendMsg(byteID,byteData.size(),byteData);
#endif
    m_settings->SetParam(APOFFSET2,ui->leAPOffSet2->text());
    //写入数据至配置文件
    m_settings->WriteSettingsInfoToMap();
}

/********************************************************
 *@Name:        on_btnAPOffSet3_clicked
 *@Author:      HuaT
 *@Description: 发送偏移量大小给通道3
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-15
********************************************************/
void UserInterface::on_btnAPOffSet3_clicked()
{
#ifdef Q_OS_LINUX
    QByteArray byteID,byteData;
    byteID[0] = QT_TESTOPT_SETAPOFFSET;
    byteID[1] = 0x00;
    byteID[2] = QT_HOST_MASTER;
    byteID[3] = QT_HOST_SLAVE3;
    byteData[0] = ui->leAPOffSet3->text().toInt();
    m_CanBus->SendMsg(byteID,byteData.size(),byteData);
#endif
    m_settings->SetParam(APOFFSET3,ui->leAPOffSet3->text());
    //写入数据至配置文件
    m_settings->WriteSettingsInfoToMap();
}

/********************************************************
 *@Name:        on_btnClearDb_clicked
 *@Author:      HuaT
 *@Description: 删除数据库事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-16
********************************************************/
void UserInterface::on_btnClearDb_clicked()
{
    QString strTitle,strContent;
    QByteArray byteError;
    QString strError;
    QString strDbPath = "/home/root/qt200/qtdb.db";
    QString strCMD = QString("rm %1").arg(strDbPath);
    QProcess pc;
    strContent = "删除数据库后不能恢复,是否继续";
    if(QMessageBox::Ok == QMessageBox::information(this,strTitle,strContent,QMessageBox::Ok|QMessageBox::Cancel)){
        pc.start(strCMD);
        pc.waitForFinished(-1);
        byteError = pc.readAllStandardError();
        strError = byteError.data();
        strTitle = "提示";
        if(strError.size() == 0){
            QMessageBox::information(this,strTitle,"数据库删除成功",QMessageBox::Ok);
        }else{
            QMessageBox::information(this,strTitle,strError,QMessageBox::Ok);
        }
    }
}

/********************************************************
 *@Name:        on_btnCopyDb_clicked
 *@Author:      HuaT
 *@Description: 复制数据库事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-16
********************************************************/
void UserInterface::on_btnCopyDb_clicked()
{

}
