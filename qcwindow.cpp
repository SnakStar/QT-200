/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: qcwindow.cpp
 *  简要描述: 质控界面实现文件，实现质控品的图形显示与存储
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "qcwindow.h"
#include "ui_qcwindow.h"
#include "mainwindow.h"

QCWindow::QCWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QCWindow)
{
    ui->setupUi(this);
    MainWindow* MainWin = (MainWindow*)parent;
    //数据库对象获取
    m_Query = MainWin->GetSqlQuery();
    //配置对象获取
    m_settings = MainWin->GetUtilSetting();
    m_SetParam = m_settings->ReadSettingsInfoToMap();
#ifdef Q_OS_LINUX
    //获取Can通信对象
    m_CanBus = MainWin->GetCanBus();
#endif
    //保存主界面对象
    m_ParentWidget = parent;
    //
    ui->CustomPlot->legend->setVisible(true);
    ui->CustomPlot->xAxis->setLabel("Raw Data To X");
    ui->CustomPlot->yAxis->setLabel("Raw Data To Y");
    ui->CustomPlot->xAxis->setAutoTickStep(false);
    ui->CustomPlot->xAxis->setTickStep(86400);
    //ui->CustomPlot->xAxis->setRange(0,30);
    ui->CustomPlot->yAxis->setRange(0,100);
    //时间格式
    ui->CustomPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->CustomPlot->xAxis->setDateTimeFormat("MM/\ndd");

    ui->CustomPlot->setBackground(QBrush(QColor(193,214,250)));
    //初始化下拉框
    InitItemComboBox();
    InitBatchNoComboBox(ui->cBoxItem->currentText());
    //画图
    SetCurveDataAddDraw(ui->cBoxItem->currentText(),ui->cBoxBatchNum->currentText());
    //
    ui->lbItem->hide();
    ui->cBoxItem->hide();

    //ui->btnQcTest->setEnabled(false);

}

QCWindow::~QCWindow()
{
    delete ui;
}

/********************************************************
 *@Name:        GetSqlQuery
 *@Author:      HuaT
 *@Description: 获取数据库查询对象
 *@Param:       无
 *@Return:      数据库查询对象地址
 *@Version:     1.0
 *@Date:        2016-7-12
********************************************************/
QSqlQuery *QCWindow::GetSqlQuery()
{
    return m_Query;
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
void QCWindow::changeEvent(QEvent *e)
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


#define Plot1_DotColor QColor(5,189,251)
#define Plot1_LineColor QColor(41,138,220)

#define LineWidth 2
#define DotWidth 8

#define Plot1_Count 20
/********************************************************
 *@Name:        DrawGraph
 *@Author:      HuaT
 *@Description: 图形线条数据
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-12
********************************************************/
void QCWindow::DrawGraph(QVector<double> keys, QVector<double> values)
{
    ui->CustomPlot->addGraph();
    ui->CustomPlot->graph()->removeFromLegend();
    //QColor LineColor = QColor(qrand()%255,qrand()%255,qrand()%255);
    //ui->CustomPlot->graph()->setPen(QPen(LineColor, LineWidth));
    ui->CustomPlot->graph()->setPen(QPen(QColor(8,161,180), LineWidth));
    /*
    ui->CustomPlot->graph()->setScatterStyle(
                QCPScatterStyle(QCPScatterStyle::ssCircle,
                                QPen(Plot1_DotColor, LineWidth),
                                QBrush(Plot1_DotColor), DotWidth));*/
    //QColor DotColor = QColor(qrand()%255,qrand()%255,qrand()%255);
    QColor DotColor = QColor(41,97,206);
    ui->CustomPlot->graph()->setScatterStyle(
                QCPScatterStyle(QCPScatterStyle::ssCircle,
                                QPen(DotColor, LineWidth),
                                QBrush(DotColor), DotWidth));

    ui->CustomPlot->graph()->setData(keys,values);
    ui->CustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->CustomPlot->replot();
}

/********************************************************
 *@Name:        DrawRenfValue
 *@Author:      HuaT
 *@Description: 画参考值线
 *@Param1:      项目名称
 *@Param2:      项目批号
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-9-21
********************************************************/
void QCWindow::DrawRenfValue(QString strItem, QString strBatchNo)
{
    QString strCMD = QString("select upper,lower from qcrenfvalue where item='%1' and batchno='%2' order by inputtime")
                             .arg(strItem).arg(strBatchNo);
    if(!m_Query->exec(strCMD)){
        qDebug()<<"query qcrenfvalue table to upper,lower error";
        return;
    }else{
        //
        //m_Query->first();
        double highValue = 0;
        double lowerValue = 0;
        if(m_PointX.size() == 0){
            highValue = QDateTime::currentDateTime().toTime_t()+84600*30;
            lowerValue = QDateTime::currentDateTime().toTime_t()-84600*30;
        }else{
            highValue = m_PointX.at(0)+84600*30;
            lowerValue = m_PointX.at(0)-84600*30;
        }
        if(m_Query->next()){
            ui->CustomPlot->legend->setVisible(true);
            //高值
            ui->CustomPlot->addGraph();
            ui->CustomPlot->graph()->addData(lowerValue,m_Query->value(0).toDouble());
            ui->CustomPlot->graph()->addData(highValue,m_Query->value(0).toDouble());
            ui->CustomPlot->graph()->setPen(QPen(QColor(255,139,190),2,Qt::DashLine));
            ui->CustomPlot->graph()->setName(m_Query->value(0).toString());
            //低值
            ui->CustomPlot->addGraph();
            ui->CustomPlot->graph()->addData(lowerValue,m_Query->value(1).toDouble());
            ui->CustomPlot->graph()->addData(highValue,m_Query->value(1).toDouble());
            ui->CustomPlot->graph()->setPen(QPen(QColor(76,144,50),2,Qt::DashLine));
            ui->CustomPlot->graph()->setName(m_Query->value(1).toString());
            //
            ui->CustomPlot->yAxis->setRange(0,m_Query->value(0).toDouble() * 2);
            ui->CustomPlot->replot();
        }else{
            ui->CustomPlot->legend->setVisible(false);
        }
    }
}

/********************************************************
 *@Name:        SetControlStatus
 *@Author:      HuaT
 *@Description: 设置质控测试相关控件状态
 *@Param:       控件是否可用
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-12-15
********************************************************/
void QCWindow::SetControlStatus(bool bStatus)
{
    ui->btnQcTest->setEnabled(bStatus);
    ui->cBoxBatchNum->setEnabled(bStatus);
}

/********************************************************
 *@Name:        on_btnQCSetting_clicked
 *@Author:      HuaT
 *@Description: 质控设置事件,添加质控上下限值
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-12
********************************************************/
void QCWindow::on_btnQCSetting_clicked()
{
    m_pQCValue = new QCValue(this);
    connect(m_pQCValue,SIGNAL(UpdateComboBoxMsg()), this, SLOT(UpdateComboBox()) );
    m_pQCValue->show();
}

/********************************************************
 *@Name:        on_btnTest_clicked
 *@Author:      HuaT
 *@Description: 测定按钮事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-12
********************************************************/
void QCWindow::on_btnQcTest_clicked()
{
    QString strTitle,strContent;
    strTitle = QObject::tr("Note");
    if(ui->cBoxBatchNum->currentText().isEmpty()){
        strContent = QObject::tr("Please select a batch number");
        QMessageBox::information(this,strTitle,strContent,QMessageBox::Ok);
        return;
    }
    QByteArray byteID,byteData;
    byteID[0] = QT_TESTOPT_SETUPTEST;
    byteID[1] = 0x00;
    byteID[2] = QT_HOST_MASTER;
    int nChannel = m_SetParam->value(CHANNELSET,0).toInt();
    bool bChannelStatus = true;
    switch (nChannel) {
    case 0:
        byteID[3] = QT_HOST_SLAVE1;
        bChannelStatus = ((MainWindow*)m_ParentWidget)->GetChannel1Status();
        strContent = QObject::tr("Channel1 is busy");
        break;
    case 1:
        byteID[3] = QT_HOST_SLAVE2;
        bChannelStatus = ((MainWindow*)m_ParentWidget)->GetChannel2Status();
        strContent = QObject::tr("Channel2 is busy");
        break;
    case 2:
        byteID[3] = QT_HOST_SLAVE3;
        bChannelStatus = ((MainWindow*)m_ParentWidget)->GetChannel3Status();
        strContent = QObject::tr("Channel3 is busy");
        break;
    default:
        byteID[3] = QT_HOST_SLAVE1;
        bChannelStatus = ((MainWindow*)m_ParentWidget)->GetChannel1Status();
        strContent = QObject::tr("Channel1 is busy");
        break;
    }
    //质控设置
    byteData[0] = 0x02;
    //如果通道正在运行则提示,返回
    if(bChannelStatus){
        QMessageBox::information(this,strTitle,strContent,QMessageBox::Ok);
        return;
    }else{
#ifdef Q_OS_LINUX
        m_CanBus->SendMsg(byteID,byteData.size(),byteData);
#endif
        //修改通道状态
        switch (nChannel) {
        case 0:
            ((MainWindow*)m_ParentWidget)->SetChannel1Status(true);
            break;
        case 1:
            ((MainWindow*)m_ParentWidget)->SetChannel2Status(true);
            break;
        case 2:
            ((MainWindow*)m_ParentWidget)->SetChannel3Status(true);
            break;
        default:
            break;
        }
    }
    //关闭控件
    SetControlStatus(false);
}

/********************************************************
 *@Name:        InitBatchNoComboBox
 *@Author:      HuaT
 *@Description: 根据项目名称,初始化批号下拉框
 *@Param:       项目名称
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-13
********************************************************/
void QCWindow::InitBatchNoComboBox(QString strItem)
{
    QString strCMD;
    //m_mapResultData.clear();
    ui->cBoxBatchNum->clear();
    //strCMD = "select batchno,result from qc order by checkdate asc limit 0,10";
    strCMD = QString("select batchno from QCRenfValue where item='%1' order by inputtime desc limit 0,10")
            .arg(strItem);
    if(!m_Query->exec(strCMD)){
        qDebug()<<"query QCRenfValue table to batchno error";
        return;
    }else{
        while(m_Query->next()){
            //m_mapResultData[m_Query->value(0).toString()] = m_Query->value(1).toString();
            ui->cBoxBatchNum->addItem(m_Query->value(0).toString());
        }
    }
}

/********************************************************
 *@Name:        InitItemComboBox
 *@Author:      HuaT
 *@Description: 初始化项目下拉框
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-9-21
********************************************************/
void QCWindow::InitItemComboBox()
{
    QString strCMD;
    //m_mapResultData.clear();
    ui->cBoxItem->clear();
    //strCMD = "select batchno,result from qc order by checkdate asc limit 0,10";
    strCMD = "select item from QCRenfValue group by item";
    if(!m_Query->exec(strCMD)){
        qDebug()<<"query QCRenfValue table to item error";
        return;
    }else{
        while(m_Query->next()){
            //m_mapResultData[m_Query->value(0).toString()] = m_Query->value(1).toString();
            ui->cBoxItem->addItem(m_Query->value(0).toString());
        }
    }
}

/********************************************************
 *@Name:        UpdateComboBox
 *@Author:      HuaT
 *@Description: 当有新批号添加时，更新批号下拉框
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-15
********************************************************/
void QCWindow::UpdateComboBox()
{
    QString strItem,strBatchNo;
    strItem = ui->cBoxItem->currentText();
    strBatchNo = ui->cBoxBatchNum->currentText();
    InitItemComboBox();
    InitBatchNoComboBox(ui->cBoxItem->currentText());
    SetCurveDataAddDraw(ui->cBoxItem->currentText(),ui->cBoxBatchNum->currentText());
}

/********************************************************
 *@Name:        UpdateResultDataRecord
 *@Author:      HuaT
 *@Description: 更新结果数据集
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-11
********************************************************/
void QCWindow::UpdateResultDataRecord()
{

    QString strCMD;
    m_mapResultData.clear();
    strCMD = "select batchno,result from qc order by checkdate asc limit 0,10";
    if(!m_Query->exec(strCMD)){
        return;
    }else{
        while(m_Query->next()){
            m_mapResultData[m_Query->value(0).toString()] = m_Query->value(1).toString();
        }
    }
}

/********************************************************
 *@Name:        SetCurveDataAddDraw
 *@Author:      HuaT
 *@Description: 设置曲线数据并画图
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-14
********************************************************/
void QCWindow::SetCurveDataAddDraw(QString strItem, QString strBatchNo)
{
    ui->CustomPlot->clearGraphs();
    ui->CustomPlot->replot();
    //
    QString strCMD = QString("select result,checkdate from qc where item='%1' and batchno='%2' order by checkdate")
                     .arg(strItem).arg(strBatchNo);
    m_PointX.clear();
    m_PointY.clear();
    int n=0;
    if(!m_Query->exec(strCMD)){
        qDebug()<<"SetCurveDataAddDraw: select query for error! ";
    }else{
        while(m_Query->next()){
            //m_PointX.append(n);
            QDateTime dtime = QDateTime::fromString(m_Query->value(1).toString(),"yyyy-MM-dd hh:mm:ss");
            //qDebug()<<dtime.toTime_t();
            m_PointX.append(dtime.toTime_t());
            m_PointY.append(m_Query->value(0).toFloat());
            n++;
        }
    }
    if(m_PointX.size()>0){
        ui->CustomPlot->xAxis->setRange(m_PointX.at(0)-43200,m_PointX.at(m_PointX.size()-1)+43200);
    }else{
        ui->CustomPlot->xAxis->setRange(QDateTime::currentDateTime().toTime_t(),QDateTime::currentDateTime().toTime_t()+84600*30);
    }
    //画线
    DrawGraph(m_PointX,m_PointY);
    //画参考值
    DrawRenfValue(strItem,strBatchNo);
}

/********************************************************
 *@Name:        UpdateResult
 *@Author:      HuaT
 *@Description: 收到结果数据，进行存储更新
 *@Param:       结果值
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-12
********************************************************/
void QCWindow::UpdateResult(QString strResult)
{
    QString msgTitle,msgContain;
    msgTitle = QObject::tr("Note");
    //int nValue = 11.2;
    msgContain = QObject::tr("The QC test results is %1, save the results?").arg(strResult);
    int nRe = QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Yes|QMessageBox::No);
    if(nRe == QMessageBox::Yes){
        QString strCMD;
        QString strItem = ui->cBoxItem->currentText();
        QString strBatchNo = ui->cBoxBatchNum->currentText();
        /*if(m_mapResultData.contains(strBatchNo)){
            QString strOldResult = m_mapResultData.value(strBatchNo);
            QString strNewResult = strOldResult + "|" + strResult;
            strCMD = QString("update qc set result='%1' where batchno='%2'")
                    .arg(strNewResult).arg(strBatchNo);
            if(!m_Query->exec(strCMD)){
                qDebug()<<"质控结果插入失败";
            }
        }else{
            strCMD = QString("insert into qc values(null,'%1','%2',datetime('now','localtime'),'')")
                    .arg(strBatchNo).arg(strResult);
            if(!m_Query->exec(strCMD)){
                qDebug()<<"质控结果插入失败";
            }
        }*/
        strCMD = QString("insert into qc values(null,'%1','%2','%3','',datetime('now','localtime'))")
                .arg(strItem).arg(strBatchNo).arg(strResult);
        if(!m_Query->exec(strCMD)){
            qDebug()<<"质控结果插入失败";
        }
        //结果插入后进行数据更新
        //InitBatchNoComboBox(ui->cBoxItem->currentText());
        SetCurveDataAddDraw(ui->cBoxItem->currentText(),ui->cBoxBatchNum->currentText());
    }
    SetControlStatus(true);
}

/********************************************************
 *@Name:        UpdateControlStatus
 *@Author:      HuaT
 *@Description: 更新质控控件状态
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-12-15
********************************************************/
void QCWindow::UpdateControlStatus()
{
    SetControlStatus(true);
}


/********************************************************
 *@Name:        on_cBoxBatchNum_currentIndexChanged
 *@Author:      HuaT
 *@Description: 下拉框更新事件,当新增加质控批号时更新下拉框
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-11
********************************************************/
/*void QCWindow::on_cBoxBatchNum_currentIndexChanged(const QString &arg1)
{
    //UpdateComboBox();
}*/

void QCWindow::on_cBoxBatchNum_activated(const QString &arg1)
{
    SetCurveDataAddDraw(ui->cBoxItem->currentText(),arg1);
}


void QCWindow::on_cBoxItem_activated(const QString &arg1)
{
    InitBatchNoComboBox(arg1);
    SetCurveDataAddDraw(arg1,ui->cBoxBatchNum->currentText());
}
