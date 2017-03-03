/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: cqtprodb.h
 *  简要描述: 数据库声明文件，实现数据库连接、表格创建、sql语句执行
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef CQTPRODB_H
#define CQTPRODB_H

#include<QSqlQuery>
#include<QTableWidget>
#include<QSqlDatabase>
#include<QString>
#include<QDebug>
#include<QSqlRecord>
#include<QSqlDriver>
#include<QSqlError>
#include<QDir>

class CQtProDB
{
public:
    CQtProDB();
    ~CQtProDB();
public:
    //连接数据库
    bool ConnectDB(QString dbPath);
    //连接数据库
    bool ConnectDB();

    //获取查询对象
    QSqlQuery *GetSqlQuery();
    //获取数据库对象
    QSqlDatabase *GetDatabase();
    //执行sql语句
    bool Exec(QString strSql);
    //返回查询结果
    QStringList ExecQuery(QString strSql);
    //返回出错信息
    QString LastError();

    //void ShowTableData(QString strSql);
private:
    //查询对象
    QSqlQuery *m_query;
    //数据库实例对象
    QSqlDatabase m_db;
private:
    //创建数据库表格
    bool CreateTable();
};

#endif // CQTPRODB_H
