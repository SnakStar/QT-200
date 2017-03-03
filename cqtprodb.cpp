/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: cqtprodb.cpp
 *  简要描述: 数据库实现文件，实现数据库连接、表格创建、sql语句执行
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/

#include "cqtprodb.h"

CQtProDB::CQtProDB()
{

}

CQtProDB::~CQtProDB()
{
    if(m_query){
        m_query->clear();
        delete m_query;
        m_query = NULL;
    }
}

/*
void CQtProDB::ShowTableData(QString strSql)
{
    if(m_query->exec(strSql)){
        int nRow = 0;
        m_tableWidget->setRowCount(0);
        while(m_query->next()){
            m_tableWidget->insertRow(nRow);
            for(int i=1; i<m_query->record().count(); i++){
                if(i == 1){
                    QTableWidgetItem* item = new QTableWidgetItem();
                    item->setCheckState(Qt::Unchecked);
                    item->setText(m_query->value(i).toString());
                    m_tableWidget->setItem(nRow,0,item);
                    //m_tableWidget->setItem(nRow,i-1,new QTableWidgetItem(m_query->value(i).toString()));
                    continue;
                }
                m_tableWidget->setItem(nRow,i-1,new QTableWidgetItem(m_query->value(i).toString()));
            }
            nRow++;
        }
    }
}
*/

/********************************************************
 *@Name:ConnectDB
 *@Author:HuaT
 *@Description:连接指定路径数据库
 *@Param:数据库路径
 *@Return:连接数据库是否成功
 *@Version:1.0
 *@Date:2016-5-13
********************************************************/
bool CQtProDB::ConnectDB(QString dbPath)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);
    if(m_db.open()){
        if(m_db.isValid()){
            m_query = new QSqlQuery(m_db);
            if(!CreateTable()){
                qDebug() << "CQtProDB::ConnectDB : create table fail!";
                return false;
            }
            return true;
        }
    }
    return false;
}

/********************************************************
 *@Name:connectDB
 *@Author:HuaT
 *@Description:连接默认数据库，位置为当前目录下的qtdb.db
 *@Param:无
 *@Return:连接是否成功
 *@Version:1.0
 *@Date:2016-5-13
********************************************************/
bool CQtProDB::ConnectDB()
{
    //QString CurrentPath = QDir::currentPath();
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbFilePath;
    //dbFilePath = CurrentPath + "/qtdb.db";
    dbFilePath = "/home/root/qt200/qtdb.db";
    m_db.setDatabaseName(dbFilePath);
    if(m_db.open()){
        if(m_db.isValid()){
            m_query = new QSqlQuery(m_db);
            if(!CreateTable()){
                qDebug() << "CQtProDB::ConnectDB : create table fail!" << m_query->lastError();
                return false;
            }
            return true;
        }
    }
    return false;
}

/********************************************************
 *@Name:GetSqlQuery
 *@Author:HuaT
 *@Description:获取数据库查询对象
 *@Param:无
 *@Return:返回数据库查询实例对象
 *@Version:1.0
 *@Date:2016-5-13
********************************************************/
QSqlQuery *CQtProDB::GetSqlQuery()
{
    return m_query;
}

/********************************************************
 *@Name: GetDatabase
 *@Author:HuaT
 *@Description: 获取数据库实例
 *@Param:无
 *@Return:返回数据库实例对象
 *@Version:1.0
 *@Date:2016-5-13
********************************************************/
QSqlDatabase *CQtProDB::GetDatabase()
{
    return &m_db;
}

/********************************************************
 *@Name:        Exec
 *@Author:      HuaT
 *@Description: 执行SQL语句
 *@Param:       SQL语句
 *@Return:      执行是否成功
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
bool CQtProDB::Exec(QString strSql)
{
    return m_query->exec(strSql);
}

/********************************************************
 *@Name:        ExecQuery
 *@Author:      HuaT
 *@Description: 执行SQL查询语句，返回查询结果
 *@Param:       SQL语句
 *@Return:      查询结果列表
 *@Version:     1.0
 *@Date:        2016-12-29
********************************************************/
QStringList CQtProDB::ExecQuery(QString strSql)
{
    QStringList listResult;
    if(!m_query->exec(strSql)){
        return listResult;
    }else{
        while(m_query->next()){
            for(int i=0; i<m_query->record().count(); i++){
                listResult.append(m_query->value(i).toString());
            }
        }
        return listResult;
    }
}

/************************************
 *Name:           LastError
 *Author:         HuaT
 *Description:    返回数据库操作时的错误信息
 *Param:          无
 *Return:         错误信息内容
 *Version:        1.0
 *Date:           2017-3-3
************************************/
QString CQtProDB::LastError()
{
    return m_query->lastError().text();
}

/************************************
 *Name:CreateTable
 *Author:HuaT
 *Description:检测数据库中是否有指定表，如果没有则新建
 *Param:无
 *Return:创建数据库表是否成功，有一个表创建失败返回FALSE
 *Version:1.0
 *Date:2016-5-13
************************************/

bool CQtProDB::CreateTable()
{
    bool bOK1 = false;
    bool bOK2 = false;
    bool bOK3 = false;
    bool bOK4 = false;
    bool bOK5 = false;
    bool bOK6 = false;
    bool bOK7 = false;
    bool bOK8 = false;
    //病人信息表
    QString strCreateTable = "create table if not exists \
                Patient(ID INTEGER PRIMARY KEY AUTOINCREMENT,\
                     Number integer,\
                     Name text,\
                     Age smallint,\
                     Sex integer,\
                     Item text,\
                     Result text,\
                     TestDate datetime,\
                     RawData text,\
                     Channel integer)";
    bOK1 = m_query->exec(strCreateTable);
    if(!bOK1){
        qDebug()<<"Patient:"<<m_query->lastError().text();
    }
    //日志表
    strCreateTable = "create table if not exists \
                Log(ID INTEGER PRIMARY KEY AUTOINCREMENT,\
                         LogType integer,\
                         TestNumbel integer,\
                         LogContent text,\
                         LogDate datetime)";
    bOK2 = m_query->exec(strCreateTable);
    if(!bOK2){
        qDebug()<<"Log:"<<m_query->lastError().text();
    }
    //质控结果表
    strCreateTable = "create table if not exists \
                Qc(ID INTEGER PRIMARY KEY AUTOINCREMENT,\
                         Item text,\
                         BatchNo text,\
                         Result text,\
                         RawData text,\
                         CheckDate datetime)";
    bOK3 = m_query->exec(strCreateTable);
    if(!bOK3){
        qDebug()<<"Qc:"<<m_query->lastError().text();
    }
    /*strCreateTable = "create table if not exists \
                RenfValue(ID INTEGER PRIMARY KEY AUTOINCREMENT,\
                         Items text,\
                         Value text,\
                         Age smallint,\
                         Sex text)";*/
    //结果参考值表
    strCreateTable = "create table if not exists \
            RenfValue(Item text,\
                      Value text,\
                      AgeLow text,\
                      AgeHight text,\
                      Sex smallint)";
    bOK4 = m_query->exec(strCreateTable);
    if(!bOK4){
        qDebug()<<"RenfValue:"<<m_query->lastError().text();
    }
    //ID卡表
    strCreateTable = "create table if not exists \
            IDCard(ID INTEGER PRIMARY KEY AUTOINCREMENT,\
                   CardNo integer,\
                   BatchNo text,\
                   Data text)";
    bOK5 = m_query->exec(strCreateTable);
    if(!bOK5){
        qDebug()<<"IDCard:"<<m_query->lastError().text();
    }

    //质控参考值表
    strCreateTable = "create table if not exists \
            QCRenfValue(ID INTEGER PRIMARY KEY AUTOINCREMENT,\
                        Item text,\
                        BatchNo text,\
                        Upper text,\
                        Lower text,\
                        InputTime datetime)";
    bOK6 = m_query->exec(strCreateTable);
    if(!bOK6){
        qDebug()<<m_query->lastError().text();
    }
    //测试项目记录表
    strCreateTable = "create table if not exists \
            ItemType(ID INTEGER PRIMARY KEY AUTOINCREMENT,\
                        Item text)";
    bOK7 = m_query->exec(strCreateTable);
    if(!bOK7){
        qDebug()<<m_query->lastError().text();
    }
    //创建索引
    QString strIndex;
    strIndex = "Create INDEX IF NOT EXISTS index_Number ON Patient(number,testdate)";
    bOK8 = m_query->exec(strIndex);
    if(!bOK8){
        qDebug()<<m_query->lastError().text();
    }
    return bOK1&&bOK2&&bOK3&&bOK4&&bOK5&&bOK6&&bOK7&&bOK8;
}
