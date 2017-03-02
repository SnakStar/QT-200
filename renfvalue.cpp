/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: renfvalue.cpp
 *  简要描述: 参考值设置实现文件，实现指定项目参考值的设置
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "renfvalue.h"
#include "ui_renfvalue.h"

RenfValue::RenfValue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenfValue)
{
    ui->setupUi(this);
    //InitItemControl();
    InitSexControl();
    //WindowSystemMenuHint
    //this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint);
    ui->leAgeLow->setValidator(new QIntValidator(0,150,this));
    ui->leAgeHight->setValidator(new QIntValidator(0,150,this));
}

/********************************************************
 *@Name:        InitSexControl
 *@Author:      HuaT
 *@Description: InitSexControl
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-20
********************************************************/
void RenfValue::InitSexControl()
{
    QStringList SexList;
    SexList<<QObject::tr("Male/Female")<<QObject::tr("Male")<<QObject::tr("Female");
    ui->cBoxSex->addItems(SexList);
}

/********************************************************
 *@Name:        InitSexControl
 *@Author:      HuaT
 *@Description: InitSexControl
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-20
********************************************************/
void RenfValue::InitItemControl(RenfInfo &ri)
{
    if(0 == ri.m_listItem.size()){
        return;
    }else{
        ui->cBoxItem->addItems(ri.m_listItem);
    }
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
void RenfValue::changeEvent(QEvent *e)
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


/********************************************************
 *@Name:        RenfValue
 *@Author:      HuaT
 *@Description: 构造函数，如果为修改参考值选项，则自动帮助用户填写需要修改的信息
 *@Param1:      参考值信息
 *@Param2:      父窗口对象
 *@Return:
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
RenfValue::RenfValue(RenfInfo &ri, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenfValue)
{
    ui->setupUi(this);
    InitItemControl(ri);
    InitSexControl();
    //this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint);

    //项目名称
    //ui->leItem->setText(ri.m_Item);
    ui->cBoxItem->setCurrentIndex(ui->cBoxItem->findText(ri.m_Item));
    //值
    QStringList valueList;
    valueList = ParseValue(ri.m_Value);
    ui->leRenfLow->setText(valueList.at(0));
    ui->leRenfHigh->setText(valueList.at(1));
    //年龄
    ui->leAgeLow->setText(ri.m_AgeLow);
    ui->leAgeHight->setText(ri.m_AgeHight);
    //性别
    if(ri.m_Sex == QObject::tr("Female")){
        //qDebug()<<" ok female";
        ui->cBoxSex->setCurrentIndex(2);
    }else if(ri.m_Sex == QObject::tr("Male")){
        //qDebug()<<" ok male";
        ui->cBoxSex->setCurrentIndex(1);
    }else{
        ui->cBoxSex->setCurrentIndex(0);
    }
}

/********************************************************
 *@Name:        SetShowMode
 *@Author:      HuaT
 *@Description: 用来识别是新增参考值信息还是修改参考值信息
 *@Param:       分辨的标识，true,false
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void RenfValue::SetShowMode(bool isAddObj)
{
    m_isAddObj = isAddObj;
}


RenfValue::~RenfValue()
{
    delete ui;
}

/********************************************************
 *@Name:        ProcValue
 *@Author:      HuaT
 *@Description: 处理参考值上下限的格式
 *@Param1:       参考值下限
 *@Param2:      参考值上限
 *@Return:      格式化的参考值，不符合要求返回空值
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
QString RenfValue::ProcValue(QString lower, QString upper)
{
    QString FormatValue = "";
    if(lower.isEmpty() && upper.isEmpty()){
        return FormatValue;
    }
    if(lower.isEmpty()){
        FormatValue = QString("<%1").arg(upper);
    }else if(upper.isEmpty()){
        FormatValue = QString(">%1").arg(lower);
    }else{
        FormatValue = QString("%1-%2").arg(lower).arg(upper);
    }
    return FormatValue;
}


/********************************************************
 *@Name:        on_btnOK_clicked
 *@Author:      HuaT
 *@Description: 保存按钮事件
 *@Param:
 *@Return:
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void RenfValue::on_btnOK_clicked()
{
    //m_RenfInfo.m_Item = ui->leItem->text();
    m_RenfInfo.m_Item = ui->cBoxItem->currentText();
    QString msg;
    QString msgTitle = QString("Note");
    //项目名称
    if(m_RenfInfo.m_Item.isEmpty()){
        msg = QObject::tr("Item name cannnot be empty");
        QMessageBox::information(this,msgTitle,msg,QMessageBox::Ok);
        return;
    }
    QString upper = ui->leRenfHigh->text();
    QString lower = ui->leRenfLow->text();
    if(upper.isEmpty() && lower.isEmpty()){
        msg = QObject::tr("Renf Upper limit and lower limit value can't be empty!");
        QMessageBox::information(this,msgTitle,msg,QMessageBox::Ok);
        return;
    }
    //参考值
    m_RenfInfo.m_Value = ProcValue(lower,upper);
    //年龄
    QString strAgeLow = ui->leAgeLow->text();
    QString strAgeHight = ui->leAgeHight->text();
    m_RenfInfo.m_AgeLow = strAgeLow;
    m_RenfInfo.m_AgeHight = strAgeHight;
    //性别
    m_RenfInfo.m_Sex = ui->cBoxSex->currentText();
    msg = QObject::tr("Save successful");
    QMessageBox::information(this,msgTitle,msg,QMessageBox::Ok);
    if(m_isAddObj){
        emit RenfInfoAddComplete(m_RenfInfo);
    }else{
        emit RenfInfoModifyComplete(m_RenfInfo);
    }
    this->done(0);
}

/********************************************************
 *@Name:        on_btnCancel_clicked
 *@Author:      HuaT
 *@Description: 取消按钮事件
 *@Param:
 *@Return:
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void RenfValue::on_btnCancel_clicked()
{
    this->close();
}

/********************************************************
 *@Name:        ParseValue
 *@Author:      HuaT
 *@Description: 解析参考值
 *@Param:       带符号的参考值
 *@Return:      返回下限、上限值，返回格式固定为：
 *              0：下限，没有则插入空
 *              1：上限，没有则插入空
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
QStringList RenfValue::ParseValue(QString value)
{
    QStringList valueList;
    int index = -1;
    if(value.indexOf("<") != -1){
        index = value.indexOf("<");
        valueList.append("");
        valueList.append(value.mid(index+1));
    }else if(value.indexOf(">") != -1){
        index = value.indexOf(">");
        valueList.append(value.mid(index+1));
        valueList.append("");
    }else if(value.indexOf("-") != -1){
        index = value.indexOf("-");
        valueList.append(value.mid(0,index));
        valueList.append(value.mid(index+1));
    }else{
        valueList.append("");
        valueList.append("");
    }
    return valueList;
}


