/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: qcvalue.cpp
 *  简要描述: 实现文件，实现质控品的上下限数据存储
 *
 *  创建日期: 2016-7-12
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "qcvalue.h"
#include "ui_qcvalue.h"
#include "qcwindow.h"

QCValue::QCValue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QCValue)
{
    ui->setupUi(this);
    QCWindow* QCWin = (QCWindow*)parent;
    m_Query = QCWin->GetSqlQuery();

    //添加浮点过滤
    QRegExp reg("^(([0-9]+\.[0-9]*[1-9][0-9]*)|([0-9]*[1-9][0-9]*\.[0-9]+)|([0-9]*[1-9][0-9]*))$");
    QRegExpValidator* regValidator = new QRegExpValidator(reg);
    ui->leUpper->setValidator(regValidator);
    ui->leLower->setValidator(regValidator);
    //
    ui->lbQCITEM->hide();
    ui->leQCITEM->hide();
}

QCValue::~QCValue()
{
    delete ui;
}

/********************************************************
 *@Name:        on_btnSave_clicked
 *@Author:      HuaT
 *@Description: 保存事件，保存批号上下限数据到数据库
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void QCValue::on_btnSave_clicked()
{
    QString msgTitle,msgContain;
    msgTitle = QObject::tr("Note");
    QString strItem,strBN,strUpper,strLower;
    strItem = ui->leQCITEM->text();
    strBN = ui->leQCBN->text();

    strUpper = ui->leUpper->text();
    strLower = ui->leLower->text();
    if(strUpper.toFloat()<strLower.toFloat()){
        msgContain = QObject::tr("Upper limit cannot be less than the lower limit value");
        QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Ok);
        return;
    }
    if(strItem.isEmpty()){
        strItem = "crp";
    }
    if(strBN.isEmpty()){
        msgContain = QObject::tr("QC Batch number can't be empty");
        QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Ok);
        return;
    }
    if(strUpper.isEmpty()){
        msgContain = QObject::tr("Upper Limit can't be empty");
        QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Ok);
        return;
    }
    if(strLower.isEmpty()){
        msgContain = QObject::tr("Lower Limit can't be empty");
        QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Ok);
        return;
    }
    QString sqlCmd;
    sqlCmd = QString("insert into qcrenfvalue values(null,'%1','%2','%3','%4',datetime('now','localtime'))")
            .arg(strItem).arg(strBN).arg(strUpper).arg(strLower);
    if(!m_Query->exec(sqlCmd)){
        qDebug()<<"qc renf value insert to faile";
    }
    emit UpdateComboBoxMsg();
    msgContain = QObject::tr("Save successful");
    QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Ok);
    this->done(0);

}
