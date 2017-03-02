/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: SetTimer.h
 *  简要描述: 计时器头文件，实现多个计时器的倒计时功能
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/

#ifndef SETTIMER_H
#define SETTIMER_H
#include <QDialog>
#include<QMessageBox>
#include<QIntValidator>
#include<QDebug>

namespace Ui {
class SetTimerObj;
}

class SetTimerObj : public QDialog
{
    Q_OBJECT
    
public:
    explicit SetTimerObj(QWidget *parent = 0);
    ~SetTimerObj();
    //设置要计时的序列号
    void SetTimeSn(int Sn);
    //清除编辑框控件
    void ClearLineEdit();
    
private slots:
    //OK按钮事件
    void on_btnOK_clicked();

    //3分钟按钮事件
    void on_btn3Min_clicked();

    //5分钟按钮事件
    void on_btn5Min_clicked();

    //10分钟按钮事件
    void on_btn10Min_clicked();

    //15分钟按钮事件
    void on_btn15Min_clicked();

signals:
    void CoutDown(int Time,int SerialNum);
private:
    void changeEvent(QEvent *e);
private:
    Ui::SetTimerObj *ui;
    //计时器序号
    int m_TimeSN;
};

#endif // SETTIMER_H
