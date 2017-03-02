/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: qcvalue.h
 *  简要描述: 头文件，实现质控品的上下限数据存储
 *
 *  创建日期: 2016-7-12
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef QCVALUE_H
#define QCVALUE_H

#include <QDialog>
#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>

namespace Ui {
class QCValue;
}

class QCValue : public QDialog
{
    Q_OBJECT
    
public:
    explicit QCValue(QWidget *parent = 0);
    ~QCValue();

signals:
    void UpdateComboBoxMsg();

private slots:
    void on_btnSave_clicked();

private:
    Ui::QCValue *ui;
    QSqlQuery* m_Query;
};

#endif // QCVALUE_H
