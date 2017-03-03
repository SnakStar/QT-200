/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: querywindow.cpp
 *  简要描述: 查询界面实现文件，实现病人信息显示、打印等操作
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "querywindow.h"
#include "ui_querywindow.h"
#include"mainwindow.h"


QueryWindow::QueryWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueryWindow)
{
    ui->setupUi(this);

    m_CurrentPage = 1;
    m_CountPage = -1;
    //初始化查询时间
    InitQueryDate();
    //
    MainWindow* MainWin = (MainWindow*)parent;
    m_SqlDB = MainWin->GetSqlDatabase();
    m_Query = MainWin->GetSqlQuery();
    m_db    = MainWin->GetQtProDb();
    //获取配置文件对象
    m_settings = MainWin->GetUtilSetting();
    //读取配置文件参数
    m_SetParam = m_settings->ReadSettingsInfoToMap();
    //获取打印串口对象
    m_SerialPrint = MainWin->GetPrintSerialPort();
    //获取HL7串口对象
    m_SerialHL7 = MainWin->GetHL7SerialPort();
    //初始化列表控件
    InitPatientInfo();
    //取消时间控件输入法焦点
    ui->dteStartDate->setProperty("noinput",true);
    ui->dteEndDate->setProperty("noinput",true);
    //隐藏修改和删除按钮
    //ui->btnModify->hide();
    //ui->btnDelete->hide();


}

QueryWindow::~QueryWindow()
{
    delete ui;
}

/********************************************************
 *@Name:        SetExportHide
 *@Author:      HuaT
 *@Description: 设置导出按钮是否隐藏
 *@Param:       true-隐藏，false-显示
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-15
********************************************************/
void QueryWindow::SetExportHide(bool bHide)
{
    ui->btnExport->setHidden(bHide);
}

/********************************************************
 *@Name:        SetDbModifyFuncHide
 *@Author:      HuaT
 *@Description: 设置数据库删除修改按钮是否隐藏
 *@Param:       true-隐藏，false-显示
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-3-3
********************************************************/
void QueryWindow::SetDbModifyFuncHide(bool bHide)
{
    ui->btnModify->setHidden(bHide);
    ui->btnDelete->setHidden(bHide);
}

/********************************************************
 *@Name:        InitQueryDate
 *@Author:      HuaT
 *@Description: 初始化查询的起始和结束时间
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-15
********************************************************/
void QueryWindow::InitQueryDate()
{
    QDate StartDate = QDate::currentDate();
    QDate EndDate = QDate::currentDate();
    ui->dteStartDate->setDate(StartDate);
    ui->dteEndDate->setDate(EndDate);
}

/********************************************************
 *@Name:        InitPatientInfo
 *@Author:      HuaT
 *@Description:
 *@Param:
 *@Return:
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void QueryWindow::InitPatientInfo()
{
    m_CheckTableWidget = new CCheckTableWidgetModel(ui->twPatientInfo,m_Query,m_SqlDB);
}

/********************************************************
 *@Name:        on_btnQuery_clicked
 *@Author:      HuaT
 *@Description: 查询按钮事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void QueryWindow::on_btnQueryData_clicked()
{
    QString StartDate = ui->dteStartDate->date().toString("yyyy-MM-dd");
    QString EndDate = ui->dteEndDate->date().toString("yyyy-MM-dd");
    QString Numble = ui->leNumble->text();
    QString Name = ui->leName->text();
    QString Filter,SqlCmd;
    if(Numble.isEmpty() && Name.isEmpty()){
        Filter = QString("where date(testdate) between '%1' and '%2'")
                .arg(StartDate).arg(EndDate);
    }else if(Numble.isEmpty()){
        Filter = QString("where name='%1' and date(testdate) between '%2' and '%3'")
                .arg(Name).arg(StartDate).arg(EndDate);
    }else if(Name.isEmpty()){
        Filter = QString("where number='%1' and date(testdate) between '%2' and '%3'")
                .arg(Numble).arg(StartDate).arg(EndDate);
    }else{
        Filter = QString("where number='%1' and name='%2' and date(testdate) between '%3' and '%4'")
                .arg(Numble).arg(Name).arg(StartDate).arg(EndDate);
    }
    SqlCmd = QString("select number,name,age,sex,item,result,testdate from patient %1 order by testdate desc")
            .arg(Filter);
    //qDebug()<<SqlCmd;
    m_CheckTableWidget->exec(SqlCmd);
    m_CheckTableWidget->ShowDataBaseToTable(1);

    //m_CheckTableWidget->GetCountPage()无结果时返回0
    m_CountPage = m_CheckTableWidget->GetCountPage();
    if(m_CountPage != 0){
        ui->lbPageTag->setText(QObject::tr("Current is %1 Page Count %2 Page").arg(m_CurrentPage).arg(m_CountPage));
    }else{
        ui->twPatientInfo->setRowCount(0);
        QString Title = QObject::tr("Note");
        QString Msg = QObject::tr("No qualified result, please modify and query again");
        QMessageBox::information(this,Title,Msg,QMessageBox::Ok);
    }

}

/********************************************************
 *@Name:        on_btnPrint_clicked
 *@Author:      HuaT
 *@Description: 打印按钮事件,语言配置为：0:中文，1:英文
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void QueryWindow::on_btnPrint_clicked()
{
    //QVector<PatientInfo>::iterator it;
    int nSex;
    QString strSex;
    int nLanguage = m_SetParam->value(LANGUAGESET,"1").toInt(0);
    QVector<PatientInfo> vecPatientInfo = m_CheckTableWidget->GetPatientInfo();
    QString strRenf,strSql,strFlag;
    QStringList listRenfValue;
    if(m_SerialPrint->isOpen()){
        for(int i=0; i<vecPatientInfo.count();i++){
            listRenfValue.clear();
            if(vecPatientInfo.at(i).m_bSelected == true){
                nSex = vecPatientInfo.at(i).m_strSex;
                if(nSex == 1){
                    strSex = QObject::tr("Male");
                }else if(nSex == 2){
                    strSex = QObject::tr("Female");
                }else{
                    strSex = QObject::tr("");
                }
                //计算参考值和参考标志
                strSql = QString("select * from renfvalue where item='%1' and sex=%2").arg(vecPatientInfo.at(i).m_strItems).arg(nSex);
                listRenfValue = m_db->ExecQuery(strSql);
                //年龄为字符，性别格式为字符型的0、1、2，0(Male/Female),1(Male),2(Female)
                strRenf = m_settings->GetRenfValue(
                            QString::number(vecPatientInfo.at(i).m_nAge),
                            listRenfValue);
                strFlag = m_settings->GetResultFlag(strRenf,vecPatientInfo.at(i).m_strResult);
                if(nLanguage == 0){
                    m_settings->PrintChineseData(m_SerialPrint,vecPatientInfo.at(i).m_strName,
                                                 QString::number(vecPatientInfo.at(i).m_nNumble),
                                                 QString::number(vecPatientInfo.at(i).m_nAge),strSex,
                                                 vecPatientInfo.at(i).m_strItems,
                                                 vecPatientInfo.at(i).m_strResult,
                                                 vecPatientInfo.at(i).m_strTestDate,
                                                 strRenf,strFlag);
                }else{
                    m_settings->PrintEnglishData(m_SerialPrint,vecPatientInfo.at(i).m_strName,
                                                 QString::number(vecPatientInfo.at(i).m_nNumble),
                                                 QString::number(vecPatientInfo.at(i).m_nAge),strSex,
                                                 vecPatientInfo.at(i).m_strItems,
                                                 vecPatientInfo.at(i).m_strResult,
                                                 vecPatientInfo.at(i).m_strTestDate,
                                                 strRenf,strFlag);
                }
            }
        }
    }
}

/********************************************************
 *@Name:        on_btnExport_clicked
 *@Author:      HuaT
 *@Description: 导出按钮事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void QueryWindow::on_btnExport_clicked()
{
    //ExportData* export = new ExportData();
    //export->show();
    ExportData* data = new ExportData(m_CheckTableWidget->GetPatientInfo());
    data->show();
    data->setWindowTitle(QObject::tr("File Export"));
}

/********************************************************
 *@Name:        on_btnPrePage_clicked
 *@Author:      HuaT
 *@Description: 上一页事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-16
********************************************************/
void QueryWindow::on_btnPrePage_clicked()
{
    QString Title = QObject::tr("Note");
    QString Msg = QObject::tr("No data in proceding");
    if(m_CurrentPage-1 == 0){
        QMessageBox::information(this,Title,Msg,QMessageBox::Ok);
    }else{
        m_CheckTableWidget->ShowDataBaseToTable(--m_CurrentPage);
        ui->lbPageTag->setText(QObject::tr("Current is %1 Page Count %2 Page").arg(m_CurrentPage).arg(m_CountPage));
    }
}

/********************************************************
 *@Name:        on_btnNextPage_clicked
 *@Author:      HuaT
 *@Description: 下一页事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-16
********************************************************/
void QueryWindow::on_btnNextPage_clicked()
{
    QString Title = QObject::tr("Note");
    QString Msg = QObject::tr("No data in the following");
    if(m_CurrentPage+1 > m_CountPage){
        QMessageBox::information(this,Title,Msg,QMessageBox::Ok);
    }else{
        m_CheckTableWidget->ShowDataBaseToTable(++m_CurrentPage);
        ui->lbPageTag->setText(QObject::tr("Current is %1 Page Count %2 Page").arg(m_CurrentPage).arg(m_CountPage));
    }
}

/********************************************************
 *@Name:        on_btnModify_clicked
 *@Author:      HuaT
 *@Description: 修改结果事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void QueryWindow::on_btnModify_clicked()
{
    QVector<PatientInfo> vecPatientInfo = m_CheckTableWidget->GetPatientInfo();
    for(int i=0; i<vecPatientInfo.count();i++){
        if(vecPatientInfo.at(i).m_bSelected == true){
            ResultModify* rm = new ResultModify(m_db,
                                                QString::number(vecPatientInfo.at(i).m_nNumble),
                                                vecPatientInfo.at(i).m_strTestDate,
                                                vecPatientInfo.at(i).m_strResult,this);
            rm->show();
            break;
        }
    }
}

/********************************************************
 *@Name:        on_btnDelete_clicked
 *@Author:      HuaT
 *@Description: 删除结果事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void QueryWindow::on_btnDelete_clicked()
{
    //QVector<PatientInfo>::iterator it;
    QVector<PatientInfo> vecPatientInfo = m_CheckTableWidget->GetPatientInfo();
    QString strSql,strFilter;
    strSql = QString("delete from patient where");
    for(int i=0; i<vecPatientInfo.count();i++){
        if(vecPatientInfo.at(i).m_bSelected == true){
            if(i != 0){
                strFilter.append(" or ");
            }
            strFilter.append(QString(" number=%1 and testdate='%2'")
                    .arg(vecPatientInfo.at(i).m_nNumble)
                    .arg(vecPatientInfo.at(i).m_strTestDate) );
            strSql.append(strFilter);
            strFilter.clear();
        }
    }
    //qDebug()<<strSql;
    QString strTitle,strContain;
    strTitle = "提示";
    strContain = "确定删除数据?删除后数据将无法恢复!";
    if(QMessageBox::Yes == QMessageBox::information(this,strTitle,strContain,QMessageBox::Yes|QMessageBox::No)){
        if(m_db->Exec(strSql)){
            strContain = "删除成功";
            QMessageBox::information(this,strTitle,strContain,QMessageBox::Ok);
        }else{
            strContain = QString("删除失败:%1").arg(m_db->LastError());
            QMessageBox::critical(this,strTitle,strContain,QMessageBox::Ok);
        }
    }
    on_btnQueryData_clicked();
}

/********************************************************
 *@Name:        on_btnPostback_clicked
 *@Author:      HuaT
 *@Description: 结果数据回传
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-2-13
********************************************************/
void QueryWindow::on_btnPostback_clicked()
{
    //QVector<PatientInfo>::iterator it;
    int nSex;
    QString strSex;
    QVector<PatientInfo> vecPatientInfo = m_CheckTableWidget->GetPatientInfo();
    if(m_SerialHL7->isOpen()){
        for(int i=0; i<vecPatientInfo.count();i++){
            if(vecPatientInfo.at(i).m_bSelected == true){
                nSex = vecPatientInfo.at(i).m_strSex;
                if(nSex == 1){
                    strSex = QObject::tr("M");
                }else if(nSex == 2){
                    strSex = QObject::tr("F");
                }else{
                    strSex = QObject::tr("U");
                }
                QStringList listResult = m_settings->ParseTestUnit(vecPatientInfo.at(i).m_strResult);
                QString strCheckTime;
                QDateTime dtCheckTime = QDateTime::fromString(vecPatientInfo.at(i).m_strTestDate,"yyyy-MM-dd hh:mm:ss");
                strCheckTime = dtCheckTime.toString("yyyyMMddhhmmss");
                m_SimpleHL7.WriteHL7Msg(m_SerialHL7,QString::number(vecPatientInfo.at(i).m_nNumble),
                                        vecPatientInfo.at(i).m_strName,
                                        QString::number(vecPatientInfo.at(i).m_nAge),
                                        strSex,vecPatientInfo.at(i).m_strItems,
                                        listResult.at(0),listResult.at(1),strCheckTime,"ORU^R01");
            }
        }
    }
}

/********************************************************
 *@Name:        on_twPatientInfo_itemClicked
 *@Author:      HuaT
 *@Description: 单击表格项
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void QueryWindow::on_twPatientInfo_itemClicked(QTableWidgetItem *item)
{
    if(ui->twPatientInfo->item(item->row(),0)->checkState() == Qt::Checked){
            ui->twPatientInfo->item(item->row(),0)->setCheckState(Qt::Unchecked);
            m_CheckTableWidget->SetRowDateState(ui->twPatientInfo->item(item->row(),0)->text().toLongLong(),
                                                     ui->twPatientInfo->item(item->row(),6)->text(),Qt::Unchecked);
        }else{
            ui->twPatientInfo->item(item->row(),0)->setCheckState(Qt::Checked);
            m_CheckTableWidget->SetRowDateState(ui->twPatientInfo->item(item->row(),0)->text().toLongLong(),
                                                     ui->twPatientInfo->item(item->row(),6)->text(),Qt::Checked);
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
void QueryWindow::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        InitPatientInfo();
        ui->twPatientInfo->setRowCount(0);
        break;
    default:
        break;
    }
}

/********************************************************
 *@Name:        Utf8ToGbk
 *@Author:      HuaT
 *@Description: UTF8编码格式转GBK编码格式
 *@Param:       UTF8编码字符串
 *@Return:      Gbk编码字符串内码
 *@Version:     1.0
 *@Date:        2016-6-20
********************************************************/
QByteArray QueryWindow::Utf8ToGbk(QString str)
{
    QTextCodec *gbk = QTextCodec::codecForName("gb18030");
    QTextCodec *utf8 = QTextCodec::codecForName("utf8");
    QTextCodec::setCodecForTr(gbk);
    QTextCodec::setCodecForLocale(gbk);
    QTextCodec::setCodecForCStrings(gbk);
    QByteArray byteGbk;
    byteGbk.append(gbk->toUnicode(str.toLocal8Bit()));
    QTextCodec::setCodecForTr(utf8);
    QTextCodec::setCodecForLocale(utf8);
    QTextCodec::setCodecForCStrings(utf8);
    return byteGbk;
}


