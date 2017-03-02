/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: cchecktablewidget.h
 *  简要描述: 带CHECKBOX的列表控件源文件，实现插入的每条项目都能CHECK
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef CCHECKTABLEWIDGET_H
#define CCHECKTABLEWIDGET_H

#include<QObject>
#include<QDebug>
#include<QVector>
#include<QMutableVectorIterator>
#include<QTableWidget>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlError>
#include<QSqlDriver>
#include<QSqlRecord>
#include<QTableWidgetItem>
#include"ccheckheaderview.h"

#define SHOW_ROW 7
#define TABLE_COLUMN 7

class PatientInfo{
public:
    PatientInfo(){
        m_nNumble = 0;
        m_nAge = 0;
        m_bSelected = false;
        m_strName = "";
        m_strSex = 0;
        m_strItems = "";
        m_strResult = "";
        m_strTestDate = "";
    }

    quint64 m_nNumble;
    QString m_strName;
    int m_nAge;
    int m_strSex;
    QString m_strItems;
    QString m_strResult;
    QString m_strTestDate;
    bool m_bSelected;
};


class CCheckTableWidgetModel :public QObject
{
    Q_OBJECT
public:
    CCheckTableWidgetModel(QTableWidget *tableWidget,QSqlQuery* sqlQuery,QSqlDatabase* db);
    bool exec(QString strSql);
    bool ShowDataBaseToTable(int currentPage);
    bool SetRowDateState(quint64 nNumble, QString strTestDate, bool bState);
    int GetCountPage();
    QVector<PatientInfo> GetPatientInfo();
private:
    int Rows();
    int Columns();
private:
    QTableWidget* m_tableWidget;
    CheckBoxHeader* m_checkBoxHeader;
    QSqlQuery *m_sqlQuery;
    QSqlDatabase *m_db;

    QVector<PatientInfo> m_PatientInfos;

private slots:
    void HeaderCheckBoxClicked(bool state);
signals:
    void CheckBoxClicked(bool state);
};

#endif // CCHECKTABLEWIDGET_H
