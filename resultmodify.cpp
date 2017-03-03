/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: resultmodify.cpp
 *  简要描述: 实现文件，修改数据库中测试结果
 *
 *  创建日期: 2017-3-3
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "resultmodify.h"
#include "ui_resultmodify.h"

ResultModify::ResultModify(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultModify)
{
    ui->setupUi(this);
}

ResultModify::ResultModify(CQtProDB *db,QString strNumber, QString strTestDate,
                           QString strResult, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultModify)
{
    ui->setupUi(this);
    m_db = db;
    QueryWindow* QueryWin = (QueryWindow*)parent;
    ui->leRMNumber->setText(strNumber);
    ui->leRMTestDate->setText(strTestDate);
    ui->leRMOldResult->setText(strResult);

    connect(this,SIGNAL(accepted()),QueryWin,SLOT( on_btnQueryData_clicked())  );
    //QRegExp regExp("^(([0-9]+\.[0-9]*[1-9][0-9]*)|([0-9]*[1-9][0-9]*\.[0-9]+)|([0-9]*[1-9][0-9]*))$");
    //QRegExpValidator* regValidator = new QRegExpValidator(regExp);
    //ui->leRMNewResult->setValidator(regValidator);
}

ResultModify::~ResultModify()
{
    delete ui;
}

/********************************************************
 *@Name:        on_pbRMOK_clicked
 *@Author:      HuaT
 *@Description: 确定
 *@Param:       是否隐藏
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-11-9
********************************************************/
void ResultModify::on_pbRMOK_clicked()
{
    QString strTitle,strContain;
    strTitle = "提示";
    QStringList slistOldResult,slistNewResult;
    QString strOldResult = ui->leRMOldResult->text();
    slistOldResult = m_Settings.ParseTestUnit(strOldResult);
    QString strNewReuslt = ui->leRMNewResult->text();
    if(slistOldResult.size() == 0){
        //如果老的没有单位,则直接插入新输入的结果数据
        strNewReuslt = QString("%1").arg(ui->leRMNewResult->text());
    }else{
        //如果老的有单位
        slistNewResult = m_Settings.ParseTestUnit(strNewReuslt);
        if(slistNewResult.size() == 0){
            //新输入的没有单位,则自动添加老的单位给新数据
            strNewReuslt = QString("%1%2").arg(ui->leRMNewResult->text()).arg(slistOldResult.at(1));
        }
        //如果新输入的也有单位,则直接使用新的单位
        //strNewResult
    }
    QString strSql;
    strSql = QString("update patient set result='%1' where number=%2 and testdate='%3' ")
            .arg(strNewReuslt).arg(ui->leRMNumber->text()).arg(ui->leRMTestDate->text());
    if(m_db->Exec(strSql)){
        strContain = "修改成功";
        QMessageBox::information(this,strTitle,strContain,QMessageBox::Ok);
    }else{
        strContain = QString("修改失败:%1").arg(m_db->LastError());
        QMessageBox::critical(this,strTitle,strContain,QMessageBox::Ok);
    }
    this->accept();
}
