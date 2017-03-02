/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: SetTimer.cpp
 *  简要描述: 计时器实现文件，实现多个计时器的倒计时功能
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "settimer.h"
#include "ui_settimer.h"
SetTimerObj::SetTimerObj(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetTimerObj)
{
    ui->setupUi(this);
    m_TimeSN = 0;
    ui->leCustomDate->clear();
    ui->leCustomDate->setValidator(new QIntValidator(3,100,this));
}

SetTimerObj::~SetTimerObj()
{
    delete ui;
}

/********************************************************
 *@Name:SetTimeSn
 *@Author:HuaT
 *@Description:设置需要记时的按钮号
 *@Param: 按钮的按钮号，范围值为1，2，3
 *@Return:无
 *@VeSetTimerDlg0
 *@Date:2016-6-3
********************************************************/
void SetTimerObj::SetTimeSn(int Sn)
{
    if(Sn != 1 && Sn != 2 && Sn != 3){
        return;
    }
    m_TimeSN = Sn;
}

/********************************************************
 *@Name:ClearLineEdit
 *@Author:HuaT
 *@Description:清除时间编辑框里的输入数据
 *@Param: 无
 *@Return:无
 *@Version:1.0
 *@Date:2016-8-18
********************************************************/
void SetTimerObj::ClearLineEdit()
{
    ui->leCustomDate->clear();
}

/********************************************************
 *@Name:on_btnOK_clicked
 *@Author:HuaT
 *@Description:确定按钮单击事件
 *@Param: 无
 *@Return:无
 *@Version:1.0
 *@Date:2016-5-13
********************************************************/
void SetTimerObj::on_btnOK_clicked()
{

    if(ui->leCustomDate->text().isEmpty()){
        QString msg = QObject::tr("Input time cannot be empty");
        QMessageBox::warning(this,QObject::tr("Warning"),msg,QMessageBox::Ok);
        return;
    }
    QString time = ui->leCustomDate->text();
    emit CoutDown(time.toInt(0),m_TimeSN);
    done(0);

}

/********************************************************
 *@Name:on_btn3Min_clicked
 *@AuSetTimerDlgT
 *@Description:3分钟按钮单击事件
 *@Param:无
 *@Return:无
 *@Version:1.0
 *@Date:2016-5-13
********************************************************/
void SetTimerObj::on_btn3Min_clicked()
{
    qDebug()<<m_TimeSN;
    emit CoutDown(3,m_TimeSN);
    done(0);
}

/****SetTimerDlg********************************************
 *@Name:on_btn5Min_clicked
 *@Author:HuaT
 *@Description:5分钟按钮单击事件
 *@Param:无
 *@Return:无
 *@Version:1.0
 *@Date:2016-5-13
********************************************************/
void SetTimerObj::on_btn5Min_clicked()
{
    emit CoutDown(5,m_TimeSN);
    done(0);
}

/*SetTimerDlg***********************************************
 *@Name:on_btn9Min_clicked
 *@Author:HuaT
 *@Description:9分钟按钮单击事件
 *@Param:无
 *@Return:无
 *@Version:1.0
 *@Date:2016-5-13
********************************************************/
void SetTimerObj::on_btn10Min_clicked()
{
    emit CoutDown(10,m_TimeSN);
    done(0);
}

/********************************************************
 *@Name:on_btn15Min_clicked
 *@Author:HuaT
 *@Description:15分钟按钮单击事件
 *@Param:无
 *@Return:无
 *@Version:1.0
 *@Date:2016-5-13
********************************************************/
void SetTimerObj::on_btn15Min_clicked()
{
    emit CoutDown(15,m_TimeSN);
    done(0);
}

/********************************************************
 *@Name:        changeEvent
 *@Author:      HuaT
 *@Description: 语言事件处理
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-20
********************************************************/
void SetTimerObj::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
