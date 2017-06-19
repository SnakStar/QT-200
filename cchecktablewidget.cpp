/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: cchecktablewidget.cpp
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

#include "cchecktablewidget.h"

CCheckTableWidgetModel::CCheckTableWidgetModel(QTableWidget *tableWidget,
                                               QSqlQuery* sqlQuery, QSqlDatabase *db):
    m_tableWidget(tableWidget),
    m_sqlQuery(sqlQuery),
    m_db(db)
{

    m_checkBoxHeader = new CheckBoxHeader(Qt::Horizontal,tableWidget);
    m_tableWidget->setHorizontalHeader(m_checkBoxHeader);

    m_tableWidget->verticalHeader()->setHidden(true);
    m_tableWidget->setColumnCount(TABLE_COLUMN);
    //m_tableWidget->setRowCount(SHOW_ROW);
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    //m_tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);


    QStringList headerList;
    headerList <<QObject::tr("Numble") <<QObject::tr("Name") <<QObject::tr("Age")
              <<QObject::tr("Sex") <<QObject::tr("Test Item")
             <<QObject::tr("Check Result") <<QObject::tr("Check Time");
    m_tableWidget->setHorizontalHeaderLabels(headerList);

    m_tableWidget->setColumnWidth(0,150);
    m_tableWidget->setColumnWidth(1,80);
    m_tableWidget->setColumnWidth(2,50);
    m_tableWidget->setColumnWidth(3,50);
    m_tableWidget->setColumnWidth(4,150);
    m_tableWidget->setColumnWidth(5,130);


    m_tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    m_tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);

    QObject::connect(m_checkBoxHeader,SIGNAL(checkBoxClicked(bool)),this,SLOT(HeaderCheckBoxClicked(bool)));
    QObject::connect(this,SIGNAL(CheckBoxClicked(bool)),m_checkBoxHeader,SLOT(changeHeaderCheckBox(bool)));
}

/********************************************************
 *@Name:        exec
 *@Author:      HuaT
 *@Description: 执行SQL语句
 *@Param:       SQL字符串
 *@Return:      返回执行结果
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
bool CCheckTableWidgetModel::exec(QString strSql)
{
    if(m_sqlQuery->exec(strSql)){
        PatientInfo pi;
        if(!m_PatientInfos.isEmpty()){
            m_PatientInfos.clear();
        }
        while(m_sqlQuery->next()){
            pi.m_nNumble = m_sqlQuery->value(0).toLongLong();
            pi.m_strName = m_sqlQuery->value(1).toString();
            pi.m_nAge = m_sqlQuery->value(2).toInt();
            pi.m_strSex = m_sqlQuery->value(3).toInt();
            pi.m_strItems = m_sqlQuery->value(4).toString();
            pi.m_strResult = m_sqlQuery->value(5).toString();
            pi.m_strTestDate = m_sqlQuery->value(6).toString();
            m_PatientInfos.append(pi);
            //qDebug()<<m_sqlQuery->value(0).toInt()<<m_sqlQuery->value(1).toString();
        }
        return true;
    }else{
        qDebug()<<"exec:"<<m_sqlQuery->lastError().text();
        return false;
    }
}

//
/********************************************************
 *@Name:
 *@Author:      HuaT
 *@Description: 显示第几页内容到列表控件
 *@Param:       需要显示的当前页数，currentPage > 0, <=0 is invalid
 *@Return:      显示数据是否成功
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
bool CCheckTableWidgetModel::ShowDataBaseToTable(int currentPage)
{
    QVector<PatientInfo>::const_iterator it;
    //解析性别变量
    QString strSex;
    int nSex;
    //获取当前查询结果的总页数
    int nCountPage = GetCountPage();
    if(currentPage <= 0 || currentPage > nCountPage){
        qDebug()<<"currentPage is invalid!";
        return false;
    }
    //清除当前表格信息
    m_tableWidget->setRowCount(0);
    //计算起始结束行号
    int nStartRow = currentPage * SHOW_ROW - SHOW_ROW;
    int nEndRow = currentPage * SHOW_ROW;
    int nCurrentRow = 0;
    //插入数据到哪一行
    int nInserRow;
    for(it = m_PatientInfos.begin(); it != m_PatientInfos.end(); it++){
        //如果当前行小于指定页的开始行则移动it
        if(nCurrentRow<nStartRow){
            nCurrentRow++;
            continue;
        }
        //如果当前行大于指定页的结果行则指定页填充完毕
        if(nCurrentRow>=nEndRow){
            break;
        }
        nInserRow = m_tableWidget->rowCount();
        m_tableWidget->insertRow(nInserRow);
        QTableWidgetItem* item0 = new QTableWidgetItem;
        QTableWidgetItem* item1 = new QTableWidgetItem;
        QTableWidgetItem* item2 = new QTableWidgetItem;
        QTableWidgetItem* item3 = new QTableWidgetItem;
        QTableWidgetItem* item4 = new QTableWidgetItem;
        QTableWidgetItem* item5 = new QTableWidgetItem;
        QTableWidgetItem* item6 = new QTableWidgetItem;
        if((*it).m_bSelected){
            item0->setCheckState(Qt::Checked);
            item0->setText(QString::number((*it).m_nNumble));
            m_tableWidget->setItem(m_tableWidget->rowCount()-1,0,item0);
        }else{
            item0->setCheckState(Qt::Unchecked);
            item0->setText(QString::number((*it).m_nNumble));
            m_tableWidget->setItem(m_tableWidget->rowCount()-1,0,item0);
        }
        item1->setText((*it).m_strName);
        m_tableWidget->setItem(m_tableWidget->rowCount()-1,1,item1);
        item2->setText(QString::number((*it).m_nAge));
        m_tableWidget->setItem(m_tableWidget->rowCount()-1,2,item2);
        //解析性别
        nSex = (*it).m_strSex;
        if(nSex == 1){
            strSex = QObject::tr("Male");
        }else if(nSex == 2){
            strSex = QObject::tr("Female");
        }else{
            strSex = QObject::tr("");
        }
        item3->setText(strSex);
        m_tableWidget->setItem(m_tableWidget->rowCount()-1,3,item3);
        item4->setText((*it).m_strItems);
        m_tableWidget->setItem(m_tableWidget->rowCount()-1,4,item4);
        item5->setText((*it).m_strResult);
        m_tableWidget->setItem(m_tableWidget->rowCount()-1,5,item5);
        item6->setText((*it).m_strTestDate);
        m_tableWidget->setItem(m_tableWidget->rowCount()-1,6,item6);

        nCurrentRow++;
    }
    //m_tableWidget->resizeColumnToContents(0);
    //m_tableWidget->resizeColumnToContents(2);
    m_tableWidget->resizeColumnToContents(6);
    return true;
}

/********************************************************
 *@Name:
 *@Author:      HuaT
 *@Description: 设置当前行的选择状态
 *@Param:       行号
 *@Param2:      当前行数的测试时间
 *@Param3:      需要设置的状态
 *@Return:      是否设置成功
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
bool CCheckTableWidgetModel::SetRowDateState(quint64 nNumble, QString strTestDate, bool bState)
{
    QVector<PatientInfo>::iterator it;
    for(it = m_PatientInfos.begin(); it != m_PatientInfos.end(); it++){
        if(nNumble == (*it).m_nNumble && strTestDate == (*it).m_strTestDate){
            (*it).m_bSelected = bState;
            if(!bState){
                qDebug()<<bState;
                emit CheckBoxClicked(bState);
            }
            return true;
        }
    }
    return false;
}

/********************************************************
 *@Name:        GetCountPage
 *@Author:      HuaT
 *@Description: 获取总页数
 *@Param:       无
 *@Return:      返回本次查询的页数
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
int CCheckTableWidgetModel::GetCountPage()
{
    return m_PatientInfos.size()%SHOW_ROW ?
                m_PatientInfos.size()/SHOW_ROW+1 :
                m_PatientInfos.size()/SHOW_ROW;
}

/********************************************************
 *@Name:        GetPatientInfo
 *@Author:      HuaT
 *@Description: 获取病人信息结果列表对象
 *@Param:       无
 *@Return:      返回本次查询的页数
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
QVector<PatientInfo> CCheckTableWidgetModel::GetPatientInfo()
{
    return m_PatientInfos;
}

/********************************************************
 *@Name:        Rows
 *@Author:      HuaT
 *@Description: 总行数
 *@Param:       无
 *@Return:      返回本次查询的总行数
 *@Version:     1.0
 *@Date:        2016-6-16
********************************************************/
int CCheckTableWidgetModel::Rows()
{
    if(m_db->driver()->hasFeature(QSqlDriver::QuerySize)){
        qDebug()<<"driver support function : QSqlQuery::size()";
        m_sqlQuery->lastError();
        return m_sqlQuery->size();
    }else{
        m_sqlQuery->last();
        int count = m_sqlQuery->at()+1;
        m_sqlQuery->first();
        return count;
    }
}

/********************************************************
 *@Name:        Columns
 *@Author:      HuaT
 *@Description: 表格页数
 *@Param:       无
 *@Return:      返回此次查询的列数
 *@Version:     1.0
 *@Date:        2016-6-16
********************************************************/
int CCheckTableWidgetModel::Columns()
{
    return m_sqlQuery->record().count();
}

/********************************************************
 *@Name:        HeaderCheckBoxClicked
 *@Author:      HuaT
 *@Description: 表头点击事件
 *@Param:       表头状态
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-16
********************************************************/
void CCheckTableWidgetModel::HeaderCheckBoxClicked(bool state)
{
    QVector<PatientInfo>::iterator it;

    for(int i=0; i<m_tableWidget->rowCount(); i++){
        m_tableWidget->item(i,0)->setCheckState(state?Qt::Checked:Qt::Unchecked);
        for(it = m_PatientInfos.begin(); it != m_PatientInfos.end(); it++){
            quint64 nNum = m_tableWidget->item(i,0)->text().toULongLong();
            QString strDate = m_tableWidget->item(i,6)->text();
            //qDebug()<<nNum<<(*it).m_nNumble<<"  date:" <<strDate<<(*it).m_strTestDate <<"  state:"<<state;
            if((*it).m_nNumble == nNum && (*it).m_strTestDate == strDate){
                (*it).m_bSelected = state;
                //qDebug()<<state<<(*it).m_bSelected;
            }
        }
    }
}


