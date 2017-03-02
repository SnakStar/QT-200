/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: renfvalue.h
 *  简要描述: 参考值设置头文件，实现指定项目参考值的设置
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/

#ifndef RENFVALUE_H
#define RENFVALUE_H


#include <QDialog>
#include<QMessageBox>
#include<QDebug>

namespace Ui {
class RenfValue;
}

struct RenfInfo{
    QString m_Item;
    //已经刷过ID卡的项目名称，填充供用户选择
    QStringList m_listItem;
    QString m_Value;
    QString m_AgeLow;
    QString m_AgeHight;
    QString m_Sex;
};

class RenfValue : public QDialog
{
    Q_OBJECT
    
public:
    explicit RenfValue(QWidget *parent = 0);
    explicit RenfValue(RenfInfo& ri,QWidget *parent = 0);
    //识别参考值完成后应发增加信号还是修改信号
    void SetShowMode(bool isAddOjb);
    //处理上下限的值，返回需要的格式
    QString ProcValue(QString lower,QString upper);
    //解析传入的结果值，处理成上下限的格式，0为下限，1为上限
    QStringList ParseValue(QString value);
    ~RenfValue();
    
private slots:
    //保存
    void on_btnOK_clicked();
    //取消
    void on_btnCancel_clicked();
signals:
    //添加参考值
    void RenfInfoAddComplete(RenfInfo& ri);
    //修改参考值
    void RenfInfoModifyComplete(RenfInfo& ri);
private:
    //初始化性别下拉框
    void InitSexControl();
    //初始化项目下拉框
    void InitItemControl(RenfInfo& ri);
    //
    void changeEvent(QEvent *e);
private:
    Ui::RenfValue *ui;
    //参考值内容信息
    RenfInfo m_RenfInfo;
    //是否为新增加的参考值对像
    bool m_isAddObj;
};

#endif // RENFVALUE_H
