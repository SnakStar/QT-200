/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: testwindow.cpp
 *  简要描述: 测试界面实现文件，实现检测结果计算、存储
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "testwindow.h"
#include "ui_testwindow.h"
#include "mainwindow.h"
#include"QRoundProgressBar.h"

TestWindow::TestWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestWindow)
{
    ui->setupUi(this);
    //
    ui->leNumber1->installEventFilter(this);
    ui->leNumber2->installEventFilter(this);
    ui->leNumber3->installEventFilter(this);
    //
    m_nLastValidNumber = 0;
    //是否为升级初始帧
    m_bIsStart = true;
    //
    m_nIndex1 = 0;
    m_nIndex2 = 0;
    m_nIndex3 = 0;
#ifdef Q_OS_LINUX
    //初始化Can口
    m_CanBus.StartCan("can0",125000);
    connect(&m_CanBus,SIGNAL(RecvDataMsg(quint32,QByteArray)),\
            this, SLOT(RecvCanMsg(quint32,QByteArray)) );
#endif
    //配置对象获取
    MainWindow* MainWin = (MainWindow*)parent;
    m_settings = MainWin->GetUtilSetting();
    m_SetParam = m_settings->ReadSettingsInfoToMap();
    //连接配置文件信号槽
    connect(m_settings,SIGNAL(PrintSettingChange()), this,SLOT(PrintModelChange()));
    connect(m_settings,SIGNAL(SettingChange()), this, SLOT(SettingChange()) );
    //连接调试界面信号槽
    //connect(m_UI, SIGNAL(StopAgingTest()), this, SLOT(StopAgingTimer()) );
    //获取数据库对象
    m_SqlDB = MainWin->GetSqlDatabase();
    m_Query = MainWin->GetSqlQuery();
    m_db = MainWin->GetQtProDb();
    //初始化运行时状态
    InitRunState();
    //初始化通道控件状态
    InitChannelState(1,false);
    InitChannelState(2,false);
    InitChannelState(3,false);
    //初始化测试状态
    m_TestStatus1 = false;
    m_TestStatus2 = false;
    m_TestStatus3 = false;
    //初始化自动测试状态
    m_bAutoMode1 = false;
    m_bAutoMode2 = false;
    m_bAutoMode3 = false;
    //打开计时定时器
    connect(&m_timerClock1,SIGNAL(timeout()), this, SLOT(CalcReactionTime1()) );
    connect(&m_timerClock2,SIGNAL(timeout()), this, SLOT(CalcReactionTime2()) );
    connect(&m_timerClock3,SIGNAL(timeout()), this, SLOT(CalcReactionTime3()) );
    //获取打印串口
    m_SerialPrint = MainWin->GetPrintSerialPort();
    connect(m_SerialPrint,SIGNAL(readyRead()), this, SLOT(RecvSerialData()));
    //获取HL7串口
    m_SerialHL7 = MainWin->GetHL7SerialPort();
    //经测试,这里增加的槽函数在主函数的后面,如果它已经ReadAll,则这里不能获取数据,否则可以
    connect(m_SerialHL7,SIGNAL(readyRead()), this, SLOT(RecvHL7SerialData()) );
    //初始化计时样式
    InitRoundBar();
    //获取放大偏移参数
    GetAPOffSetInfo();
    //获取硬件版本信息
    GetHardVersion();

}

TestWindow::~TestWindow()
{
    delete ui;
}

/********************************************************
 *@Name:        InitRoundBar
 *@Author:      HuaT
 *@Description: 初始化进度条对象
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-11-7
********************************************************/
void TestWindow::InitRoundBar()
{
    //初始化计时样式
    ui->wgRoundBar1->setFormat("%v");
    ui->wgRoundBar2->setFormat("%v");
    ui->wgRoundBar3->setFormat("%v");
    ui->wgRoundBar1->setDecimals(0);
    ui->wgRoundBar2->setDecimals(0);
    ui->wgRoundBar3->setDecimals(0);
    ui->wgRoundBar1->setValue(0);
    ui->wgRoundBar2->setValue(0);
    ui->wgRoundBar3->setValue(0);

    //init color
    //1
    QPalette p;
    //中心圆底色
    p.setBrush(QPalette::AlternateBase, Qt::white);
    p.setColor(QPalette::Text, Qt::black);
    //进度条底色
    p.setBrush(QPalette::Base, QBrush(QColor(170,170,255)));
    //背景色
    p.setBrush(QPalette::Background, QBrush(QColor(245,245,245)));
    //进度条颜色
    p.setBrush(QPalette::Highlight, QBrush(QColor(0,161,180)));
    //进度条边框颜色
    //p.setColor(QPalette::Shadow, Qt::red);
    ui->wgRoundBar1->setPalette(p);
    ui->wgRoundBar1->setOutlinePenWidth(14);
    ui->wgRoundBar1->setDataPenWidth(6);
    ui->wgRoundBar1->setBarStyle(QRoundProgressBar::StyleLine);
    QFont font;
    font.setFamily("SIMSUN");
    font.setPointSize(24);
    //font.setWeight(QFont::Bold);
    ui->wgRoundBar1->setFont(font);
    //2
    QPalette p2(p);
    p2.setBrush(QPalette::Base, Qt::white);
    ui->wgRoundBar2->setFont(font);
    ui->wgRoundBar2->setPalette(p2);

    ui->wgRoundBar2->setPalette(p);
    ui->wgRoundBar2->setOutlinePenWidth(14);
    ui->wgRoundBar2->setDataPenWidth(6);
    ui->wgRoundBar2->setBarStyle(QRoundProgressBar::StyleLine);
    //3
    QPalette p3(p);
    p3.setBrush(QPalette::Base, Qt::white);
    ui->wgRoundBar3->setFont(font);
    ui->wgRoundBar3->setPalette(p3);
    // make a gradient from green over yellow to red
    QGradientStops gradientPoints;
    gradientPoints << QGradientStop(0, Qt::red) << QGradientStop(0.5, Qt::yellow) << QGradientStop(1, Qt::green);
    // and set it
    //ui->wgRoundBar3->setDataColors(gradientPoints);

    ui->wgRoundBar3->setPalette(p);
    ui->wgRoundBar3->setOutlinePenWidth(14);
    ui->wgRoundBar3->setDataPenWidth(6);
    ui->wgRoundBar3->setBarStyle(QRoundProgressBar::StyleLine);

}


/********************************************************
 *@Name:        RecvSerialData
 *@Author:      HuaT
 *@Description: 打印串口响应函数
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-6
********************************************************/
void TestWindow::RecvSerialData(){
    //qDebug()<<m_SerialPrint->readAll();
}

/********************************************************
 *@Name:        RecvHL7SerialData
 *@Author:      HuaT
 *@Description: 打印HL7串口响应函数
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-2-13
********************************************************/
void TestWindow::RecvHL7SerialData()
{
    //qDebug()<<"Recv HL7 Serial Data to test window";
}

/********************************************************
 *@Name:        PrintModelChange
 *@Author:      HuaT
 *@Description: 打印模式更改
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-6
********************************************************/
void TestWindow::PrintModelChange()
{
    m_isAutoPrint = m_SetParam->value(PRINTMODE,0).toInt(0);
    m_isAutoEncode = m_SetParam->value(ENCODEMODE,0).toInt(0);
    //更新打印文本
    UpdatePrintText();
    //更新打印状态
    UPdatePrintState(1);
    UPdatePrintState(2);
    UPdatePrintState(3);

}

/********************************************************
 *@Name:        SettingChange
 *@Author:      HuaT
 *@Description: 配置更改
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-6
********************************************************/
void TestWindow::SettingChange()
{
    //qDebug()<<"SettingChange";
}

/********************************************************
 *@Name:        UpdatePrintText
 *@Author:      HuaT
 *@Description: 更新打印按钮文本为自动打印还是手动打印
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-6
********************************************************/
void TestWindow::UpdatePrintText()
{
    if(m_isAutoPrint){
        ui->btnPrint1->setText(QObject::tr("Auto Prt"));
        ui->btnPrint2->setText(QObject::tr("Auto Prt"));
        ui->btnPrint3->setText(QObject::tr("Auto Prt"));
    }else{
        ui->btnPrint1->setText(QObject::tr("Manual Prt"));
        ui->btnPrint2->setText(QObject::tr("Manual Prt"));
        ui->btnPrint3->setText(QObject::tr("Manual Prt"));

    }
}

/********************************************************
 *@Name:        UpdateTestText
 *@Author:      HuaT
 *@Description: 更新测试按钮文本为自动测试还是取消测试
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-6
********************************************************/
void TestWindow::UpdateTestText(int nChannel)
{
    switch(nChannel){
    case 1:
        if(m_TestStatus1){
            ui->btnAutoTest1->setText(QObject::tr("Cancel Test"));
        }else{
            ui->btnAutoTest1->setText(QObject::tr("Auto Test"));
        }
        break;
    case 2:
        if(m_TestStatus2){
            ui->btnAutoTest2->setText(QObject::tr("Cancel Test"));
        }else{
            ui->btnAutoTest2->setText(QObject::tr("Auto Test"));
        }
        break;
    case 3:
        if(m_TestStatus3){
            ui->btnAutoTest3->setText(QObject::tr("Cancel Test"));
        }else{
            ui->btnAutoTest3->setText(QObject::tr("Auto Test"));
        }
        break;
    default:
        break;
    }
}

/********************************************************
 *@Name:        UpdatePrintText
 *@Author:      HuaT
 *@Description: 更新打印按钮状态为是否可用
 *@Param:       更新按钮的通道号
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-6
********************************************************/
void TestWindow::UPdatePrintState(int nChannel)
{
    switch(nChannel){
    case 1:
        if(m_isAutoPrint){
            ui->btnPrint1->setEnabled(false);
        }else{
            if(ui->leResult1->text().isEmpty()){
                ui->btnPrint1->setEnabled(false);
            }else{
                ui->btnPrint1->setEnabled(true);
            }
        }
        break;
    case 2:
        if(m_isAutoPrint){
            ui->btnPrint2->setEnabled(false);
        }else{
            if(ui->leResult2->text().isEmpty()){
                ui->btnPrint2->setEnabled(false);
            }else{
                ui->btnPrint2->setEnabled(true);
            }
        }
        break;
    case 3:
        if(m_isAutoPrint){
            ui->btnPrint3->setEnabled(false);
        }else{
            if(ui->leResult3->text().isEmpty()){
                ui->btnPrint3->setEnabled(false);
            }else{
                ui->btnPrint3->setEnabled(true);
            }
        }
        break;
    default:
        break;
    }
}

/********************************************************
 *@Name:        CheckInputInfo
 *@Author:      HuaT
 *@Description: 检测用户输入的编号是否存在
 *@Param:       无
 *@Return:      可用返回true，否则返回false
 *@Version:     1.0
 *@Date:        2016-7-7
********************************************************/
bool TestWindow::CheckInputInfo(quint64 nNumber)
{
    QString sqlCmd = QString("select count(*) from patient where\
                             Number=%1 and date(testdate)=date('now','localtime')").arg(nNumber);
    if(!m_Query->exec(sqlCmd)){
        qDebug()<<"CheckInputInfo: 查询用户编号出错,请检查数据库";
        return false;
    }else{
        while(m_Query->next()){
            if(m_Query->value(0).toInt(0) == 0){
                return true;
            }else{
                return false;
            }
        }
        return true;
    }
}


/********************************************************
 *@Name:        InitChannelState1
 *@Author:      HuaT
 *@Description: 初始化通道1状态
 *@Param1:      禁用或打开的通道号
 *@Param2:      禁用或打开控件状态
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-4
********************************************************/
void TestWindow::InitChannelState(quint8 nChannel, bool bEnable)
{
    switch(nChannel){
    case 1:
        ui->leNumber1->setEnabled(bEnable);
        ui->leName1->setEnabled(bEnable);
        ui->leAge1->setEnabled(bEnable);
        ui->cBoxSex1->setEnabled(bEnable);
        ui->leItem1->setEnabled(bEnable);
        ui->leResult1->setEnabled(bEnable);

        ui->btnNew1->setEnabled(true);
        ui->btnAutoTest1->setEnabled(bEnable);
        ui->btnPrint1->setEnabled(bEnable);
        ui->btnInstantTest1->setEnabled(bEnable);
        break;
    case 2:
        ui->leNumber2->setEnabled(bEnable);
        ui->leName2->setEnabled(bEnable);
        ui->leAge2->setEnabled(bEnable);
        ui->cBoxSex2->setEnabled(bEnable);
        ui->leItem2->setEnabled(bEnable);
        ui->leResult2->setEnabled(bEnable);

        ui->btnNew2->setEnabled(true);
        ui->btnAutoTest2->setEnabled(bEnable);
        ui->btnPrint2->setEnabled(bEnable);
        ui->btnInstantTest2->setEnabled(bEnable);
        break;
    case 3:
        ui->leNumber3->setEnabled(bEnable);
        ui->leName3->setEnabled(bEnable);
        ui->leAge3->setEnabled(bEnable);
        ui->cBoxSex3->setEnabled(bEnable);
        ui->leItem3->setEnabled(bEnable);
        ui->leResult3->setEnabled(bEnable);

        ui->btnNew3->setEnabled(true);
        ui->btnAutoTest3->setEnabled(bEnable);
        ui->btnPrint3->setEnabled(bEnable);
        ui->btnInstantTest3->setEnabled(bEnable);
        break;
    default:
        qDebug()<<"Channel input is invalid!";
        break;
    }
}

/********************************************************
 *@Name:        SetChannelStateToNew
 *@Author:      HuaT
 *@Description: 新建时设置通道状态
 *@Param1:      通道号
 *@Param2:      禁用或打开控件状态
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-7
********************************************************/
void TestWindow::SetChannelStateToNew(quint8 nChannel, bool bEnable)
{
    //关闭时需要把项目和结果关闭，打开时也不需要打开，它们是只读的
    switch(nChannel){
    case 1:
        ui->leNumber1->setEnabled(bEnable);
        ui->leName1->setEnabled(bEnable);
        ui->leAge1->setEnabled(bEnable);
        ui->cBoxSex1->setEnabled(bEnable);

        ui->btnAutoTest1->setEnabled(bEnable);
        UPdatePrintState(nChannel);
        ui->btnInstantTest1->setEnabled(bEnable);
        break;
    case 2:
        ui->leNumber2->setEnabled(bEnable);
        ui->leName2->setEnabled(bEnable);
        ui->leAge2->setEnabled(bEnable);
        ui->cBoxSex2->setEnabled(bEnable);

        ui->btnAutoTest2->setEnabled(bEnable);
        //
        UPdatePrintState(nChannel);
        ui->btnInstantTest2->setEnabled(bEnable);
        break;
    case 3:
        ui->leNumber3->setEnabled(bEnable);
        ui->leName3->setEnabled(bEnable);
        ui->leAge3->setEnabled(bEnable);
        ui->cBoxSex3->setEnabled(bEnable);

        ui->btnAutoTest3->setEnabled(bEnable);
        //
        UPdatePrintState(nChannel);
        ui->btnInstantTest3->setEnabled(bEnable);
        break;
    default:
        break;
    }
}

/********************************************************
 *@Name:        SetChannelStateToAutoTest
 *@Author:      HuaT
 *@Description: 自动测试时设置通道状态
 *@Param1:      通道号
 *@Param2:      禁用或打开控件状态
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-7
********************************************************/
void TestWindow::SetChannelStateToAutoTest(quint8 nChannel, bool bEnable)
{
    switch(nChannel){
    case 1:
        ui->leNumber1->setEnabled(bEnable);
        ui->leName1->setEnabled(bEnable);
        ui->leAge1->setEnabled(bEnable);
        ui->cBoxSex1->setEnabled(bEnable);

        ui->btnNew1->setEnabled(bEnable);
        ui->btnPrint1->setEnabled(bEnable);
        ui->btnInstantTest1->setEnabled(bEnable);
        break;
    case 2:
        ui->leNumber2->setEnabled(bEnable);
        ui->leName2->setEnabled(bEnable);
        ui->leAge2->setEnabled(bEnable);
        ui->cBoxSex2->setEnabled(bEnable);

        ui->btnNew2->setEnabled(bEnable);
        ui->btnPrint2->setEnabled(bEnable);
        ui->btnInstantTest2->setEnabled(bEnable);
        break;
    case 3:
        ui->leNumber3->setEnabled(bEnable);
        ui->leName3->setEnabled(bEnable);
        ui->leAge3->setEnabled(bEnable);
        ui->cBoxSex3->setEnabled(bEnable);

        ui->btnNew3->setEnabled(bEnable);
        ui->btnPrint3->setEnabled(bEnable);
        ui->btnInstantTest3->setEnabled(bEnable);
        break;
    default:
        break;
    }
}


/********************************************************
 *@Name:        AutoTestButtonState
 *@Author:      HuaT
 *@Description: 自动测试快结束时不允许取消测试
 *@Param1:      通道号
 *@Param2:      禁用或打开控件状态
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-2
********************************************************/
void TestWindow::AutoTestButtonState(quint8 nChannel, bool bEnable)
{
    switch (nChannel) {
    case 1:
        ui->btnAutoTest1->setEnabled(bEnable);
        break;
    case 2:
        ui->btnAutoTest2->setEnabled(bEnable);
        break;
    case 3:
        ui->btnAutoTest3->setEnabled(bEnable);
        break;
    default:
        break;
    }
}

/********************************************************
 *@Name:        SetChannelStateToInTest
 *@Author:      HuaT
 *@Description: 即时测试时设置通道状态
 *@Param1:      通道号
 *@Param2:      禁用或打开控件状态
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-7
********************************************************/
void TestWindow::SetChannelStateToInTest(int nChannel, bool bEnable)
{
    switch(nChannel){
    case 1:
        ui->leNumber1->setEnabled(bEnable);
        ui->leName1->setEnabled(bEnable);
        ui->leAge1->setEnabled(bEnable);
        ui->cBoxSex1->setEnabled(bEnable);

        ui->btnNew1->setEnabled(bEnable);
        ui->btnPrint1->setEnabled(bEnable);
        ui->btnAutoTest1->setEnabled(bEnable);
        ui->btnInstantTest1->setEnabled(bEnable);
        break;
    case 2:
        ui->leNumber2->setEnabled(bEnable);
        ui->leName2->setEnabled(bEnable);
        ui->leAge2->setEnabled(bEnable);
        ui->cBoxSex2->setEnabled(bEnable);

        ui->btnNew2->setEnabled(bEnable);
        ui->btnPrint2->setEnabled(bEnable);
        ui->btnAutoTest2->setEnabled(bEnable);
        ui->btnInstantTest2->setEnabled(bEnable);
        break;
    case 3:
        ui->leNumber3->setEnabled(bEnable);
        ui->leName3->setEnabled(bEnable);
        ui->leAge3->setEnabled(bEnable);
        ui->cBoxSex3->setEnabled(bEnable);

        ui->btnNew3->setEnabled(bEnable);
        ui->btnPrint3->setEnabled(bEnable);
        ui->btnAutoTest3->setEnabled(bEnable);
        ui->btnInstantTest3->setEnabled(bEnable);
        break;
    default:
        break;
    }
}

/********************************************************
 *@Name:        CleareChannelInfo
 *@Author:      HuaT
 *@Description: 清除对应通道上的病人信息，以便用户新建
 *@Param:       需要清除的通道号
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-4
********************************************************/
void TestWindow::CleareChannelInfo(int nChannel)
{
    switch(nChannel){
    case 1:
        ui->leNumber1->clear();
        ui->leName1->clear();
        ui->leAge1->clear();
        ui->cBoxSex1->setCurrentIndex(0);
        ui->leItem1->clear();
        ui->leResult1->clear();
        //ui->lcdNumberRecTime1->display(0);
        ui->wgRoundBar1->setValue(0);
        break;
    case 2:
        ui->leNumber2->clear();
        ui->leName2->clear();
        ui->leAge2->clear();
        ui->cBoxSex2->setCurrentIndex(0);
        ui->leItem2->clear();
        ui->leResult2->clear();
        //ui->lcdNumberRecTime2->display(0);
        ui->wgRoundBar2->setValue(0);
        break;
    case 3:
        ui->leNumber3->clear();
        ui->leName3->clear();
        ui->leAge3->clear();
        ui->cBoxSex3->setCurrentIndex(0);
        ui->leItem3->clear();
        ui->leResult3->clear();
        //ui->lcdNumberRecTime3->display(0);
        ui->wgRoundBar3->setValue(0);
        break;
    default:
        qDebug()<<"输入的清除通道号有误，需要重新输入";
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
void TestWindow::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        UpdatePrintText();
        break;
    default:
        break;
    }
}

/********************************************************
 *@Name:        GetValidNumber
 *@Author:      HuaT
 *@Description: 通过查询数据得到当前当天最大的编号
 *@Param:       无
 *@Return:      当天最大编号,当返回-1时，表明执行查询语句出错
 *@Version:     1.0
 *@Date:        2016-7-6
********************************************************/
quint64 TestWindow::GetValidNumber()
{
    //QTime time1,time2;
    //time1 = QTime::currentTime();
    QString sqlCmd = "select max(number) from patient where date(testdate)=date('now','localtime')";
    if(!m_Query->exec(sqlCmd)){
      return -1;
    }
    //time2 = QTime::currentTime();
    //int nElapse = time1.msecsTo(time2);
    //qDebug()<<"nElapse:"<<nElapse;
    if(m_Query->next()){
        return m_Query->value(0).toLongLong(0);
    }else{
        return 1;
    }
}

/********************************************************
 *@Name:        InitRunState
 *@Author:      HuaT
 *@Description: 初始化测试模式状态，获取检测时需要的状态
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-6
********************************************************/
void TestWindow::InitRunState()
{
    m_isAutoPrint  = m_SetParam->value(PRINTMODE,0).toInt(0);
    m_isAutoEncode = m_SetParam->value(ENCODEMODE,0).toInt(0);
}

/********************************************************
 *@Name:        on_btnNew1_clicked
 *@Author:      HuaT
 *@Description: 新建1按钮单击事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-4
********************************************************/
void TestWindow::on_btnNew1_clicked()
{
    NewTest(1);
}

/********************************************************
 *@Name:        on_btnPrint1_clicked
 *@Author:      HuaT
 *@Description: 打印1按钮单击事件,语言配置为：0:中文，1:英文
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-6
********************************************************/
void TestWindow::on_btnPrint1_clicked()
{
    QString strName,strNumber,strAge,strSex,strItem,strResult;
    strName = ui->leName1->text();
    strNumber = ui->leNumber1->text();
    strAge = ui->leAge1->text();
    strSex = ui->cBoxSex1->currentText();
    strItem = ui->leItem1->text();
    strResult = ui->leResult1->text();
    int nLanguage = m_SetParam->value(LANGUAGESET,"1").toInt(0);
    QString strCheckTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //计算参考值和参考标志
    quint8 nSex;
    if(strSex.compare(QObject::tr("Male")) == 0){
        nSex = 1;
    }else if(strSex.compare(QObject::tr("Female")) == 0){
        nSex =2;
    }else{
        nSex = 0;
    }
    QString strSql = QString("select * from renfvalue where item='%1' and sex=%2").arg(strItem).arg(nSex);
    QStringList listRenfValue = m_db->ExecQuery(strSql);
    QString strRenf = m_settings->GetRenfValue(strAge,listRenfValue);
    QString strFlag = m_settings->GetResultFlag(strRenf,strResult);
    //打印
    if(nLanguage == 0){
        m_settings->PrintChineseData(m_SerialPrint,strName,strNumber,
                                     strAge,strSex,strItem,strResult,
                                     strCheckTime,strRenf,strFlag);
    }else{
        m_settings->PrintEnglishData(m_SerialPrint,strName,strNumber,
                                     strAge,strSex,strItem,strResult,
                                     strCheckTime,strRenf,strFlag);
    }
}

/********************************************************
 *@Name:        AutoTest
 *@Author:      HuaT
 *@Description: 用户选择自动测试模式
 *@Param1:      选择测试的通道
 *@Param2:      对应通道测试时的状态
 *@Param3:      需求清除的数据结构体
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-11
********************************************************/
void TestWindow::AutoTest(int nChannel, bool &bIsAutoTesting, ResultDataInfo& DataObj, QTimer& timer)
{
    QString msgTitle = QObject::tr("Warning");
    QString msgContain;
    if(bIsAutoTesting){
        msgContain = QObject::tr("Auto test is underway, cancel test will lead \n to the failure of reagent strip, whether to continue?");
        int nButton = QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Yes | QMessageBox::No);
        if(nButton == QMessageBox::Yes){
            //发送取消测试消息
            QByteArray byteRespond;
            PackageCanMsg(QT_TESTOPT_STOPTEST,QT_NULL_SYNC,nChannel,byteRespond);
            //清除本次测试界面信息
            CleareChannelInfo(nChannel);
            //恢复至新建时的按钮状态
            InitChannelState(nChannel,false);
            //测试状态取消
            bIsAutoTesting = false;
            SetTestStatus(nChannel,false);
            //自动测试模式关闭
            ChangeAutoModeStates(nChannel,false);
            //更新测试按钮控件文本
            UpdateTestText(nChannel);
            //停止倒计时功能(在收到条码时开始)
            timer.stop();
            //清除结构体数据
            ClearResultDataInfo(DataObj);
        }
    }else{
        //检测通道状态
        bool bStatus = CheckTestStatus(nChannel);
        if(bStatus){
            return;
        }
        //插入用户输入的信息
        int nState = InsertTestInfoToDb(nChannel);
        if(nState == 1){
            msgContain = QObject::tr("The serial number entered already exists, please enter again");
            QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Ok);
            return ;
        }else if(nState ==2){
            msgContain = QObject::tr("User information insert failed, stop test");
            QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Ok);
            return ;
        }
        //发送准备测试消息给下位机-1为自动测试-0为即时测试
        QByteArray byteRespond;
        byteRespond[0] = 0x01;
        PackageCanMsg(QT_TESTOPT_SETUPTEST,QT_NULL_SYNC,nChannel,byteRespond);
        //更改测试状态
        bIsAutoTesting = true;
        SetTestStatus(nChannel,true);
        //自动测试模式开启
        ChangeAutoModeStates(nChannel,true);
        //更新测试按钮控件文本
        UpdateTestText(nChannel);
        //更改其它控件按钮状态
        SetChannelStateToAutoTest(nChannel,false);
    }
}

/********************************************************
 *@Name:        InstantTest
 *@Author:      HuaT
 *@Description: 用户选择即时测试模式
 *@Param:       选择测试的通道
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-11
********************************************************/
void TestWindow::InstantTest(int nChannel)
{
    //检测通道状态
    bool bStatus = CheckTestStatus(nChannel);
    if(bStatus){
        return;
    }
    QString msgTitle = QObject::tr("Warning");
    QString msgContain;
    //插入用户输入的信息
    int nState = InsertTestInfoToDb(nChannel);
    if(nState == 1){
        msgContain = QObject::tr("Enter the serial number of already exists,Please enter again");
        QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Ok);
        return ;
    }else if(nState ==2){
        msgContain = QObject::tr("User information insertion failure, stop test");
        QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Ok);
        return ;
    }
    //设置通道状态为使用
    SetTestStatus(nChannel,true);
    //设置通道按钮状态
    SetChannelStateToInTest(nChannel,false);
    //发送测试信息给下位机
    QByteArray byteRespond;
    byteRespond[0] = 0x00;
    PackageCanMsg(QT_TESTOPT_SETUPTEST,QT_NULL_SYNC,nChannel,byteRespond);
    //用户开始测试
    QString strTestMsg = QString("通道%1开始测试").arg(nChannel);
    qDebug()<<strTestMsg;
}

/********************************************************
 *@Name:        NewTest
 *@Author:      HuaT
 *@Description: 新建按钮事件,并自动填写有效编号，现在的实现方法为编号框已打开的直接返回，
 *              有一个打开编号框的加1，有二个则加2.
 *@Param:       需要新建的通道号
 *@Return:      无
 *@Version:     1.1
 *@Date:        2016-7-11
 *
 *@Data:        2016-8-8
 *@Modify:      修改1-3通道自动编号由固定变为自动加1，如果编号框已点亮，则直接返回
********************************************************/
void TestWindow::NewTest(int nChannel)
{
    //检测通道状态
    bool bStatus = CheckTestStatus(nChannel);
    if(bStatus){
        return;
    }
    //检测编码模式
    if(1 == m_isAutoEncode){
        //打开用户信息输入控件
        SetChannelStateToNew(nChannel,true);
        //
        ScanningEncode(nChannel);
    }else{
        AutoEncode(nChannel);
        //打开用户信息输入控件
        SetChannelStateToNew(nChannel,true);
    }

}

/********************************************************
 *@Name:        AutoEncode
 *@Author:      HuaT
 *@Description: 新建时自动输入编码
 *@Param1:      通道号
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-9-9
********************************************************/
void TestWindow::AutoEncode(int nChannel)
{
    //自动输入编号
    quint64 nCurMaxNumber = GetValidNumber();
    if(nCurMaxNumber == -1){
        qDebug()<<"自动输入编号出错,可能原因为:查询出错!";
    }else{
        switch (nChannel) {
        case 1:
            //if(ui->leNumber1->isEnabled() && !ui->leNumber1->text().isEmpty())
            if(ui->leNumber1->isEnabled() && !ui->leNumber1->text().isEmpty()){
                return;
            }else{
                //新建之前需清除上一次测试信息,如果已经打开则不清
                CleareChannelInfo(nChannel);
            }
            if(ui->leNumber2->isEnabled() && ui->leNumber3->isEnabled()){
                m_nValidNumber = nCurMaxNumber + 3;
            }else if(ui->leNumber2->isEnabled() || ui->leNumber3->isEnabled()){
                m_nValidNumber = nCurMaxNumber + 2;
            }else{
                m_nValidNumber = nCurMaxNumber + 1;
            }
            ui->leNumber1->setText(QString("%1").arg( m_nValidNumber ));
            //m_Candata1.m_nNumberID = m_nValidNumber;
            break;
        case 2:
            if(ui->leNumber2->isEnabled()){
                return;
            }else{
                //新建之前需清除上一次测试信息,如果已经打开则不清
                CleareChannelInfo(nChannel);
            }
            if(ui->leNumber1->isEnabled() && ui->leNumber3->isEnabled()){
                m_nValidNumber = nCurMaxNumber + 3;
            }else if(ui->leNumber1->isEnabled() || ui->leNumber3->isEnabled()){
                m_nValidNumber = nCurMaxNumber + 2;
            }else{
                m_nValidNumber = nCurMaxNumber + 1;
            }
            ui->leNumber2->setText(QString("%1").arg( m_nValidNumber ));
            //m_Candata2.m_nNumberID = m_nValidNumber;
            break;
        case 3:
            if(ui->leNumber3->isEnabled()){
                return;
            }else{
                //新建之前需清除上一次测试信息,如果已经打开则不清
                CleareChannelInfo(nChannel);
            }
            if(ui->leNumber1->isEnabled() && ui->leNumber2->isEnabled()){
                m_nValidNumber = nCurMaxNumber + 3;
            }else if(ui->leNumber1->isEnabled() || ui->leNumber2->isEnabled()){
                m_nValidNumber = nCurMaxNumber + 2;
            }else{
                m_nValidNumber = nCurMaxNumber + 1;
            }
            ui->leNumber3->setText(QString("%1").arg( m_nValidNumber ));
            //m_Candata3.m_nNumberID = m_nValidNumber;
            break;
        default:
            break;
        }
    }
}

/********************************************************
 *@Name:        ScanningEncode
 *@Author:      HuaT
 *@Description: 扫描输入编码模式,使用扫描枪输入编码时,编码不自动填写,点新建时清除上次信息,
 *              弹出输入框,等待用户自己输入或者扫码输入
 *@Param1:      通道号
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-9-9
********************************************************/
void TestWindow::ScanningEncode(int nChannel)
{
    CleareChannelInfo(nChannel);
    switch(nChannel){
    case 1:
        ui->leNumber1->setFocus();
        break;
    case 2:
        ui->leNumber2->setFocus();
        break;
    case 3:
        ui->leNumber3->setFocus();
        break;
    default:
        break;
    }
}

/********************************************************
 *@Name:        AutoPrint
 *@Author:      HuaT
 *@Description: 出结果时，自动打印结果数据
 *@Param1:      通道号
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-30
********************************************************/
void TestWindow::AutoPrint(int nChannel)
{
    QString strName,strNumber,strAge,strSex,strItem,strResult;
    switch(nChannel){
    case 1:
        strName = ui->leName1->text();
        strNumber = ui->leNumber1->text();
        strAge = ui->leAge1->text();
        strSex = ui->cBoxSex1->currentText();
        strItem = ui->leItem1->text();
        strResult = ui->leResult1->text();
        break;
    case 2:
        strName = ui->leName2->text();
        strNumber = ui->leNumber2->text();
        strAge = ui->leAge2->text();
        strSex = ui->cBoxSex2->currentText();
        strItem = ui->leItem2->text();
        strResult = ui->leResult2->text();
        break;
    case 3:
        strName = ui->leName3->text();
        strNumber = ui->leNumber3->text();
        strAge = ui->leAge3->text();
        strSex = ui->cBoxSex3->currentText();
        strItem = ui->leItem3->text();
        strResult = ui->leResult3->text();
        break;
    }
    int nLanguage = m_SetParam->value(LANGUAGESET,"1").toInt(0);
    //检测时间
    QString strCheckTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //查询获取参考值
    quint8 nSex;
    if(strSex.compare(QObject::tr("Male")) == 0){
        nSex = 1;
    }else if(strSex.compare(QObject::tr("Female")) == 0){
        nSex =2;
    }else{
        nSex = 0;
    }
    QString strSql = QString("select * from renfvalue where item='%1' and sex=%2").arg(strItem).arg(nSex);
    QStringList listRenfValue = m_db->ExecQuery(strSql);
    QString strRenf = m_settings->GetRenfValue(strAge,listRenfValue);
    QString strFlag = m_settings->GetResultFlag(strRenf,strResult);
    if(nLanguage == 0){
        m_settings->PrintChineseData(m_SerialPrint,strName,strNumber,
                                     strAge,strSex,strItem,strResult,strCheckTime,strRenf,strFlag);
    }else{
        m_settings->PrintEnglishData(m_SerialPrint,strName,strNumber,
                                     strAge,strSex,strItem,strResult,strCheckTime,strRenf,strFlag);
    }
}

/********************************************************
 *@Name:        GetRenfValue
 *@Author:      HuaT
 *@Description: 通过病人信息，获取参考值
 *@Param1:      病人年龄
 *@Param2:      病人性别
 *@Param3:      病人测试项目
 *@Return:      对应病人结果的参考值
 *@Version:     1.0
 *@Date:        2016-12-29
********************************************************/
/*QString TestWindow::GetRenfValue(QString strAge, QString strSex, QString strItem)
{
    QString strSql;
    QStringList listInfo;
    QString strRenf;
    quint8 nSex;
    quint8 nAge = strAge.toInt();
    if(strSex.compare(QObject::tr("Male")) == 0){
        nSex = 1;
    }else if(strSex.compare(QObject::tr("Female")) == 0){
        nSex =2;
    }else{
        nSex = 0;
    }
    strSql = QString("select * from renfvalue where item='%1' and sex=%2").arg(strItem).arg(nSex);
    listInfo = m_db->ExecQuery(strSql);
    if(listInfo.size() == 0){
        return strRenf;
    }
    quint32 nAgeLow,nAgeHight;
    for(int n=0; n<listInfo.size(); n+=5){
        if(listInfo.at(n+2).isEmpty()){
            nAgeLow = 0;
            nAgeHight = listInfo.at(n+3).toInt();
            if(nAge <= nAgeHight){
                strRenf = listInfo.at(n+1);
            }
        }else if(listInfo.at(n+3).isEmpty()){
            nAgeLow = listInfo.at(n+2).toInt();
            nAgeHight = 0;
            if(nAge >= nAgeLow){
                strRenf = listInfo.at(n+1);
            }
        }else{
            nAgeLow = listInfo.at(n+2).toInt();
            nAgeHight = listInfo.at(n+3).toInt();
            if(nAge>=nAgeLow && nAge<=nAgeHight){
                strRenf = listInfo.at(n+1);
            }
        }
    }
    return strRenf;
}*/



/********************************************************
 *@Name:        ChangeAutoModeStates
 *@Author:      HuaT
 *@Description: 更改自动测试模式状态
 *@Param1:      通道号
 *@Param2:      对应测试状态
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-31
********************************************************/
void TestWindow::ChangeAutoModeStates(int nChannel, bool States)
{
    switch(nChannel){
    case 1:
        m_bAutoMode1 = States;
        break;
    case 2:
        m_bAutoMode2 = States;
        break;
    case 3:
        m_bAutoMode3 = States;
        break;
    }
}

/********************************************************
 *@Name:        GetAPOffSetInfo
 *@Author:      HuaT
 *@Description: 获取放大偏移量参数
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-31
********************************************************/
void TestWindow::GetAPOffSetInfo()
{
    QByteArray byteID,byteData;
    byteID[0] = QT_TESTOPT_GETAPOFFSET;
    byteID[1] = 0;
    byteID[2] = QT_HOST_MASTER;
    byteID[3] = QT_HOST_SLAVE1;
#ifdef Q_OS_LINUX
    m_CanBus.SendMsg(byteID,byteData.size(),byteData);
#endif
    byteID[3] = QT_HOST_SLAVE2;
#ifdef Q_OS_LINUX
    m_CanBus.SendMsg(byteID,byteData.size(),byteData);
#endif
    byteID[3] = QT_HOST_SLAVE3;
#ifdef Q_OS_LINUX
    m_CanBus.SendMsg(byteID,byteData.size(),byteData);
#endif

}

/********************************************************
 *@Name:        GetHardVersion
 *@Author:      HuaT
 *@Description: 发送命令获取硬件版本信息
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-1-18
********************************************************/
void TestWindow::GetHardVersion()
{
    QByteArray byteID,byteData;
    byteID[0] = QT_SYSOPT_GETDEVINFO;
    byteID[1] = 0;
    byteID[2] = QT_HOST_MASTER;
    byteID[3] = QT_HOST_SLAVE1;
#ifdef Q_OS_LINUX
    m_CanBus.SendMsg(byteID,byteData.size(),byteData);
#endif
}

/********************************************************
 *@Name:        on_btnAutoTest1_clicked
 *@Author:      HuaT
 *@Description: 自动测试1按钮事件
 *@Param1:      通道号
 *@Param2:      测试状态
 *@Param3:      结果数据对象
 *@Param4:      通道1计时器
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-7
********************************************************/
void TestWindow::on_btnAutoTest1_clicked()
{
    AutoTest(1, m_bAutoMode1, m_Candata1, m_timerClock1);
}

/********************************************************
 *@Name:        InsertTestInfoToDb
 *@Author:      HuaT
 *@Description: 插入用户测试信息到数据库，此信息为用户信息，没有结果信息
 *@Param:       用户测试的通道号
 *@Return:      0-无错误；1-编号已经存在；2-数据库插入出错
 *@Version:     1.0
 *@Date:        2016-7-7
********************************************************/
int TestWindow::InsertTestInfoToDb(int nChannel)
{
    QString sqlCmd;
    quint64 nNumber;
    QString Name;
    int Sex;
    int Age;
    switch(nChannel){
    case 1:
        nNumber = ui->leNumber1->text().toLongLong(0);
        Name = ui->leName1->text();
        Sex = ui->cBoxSex1->currentIndex();
        Age = ui->leAge1->text().toInt(0);
        m_Candata1.m_nNumberID = nNumber;
        m_Candata1.m_strName = Name;
        m_Candata1.m_strAge = QString::number(Age);
        if(Sex == 1){
            m_Candata1.m_strSex = "M";
        }else if(Sex == 2){
            m_Candata1.m_strSex = "F";
        }else{
            m_Candata1.m_strSex = "U";
        }
        break;
    case 2:
        nNumber = ui->leNumber2->text().toLongLong(0);
        Name = ui->leName2->text();
        Sex = ui->cBoxSex2->currentIndex();
        Age = ui->leAge2->text().toInt(0);
        m_Candata2.m_nNumberID = nNumber;
        m_Candata2.m_strName = Name;
        m_Candata2.m_strAge = QString::number(Age);
        if(Sex == 1){
            m_Candata2.m_strSex = "M";
        }else if(Sex == 2){
            m_Candata2.m_strSex = "F";
        }else{
            m_Candata2.m_strSex = "U";
        }
        break;
    case 3:
        nNumber = ui->leNumber3->text().toLongLong(0);
        Name = ui->leName3->text();
        Sex = ui->cBoxSex3->currentIndex();
        Age = ui->leAge3->text().toLongLong(0);
        m_Candata3.m_nNumberID = nNumber;
        m_Candata3.m_strName = Name;
        m_Candata3.m_strAge = QString::number(Age);
        if(Sex == 1){
            m_Candata3.m_strSex = "M";
        }else if(Sex == 2){
            m_Candata3.m_strSex = "F";
        }else{
            m_Candata3.m_strSex = "U";
        }
        break;
    default:
        break;
    }
    bool bEixts = CheckInputInfo(nNumber);
    if(!bEixts){
        return 1;
    }else{
        sqlCmd = QString("insert into patient values(null,%1,\
                         '%2',%3,%4,'','',datetime('now','localtime'),'',%5)")\
                .arg(nNumber).arg(Name).arg(Age).arg(Sex).arg(nChannel);
        bool bExec  = true;
        //qDebug()<<sqlCmd;
        bExec = m_Query->exec(sqlCmd);
        if(!bExec){
            //msgContain = QObject::tr("");
            qDebug()<<"on_btnAutoTest1_clicked：插入数据失败，请检查数据库";
            return 2;
        }
        return 0;
    }
}

/********************************************************
 *@Name:        on_btnInstantTest1_clicked
 *@Author:      HuaT
 *@Description: 即时测试1按钮事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-7
********************************************************/
void TestWindow::on_btnInstantTest1_clicked()
{
    InstantTest(1);
}

/********************************************************
 *@Name:        on_btnNew2_clicked
 *@Author:      HuaT
 *@Description: 新建2按钮单击事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-4
********************************************************/
void TestWindow::on_btnNew2_clicked()
{
    NewTest(2);
}

/********************************************************
 *@Name:        on_btnPrint2_clicked
 *@Author:      HuaT
 *@Description: 打印2按钮单击事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-7
********************************************************/
void TestWindow::on_btnPrint2_clicked()
{
    QString strName,strNumber,strAge,strSex,strItem,strResult;
    strName = ui->leName2->text();
    strNumber = ui->leNumber2->text();
    strAge = ui->leAge2->text();
    strSex = ui->cBoxSex2->currentText();
    strItem = ui->leItem2->text();
    strResult = ui->leResult2->text();
    int nLanguage = m_SetParam->value(LANGUAGESET,"1").toInt(0);
    QString strCheckTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //计算参考值和参考标志
    quint8 nSex;
    if(strSex.compare(QObject::tr("Male")) == 0){
        nSex = 1;
    }else if(strSex.compare(QObject::tr("Female")) == 0){
        nSex =2;
    }else{
        nSex = 0;
    }
    QString strSql = QString("select * from renfvalue where item='%1' and sex=%2").arg(strItem).arg(nSex);
    QStringList listRenfValue = m_db->ExecQuery(strSql);
    QString strRenf = m_settings->GetRenfValue(strAge,listRenfValue);
    QString strFlag = m_settings->GetResultFlag(strRenf,strResult);
    if(nLanguage == 0){
        m_settings->PrintChineseData(m_SerialPrint,strName,strNumber,
                                     strAge,strSex,strItem,strResult,
                                     strCheckTime,strRenf,strFlag);
    }else{
        m_settings->PrintEnglishData(m_SerialPrint,strName,strNumber,
                                     strAge,strSex,strItem,strResult,
                                     strCheckTime,strRenf,strFlag);
    }
}

/********************************************************
 *@Name:        on_btnAutoTest2_clicked
 *@Author:      HuaT
 *@Description: 自动测试2按钮事件
 *@Param1:      通道号
 *@Param2:      测试状态
 *@Param3:      结果数据对象
 *@Param4:      通道1计时器
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-7
********************************************************/
void TestWindow::on_btnAutoTest2_clicked()
{
    AutoTest(2, m_bAutoMode2, m_Candata2, m_timerClock2);
}

/********************************************************
 *@Name:        on_btnInstantTest2_clicked
 *@Author:      HuaT
 *@Description: 即时测试2按钮事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-7
********************************************************/
void TestWindow::on_btnInstantTest2_clicked()
{
    InstantTest(2);
}

/********************************************************
 *@Name:        on_btnNew3_clicked
 *@Author:      HuaT
 *@Description: 新建3按钮单击事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-4
********************************************************/
void TestWindow::on_btnNew3_clicked()
{
    NewTest(3);
}

/********************************************************
 *@Name:        on_btnPrint3_clicked
 *@Author:      HuaT
 *@Description: 打印3按钮单击事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-7
********************************************************/
void TestWindow::on_btnPrint3_clicked()
{
    QString strName,strNumber,strAge,strSex,strItem,strResult;
    strName = ui->leName3->text();
    strNumber = ui->leNumber3->text();
    strAge = ui->leAge3->text();
    strSex = ui->cBoxSex3->currentText();
    strItem = ui->leItem3->text();
    strResult = ui->leResult3->text();
    int nLanguage = m_SetParam->value(LANGUAGESET,"1").toInt(0);
    QString strCheckTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //计算参考值和参考标志
    quint8 nSex;
    if(strSex.compare(QObject::tr("Male")) == 0){
        nSex = 1;
    }else if(strSex.compare(QObject::tr("Female")) == 0){
        nSex =2;
    }else{
        nSex = 0;
    }
    QString strSql = QString("select * from renfvalue where item='%1' and sex=%2").arg(strItem).arg(nSex);
    QStringList listRenfValue = m_db->ExecQuery(strSql);
    QString strRenf = m_settings->GetRenfValue(strAge,listRenfValue);
    QString strFlag = m_settings->GetResultFlag(strRenf,strResult);
    if(nLanguage == 0){
        m_settings->PrintChineseData(m_SerialPrint,strName,strNumber,
                                     strAge,strSex,strItem,strResult,
                                     strCheckTime,strRenf,strFlag);
    }else{
        m_settings->PrintEnglishData(m_SerialPrint,strName,strNumber,
                                     strAge,strSex,strItem,strResult,
                                     strCheckTime,strRenf,strFlag);
    }
}

/********************************************************
 *@Name:        on_btnAutoTest3_clicked
 *@Author:      HuaT
 *@Description: 自动测试3按钮事件
 *@Param1:      通道号
 *@Param2:      测试状态
 *@Param3:      结果数据对象
 *@Param4:      通道1计时器
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-7
********************************************************/
void TestWindow::on_btnAutoTest3_clicked()
{
    AutoTest(3, m_bAutoMode3, m_Candata3, m_timerClock3);
}

/********************************************************
 *@Name:        on_btnInstantTest3_clicked
 *@Author:      HuaT
 *@Description: 即时测试3按钮事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-7
********************************************************/
void TestWindow::on_btnInstantTest3_clicked()
{
    InstantTest(3);
}

/********************************************************
 *@Name:        RecvCanMsg
 *@Author:      HuaT
 *@Description: Can消息响应事件
 *@Param1:      Can ID
 *@Param2:      Can数据内容
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-14
********************************************************/
void TestWindow::RecvCanMsg(quint32 can_id, QByteArray can_data)
{
    QByteArray qbaID;
#ifdef Q_OS_LINUX
    qbaID = m_CanBus.UIntIDToByte(can_id);
#endif
    //QString msgContain,strTemp;
    //qDebug()<<qbaID.toHex().toUpper() << can_data.size() << can_data.toHex().toUpper();
    //strTemp = qbaID.toHex().toUpper() + can_data.toHex().toUpper();
    //msgContain = QString("%1 %2").arg(strTemp).arg(can_data.size());
    //QMessageBox::information(this,"",msgContain,QMessageBox::Ok);
    quint8 nCMD = (quint8)qbaID.at(0);
    quint8 nSync = (quint8)qbaID.at(1);
    quint8 nChannel = (quint8)qbaID.at(2);
    switch(nCMD){
    case QT_SYSOPT_ACK:
        ParseACK(nSync,can_data,nChannel);
        break;
    case QT_SYSOPT_GETDEVINFO:
        break;
    case QT_SYSOPT_GETLINKSTATUS:
        break;
    case QT_SYSOPT_ALARM:
    {
        //发送响应ACK
        QByteArray byteRespond;
        PackageCanMsg(QT_SYSOPT_ACK,QT_SYSOPT_ALARM,nChannel,byteRespond);
        //处理报警信息
        ProcessAlarm(nChannel,can_data);
        //恢复至新建时的按钮状态
        InitChannelState(nChannel,false);
        //得到结果，更新打印按钮
        UPdatePrintState(nChannel);
        //更新测试按钮文本
        UpdateTestText(nChannel);
        break;
    }
    //准备升级
    case QT_SYSOPT_READYUPDATA:
    {
        if(m_byteHardUpdateData1.count() == 0){
            QString strTilte,strContain;
            strTilte = QObject::tr("Note");
            strContain = QObject::tr("updateing ");
            QMessageBox::information(this,strTilte,strContain,QMessageBox::Ok);
            break;
        }
        //发送响应ACK
        QByteArray byteRespond;
        PackageCanMsg(QT_SYSOPT_ACK,QT_SYSOPT_READYUPDATA,nChannel,byteRespond);
        //开始发送升级数据
        StartHardUpdate(nChannel,0);
        break;
    }
    case QT_TESTOPT_SETUPTEST:
        break;
    case QT_TESTOPT_STARTTEST:
        break;
    case QT_TESTOPT_TESTDATA:
    {
        //解析各通道发来的结果数据
        switch(nChannel){
        case 1:
            ParseTestData(nSync,can_data,nChannel,m_Candata1);
            break;
        case 2:
            ParseTestData(nSync,can_data,nChannel,m_Candata2);
            break;
        case 3:
            ParseTestData(nSync,can_data,nChannel,m_Candata3);
            break;
        default:
            break;
        }
        //发送响应ACK
        QByteArray byteRespond;
        PackageCanMsg(QT_SYSOPT_ACK,QT_TESTOPT_TESTDATA,nChannel,byteRespond);
        break;
    }
    case QT_TESTOPT_STOPTEST:
        break;
    case QT_TESTOPT_BARCODE:
    {
        if(nChannel == 1){
            ProcessBarCode(1, m_TestStatus1, m_Candata1, can_data);
            ui->wgRoundBar1->setRange(0,m_Candata1.m_nReactionTime);
            if(m_bAutoMode1){
                //开始计时
                m_timerClock1.start(1000);
            }
        }else if(nChannel == 2){
            ProcessBarCode(2, m_TestStatus2, m_Candata2, can_data);
            ui->wgRoundBar2->setRange(0,m_Candata2.m_nReactionTime);
            if(m_bAutoMode2){
                m_timerClock2.start(1000);
            }
        }else if(nChannel == 3){
            ProcessBarCode(3, m_TestStatus3, m_Candata3, can_data);
            ui->wgRoundBar3->setRange(0,m_Candata3.m_nReactionTime);
            if(m_bAutoMode3){
                m_timerClock3.start(1000);
            }
        }
        break;
    }
    default:
        break;
    }

}

/********************************************************
 *@Name:          ParseACK
 *@Author:        HuaT
 *@Description:   解析仪器返回的ACK数据
 *@Param:         响应的命令号
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-7-25
********************************************************/
void TestWindow::ParseACK(int nSync,QByteArray can_data,quint8 nChannel)
{
    switch (nSync) {
    //设备信息读取
    case QT_SYSOPT_GETDEVINFO:
    {
        quint8 nHigh = (quint8)can_data[0];
        quint8 nMid = (quint8)can_data[1];
        quint8 nLow = (quint8)can_data[2];
        QString strHardVersion = QString("V%1.%2.%3").arg(nHigh).arg(nMid).arg(nLow);
        m_settings->SetParam(HARDVERSION,strHardVersion);
        m_settings->WriteSettingsInfoToMap();
        break;
    }
    //请求升级ACK
    case QT_SYSOPT_REQUESTUPDATA:
    {
        break;
    }
    //开始升级ACK
    case QT_SYSOPT_STARTUPDATA:
    {
        quint8 nIndex = can_data[0];
        StartHardUpdate(nChannel,nIndex+1);
    }
    case QT_TESTOPT_SETUPTEST:
    {
        m_bSetupTestFlag = false;
        break;
    }
    case QT_TESTOPT_STARTTEST:
    {
        m_bStartTestFlag = false;
        break;
    }
    case QT_TESTOPT_STOPTEST:
    {
        m_bStopTestFlag = false;
        break;
    }
    //仪器关闭
    case QT_TESTOPT_SHUTDOWN:
    {
#ifdef Q_OS_LINUX
        sleep(3);
        qApp->exit(0);
        system("poweroff");
        break;
#endif
    }
    //收到设置偏移量ACK
    case QT_TESTOPT_SETAPOFFSET:
    {
        QString strTitle,strContent;
        strTitle = "提示";
        strContent = QString("通道%1更改成功").arg(nChannel);
        QMessageBox::information(this,strTitle,strContent);
    }
    //获取通道偏移量
    case QT_TESTOPT_GETAPOFFSET:
    {
        quint8 nAPOffSet = (quint8)can_data[0];
        switch(nChannel){
        case 1:
            m_settings->SetParam(APOFFSET1,QString::number(nAPOffSet));
            break;
        case 2:
            m_settings->SetParam(APOFFSET2,QString::number(nAPOffSet));
            break;
        case 3:
            m_settings->SetParam(APOFFSET3,QString::number(nAPOffSet));
            break;
        }
        m_settings->WriteSettingsInfoToMap();
    }
    //
    default:
        break;
    }
}

/********************************************************
 *@Name:          StartHardUpdate
 *@Author:        HuaT
 *@Description:   开始发送固件升级数据
 *@Param1:        发实的通道号
 *@Param2:        发送的数据索引
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-11-18
********************************************************/
void TestWindow::StartHardUpdate(quint8 nChannel,quint32 nIndex)
{
    switch(nChannel){
    case 1:
    {
        QByteArray byteHardData;
        if(nIndex == 0){
            m_nUpdataDataLeng1 = m_byteHardUpdateData1.count();
            m_nIndex1 = 0;
            m_pHardGrading->SetProgressRange(0,m_nUpdataDataLeng1);
            m_pHardGrading->SetProgressValue(0);
            byteHardData[0] = 0;
            byteHardData[1] = 0;
            byteHardData[2] = 0;
            byteHardData[3] = 0;
            byteHardData[4] = m_nUpdataDataLeng1%256;
            byteHardData[5] = (m_nUpdataDataLeng1/256)%256;
            byteHardData[6] = (m_nUpdataDataLeng1/256/256)%256;
            byteHardData[7] = (m_nUpdataDataLeng1/256/256/256)%256;
            PackageCanMsg(QT_SYSOPT_STARTUPDATA,0,nChannel,byteHardData);
            //m_bIsStart = false;
            return;
        }
        if((m_nIndex1*8) >= m_nUpdataDataLeng1){
            //m_bIsStart = true;
            m_byteHardUpdateData1.clear();
            m_nIndex1 = 0;
            m_pHardGrading->SetProgressValue(m_nUpdataDataLeng1);
            QString strTitle,strContain;
            strTitle = QObject::tr("Note");
            strContain = QObject::tr("Upgrade is complete");
            QMessageBox::information(this,strTitle,strContain,QMessageBox::Ok);
            //qDebug()<<"m_nindex1: - "<<m_nIndex1;
            return;
        }
        for(int n=m_nIndex1*8,i=0; n<m_nIndex1*8+8 && n<m_nUpdataDataLeng1; n++,i++){
            byteHardData[i] = (quint8)m_byteHardUpdateData1.at(n);
        }
        if(nIndex == 256){
            nIndex = 1;
            PackageCanMsg(QT_SYSOPT_STARTUPDATA,nIndex,nChannel,byteHardData);
        }else{
            PackageCanMsg(QT_SYSOPT_STARTUPDATA,nIndex,nChannel,byteHardData);
        }
        m_pHardGrading->SetProgressValue(m_nIndex1*8);
        m_nIndex1++;
        //qDebug()<<m_nIndex1;
        break;
    }
    case 2:
    {
        QByteArray byteHardData;
        if(nIndex == 0){
            m_nUpdataDataLeng2 = m_byteHardUpdateData2.count();
            m_nIndex2 = 0;
            byteHardData[0] = 0;
            byteHardData[1] = 0;
            byteHardData[2] = 0;
            byteHardData[3] = 0;
            byteHardData[4] = m_nUpdataDataLeng2%256;
            byteHardData[5] = (m_nUpdataDataLeng2/256)%256;
            byteHardData[6] = (m_nUpdataDataLeng2/256/256)%256;
            byteHardData[7] = (m_nUpdataDataLeng2/256/256/256)%256;
            PackageCanMsg(QT_SYSOPT_STARTUPDATA,0,nChannel,byteHardData);
            //m_bIsStart = false;
            return;
        }
        if((m_nIndex2*8) >= m_nUpdataDataLeng2){
            //m_bIsStart = true;
            m_byteHardUpdateData2.clear();
            m_nIndex2 = 0;
            //qDebug()<<"m_nindex2: - "<<m_nIndex1;
            return;
        }
        for(int n=m_nIndex2*8,i=0; n<m_nIndex2*8+8 && n<m_nUpdataDataLeng2; n++,i++){
            byteHardData[i] = (quint8)m_byteHardUpdateData2.at(n);
        }
        if(nIndex == 256){
            nIndex = 1;
            PackageCanMsg(QT_SYSOPT_STARTUPDATA,nIndex,nChannel,byteHardData);
        }else{
            PackageCanMsg(QT_SYSOPT_STARTUPDATA,nIndex,nChannel,byteHardData);
        }
        m_nIndex2++;
        //qDebug()<<m_nIndex2;
        break;
    }
    case 3:
    {
        QByteArray byteHardData;
        if(nIndex == 0){
            m_nUpdataDataLeng3 = m_byteHardUpdateData3.count();
            m_nIndex3 = 0;
            byteHardData[0] = 0;
            byteHardData[1] = 0;
            byteHardData[2] = 0;
            byteHardData[3] = 0;
            byteHardData[4] = m_nUpdataDataLeng3%256;
            byteHardData[5] = (m_nUpdataDataLeng3/256)%256;
            byteHardData[6] = (m_nUpdataDataLeng3/256/256)%256;
            byteHardData[7] = (m_nUpdataDataLeng3/256/256/256)%256;
            PackageCanMsg(QT_SYSOPT_STARTUPDATA,0,nChannel,byteHardData);
            //m_bIsStart = false;
            return;
        }
        if((m_nIndex3*8) >= m_nUpdataDataLeng3){
            //m_bIsStart = true;
            m_byteHardUpdateData3.clear();
            m_nIndex3 = 0;
            //qDebug()<<"m_nindex3: - "<<m_nIndex3;
            return;
        }
        for(int n=m_nIndex3*8,i=0; n<m_nIndex3*8+8 && n<m_nUpdataDataLeng3; n++,i++){
            byteHardData[i] = (quint8)m_byteHardUpdateData3.at(n);
        }
        if(nIndex == 256){
            nIndex = 1;
            PackageCanMsg(QT_SYSOPT_STARTUPDATA,nIndex,nChannel,byteHardData);
        }else{
            PackageCanMsg(QT_SYSOPT_STARTUPDATA,nIndex,nChannel,byteHardData);
        }
        m_nIndex3++;
        //qDebug()<<m_nIndex3;
        break;
    }
    default:
        break;
    }
}

/********************************************************
 *@Name:          PorcessBarCode
 *@Author:        HuaT
 *@Description:   处理接收到的条码数据
 *@Param1:        发来的通道号
 *@Param2:        当前通道的测试状态
 *@Param3:        结果数据结构体对象
 *@Param4:        当前帧数据
 *@Return:        是否获取到ID卡信息，如果没有查询信息则返回false
 *@Version:       1.0
 *@Date:          2016-7-25
********************************************************/
bool TestWindow::ProcessBarCode(quint8 nChannel, bool& bTestStatus, ResultDataInfo &data, QByteArray can_data)
{
    //获得条码编号,先用新条码规则,新的条码格式为10100000000101
    data.m_strIDCardBarCode = ParseBarCode(can_data,true);
    //查询此批号的ID卡数据
    data.m_strIDMessage = GetIDMessageInfo(data.m_strIDCardBarCode);
    if(data.m_strIDMessage.isEmpty()){//如果是空,则调用旧条码规则,旧的条码格式为010001000000000010001
        data.m_strIDCardBarCode = ParseBarCode(can_data,false);
        data.m_strIDMessage = GetIDMessageInfo(data.m_strIDCardBarCode);
    }
    //开始解析条码
    QString strTestMsg = QString("[通道%1] 开始解析条码,条码号为:%2").arg(nChannel).arg(data.m_strIDCardBarCode);
    qDebug()<<strTestMsg;

    QString strTitle,strContent,strLog;
    strTitle = QObject::tr("Note");
    if(data.m_strIDMessage.isEmpty()){
        //没找到批号,回复仪器
        QByteArray byteRespond;
        byteRespond[0] = QT_BARCODE_INVALID;
        PackageCanMsg(QT_SYSOPT_ACK,QT_TESTOPT_BARCODE,nChannel,byteRespond);
        //提示用户导入ID卡数据
        strContent = QObject::tr("There is no corresponding ID information, please import the relevant ID card");
        QMessageBox::information(this,strTitle,strContent,QMessageBox::Ok);
        //数据记录
        strLog = QString("无对应ID信息:%1,请导入对应ID卡信息").arg(data.m_strIDCardBarCode);
        InsertLogMsg(QT_LOGOPT_ERROR, data.m_nNumberID, strLog);
        //测试状态取消
        bTestStatus = false;
        //自动测试模式关闭
        ChangeAutoModeStates(nChannel,false);
        //恢复按钮状态
        //恢复至新建时的按钮状态
        InitChannelState(nChannel,false);
        //得到结果，更新打印按钮
        UPdatePrintState(nChannel);
        //更新测试按钮文本
        UpdateTestText(nChannel);
        //由于不能获取质控状态，不管是不是质控测试，出现异常恢复质控按钮状态
        emit UpdateQCControlStatus();
        //无射频信息结束测试
        QString strTestMsg = QString("[通道%1] 无条码号为:%2的射频信息,本次测试结束").arg(nChannel).arg(data.m_strIDCardBarCode);
        qDebug()<<strTestMsg;
        return false;
    }
    //转换批号为无符号型指针
    unsigned char *cIDMessage = m_ResultCalc.ConvetIDCardToChar(data.m_strIDMessage);
    //新旧卡标志
    if(data.m_strIDMessage.size() > 400){
        //data.m_nCardFlag = (quint8)pIDMessage[39];
        data.m_nCardFlag = 1;
    }else{
        data.m_nCardFlag = 0;
    }
    //解析ID卡数据
    quint8 nStartPoint,nAmp;
    ParseIDMessageInfo(cIDMessage,nStartPoint,nAmp,data);
    //检测有效日期
    bool bRet = CheckValidDate(data.m_strValidDate);
    if(!bRet){
        //试剂过期,回复仪器
        QByteArray byteRespond;
        //当条码无效时仪器不管1-2字节的内容
        byteRespond[0] = QT_BARCODE_INVALID;
        PackageCanMsg(QT_SYSOPT_ACK,QT_TESTOPT_BARCODE,nChannel,byteRespond);
        //提示用户试剂卡已过期
        strContent = QObject::tr("Reagent Strip has expired");
        QMessageBox::information(this,strTitle,strContent,QMessageBox::Ok);
        //测试状态取消
        bTestStatus = false;
        //自动测试模式关闭
        ChangeAutoModeStates(nChannel,false);
        //恢复至新建时的按钮状态
        InitChannelState(nChannel,false);
        //得到结果，更新打印按钮
        UPdatePrintState(nChannel);
        //更新测试按钮文本
        UpdateTestText(nChannel);
        //由于不能获取质控状态，不管是不是质控测试，出现异常恢复质控按钮状态
        emit UpdateQCControlStatus();
        //试剂过期,本次测试结束
        QString strTestMsg = QString("[通道%1] 试剂过期,本次测试结束").arg(nChannel);
        qDebug()<<strTestMsg;
        return false;
    }
    //显示测试项目
    ShowTestItem(nChannel,data.m_strTestName);
    //找到批号,响应仪器
    QByteArray byteRespond;
    byteRespond[0] = QT_BARCODE_VALID;
    byteRespond[1] = nStartPoint;
    byteRespond[2] = nAmp;
    PackageCanMsg(QT_SYSOPT_ACK,QT_TESTOPT_BARCODE,nChannel,byteRespond);
    //发送开始测试信息
    strTestMsg = QString("[通道%1] 条码解析完成,发送开始测试信息给仪器").arg(nChannel);
    qDebug()<<strTestMsg;
    return true;
}

/********************************************************
 *@Name:          CheckValidDate
 *@Author:        HuaT
 *@Description:   检测本次的试剂卡是否过期
 *@Param1:        有效期时间
 *@Return:        有效返回true，无效返回false
 *@Version:       1.0
 *@Date:          2016-8-10
********************************************************/
bool TestWindow::CheckValidDate(QString strValidDate)
{
    QDate ValidDate = QDate::fromString(strValidDate,"yyyy-MM-dd");
    QDate CurrentDate = QDate::currentDate();
    if(ValidDate.isValid()){
        if(ValidDate > CurrentDate){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

/********************************************************
 *@Name:          CheckTestStatus
 *@Author:        HuaT
 *@Description:   检测通道测试状态
 *@Param1:        无
 *@Return:        对应通道如果正在测试返回true，否则返回false
 *@Version:       1.0
 *@Date:          2016-8-14
********************************************************/
bool TestWindow::CheckTestStatus(quint8 nChannel)
{
    QString strTitle,strContent;
    strTitle = QObject::tr("Note");
    bool bRet = false;
    switch (nChannel) {
    case 1:
        bRet = m_TestStatus1;
        break;
    case 2:
        bRet = m_TestStatus2;
        break;
    case 3:
        bRet = m_TestStatus3;
        break;
    }
    if(bRet){
        switch (nChannel) {
        case 1:
            strContent = QObject::tr("Channel1 is busy");
            break;
        case 2:
            strContent = QObject::tr("Channel2 is busy");
            break;
        case 3:
            strContent = QObject::tr("Channel3 is busy");
            break;
        default:
            break;
        }
        QMessageBox::information(this,strTitle,strContent,QMessageBox::Ok);
    }
    return bRet;
}


/********************************************************
 *@Name:          ParseAlarm
 *@Author:        HuaT
 *@Description:   处理报警信息
 *@Param1:        发来的通道号
 *@Param2:        需要填充的数据结构体
 *@Return:        是否获取到ID卡信息，如果没有查询信息则返回false
 *@Version:       1.0
 *@Date:          2016-8-5
********************************************************/
void TestWindow::ProcessAlarm(quint8 nChannel,QByteArray byteCanData)
{
    if(0 == byteCanData.size()){
        return;
    }
    quint8 nAlarmType=0;
    if(2 == byteCanData.size()){
        nAlarmType = byteCanData.at(1);
    }
    //更新质控控件状态
    if(1 == nAlarmType){
        emit UpdateQCControlStatus();
    }
    quint8 nField = byteCanData.at(0);
    QString strTitle,strContent;
    strTitle = QObject::tr("Note");
    switch(nField){
    case 1:
        strContent = QObject::tr("Motor initialization failed,Please contact \ncustomer service");
        break;
    case 2:
        strContent = QObject::tr("Photoelectric switch A failed,Please contact \ncustomer service");
        break;
    case 3:
        strContent = QObject::tr("Photoelectric switch B failed,Please contact \ncustomer service");
        break;
    case 4:
        strContent = QObject::tr("Laser component failure,Please contact customer service");
        break;
    case 5:
        strContent = QObject::tr("No barcode,Please check whether the reagent \ncard is plugged or insert");
        break;
    case 6:
        strContent = QObject::tr("Wrong barcode,Please check whether the reagent \ncard is plugged or insert");
        break;
    }
    QMessageBox::information(this,strTitle,strContent,QMessageBox::Ok);
    //通道状态处理
    SetTestStatus(nChannel,false);
    //自动测试状态处理
    ChangeAutoModeStates(nChannel,false);
}

/********************************************************
 *@Name:          ShowTestItem
 *@Author:        HuaT
 *@Description:   显示测试项目到界面上
 *@Param1:        测试通道号
 *@Param2:        测试名称
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-7-28
********************************************************/
void TestWindow::ShowTestItem(quint8 nChannel, QString strTestName)
{
    switch(nChannel){
    case 1:
        ui->leItem1->setText(strTestName);
        break;
    case 2:
        ui->leItem2->setText(strTestName);
        break;
    case 3:
        ui->leItem3->setText(strTestName);
        break;
    default:
        break;
    }
}

/********************************************************
 *@Name:          ShowTestResult
 *@Author:        HuaT
 *@Description:   显示测试结果到界面上
 *@Param1:        测试通道号
 *@Param2:        测试结果
 *@Param3:        测试单位
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-7-28
********************************************************/
void TestWindow::ShowTestResult(quint8 nChannel, QString strTestResult, QString strTestUnit)
{
    QString strResult;
    strResult = strTestResult+strTestUnit;
    switch(nChannel){
    case 1:
        ui->leResult1->setText(strResult);
        break;
    case 2:
        ui->leResult2->setText(strResult);
        break;
    case 3:
        ui->leResult3->setText(strResult);
        break;
    default:
        break;
    }
}

/********************************************************
 *@Name:          SaveResultToDB
 *@Author:        HuaT
 *@Description:   保存指定的结果到数据库
 *@Param:         带单位的结果值-12.2mg/L
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-7-28
********************************************************/
void TestWindow::SaveResultToDB(ResultDataInfo DataObj)
{
    QString strRawData = DataObj.m_byteCanData.toHex().toUpper();
    //如果开启原始数据功能，则记录原始数据
    int nRawdata = m_SetParam->value(RAWDATAMODE,0).toInt();
    if(0 == nRawdata){
        //如果关闭,则把原始数据清空
        strRawData = "";
    }
    QString sqlCMD;
    sqlCMD = QString("update patient set item='%1',result='%2',\
                     testdate=datetime('now','localtime'),\
                     rawdata='%3' where number=%4 and date(testdate)=date('now','localtime')")
            .arg(DataObj.m_strTestName).arg(DataObj.m_strResult+DataObj.m_strTestUnit)
            .arg(strRawData).arg(DataObj.m_nNumberID);
    if(!m_Query->exec(sqlCMD)){
        QString msgTitle,msgContain;
        msgTitle = QObject::tr("Note");
        msgContain = QObject::tr("The result data save failed,please check the equipment");
        QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Ok);
    }
}


/********************************************************
 *@Name:          PackageCanMsg
 *@Author:        HuaT
 *@Description:   解析仪器返回的ACK数据
 *@Param:         响应的命令号
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-7-25
********************************************************/
void TestWindow::PackageCanMsg(quint8 nCMD,quint8 nSync, quint8 nChannel,QByteArray can_data)
{
    QByteArray byteID;
    byteID[0] = nCMD;
    byteID[1] = nSync;
    byteID[2] = QT_HOST_MASTER;
    switch(nChannel){
    case 1:
        byteID[3] = QT_HOST_SLAVE1;
        break;
    case 2:
        byteID[3] = QT_HOST_SLAVE2;
        break;
    case 3:
        byteID[3] = QT_HOST_SLAVE3;
        break;
    default:
        break;
    }
#ifdef Q_OS_LINUX
    m_CanBus.SendMsg(byteID,can_data.size(),can_data);
#endif
}

/********************************************************
 *@Name:          SetTestStatus
 *@Author:        HuaT
 *@Description:   设置测试状态
 *@Param1:        通道号
 *@Param2:        通道状态
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-12-14
********************************************************/
void TestWindow::SetTestStatus(int nChannel, bool bStatus)
{
    switch (nChannel) {
    case 1:
        m_TestStatus1 = bStatus;
        break;
    case 2:
        m_TestStatus2 = bStatus;
        break;
    case 3:
        m_TestStatus3 = bStatus;
        break;
    default:
        break;
    }
}

/********************************************************
 *@Name:          ParseTestData
 *@Author:        HuaT
 *@Description:   解析结果数据
 *@Param1:        仪器发来的同步字
 *@Param2:        测试数据
 *@Param3:        通道号
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-7-14
********************************************************/
void TestWindow::ParseTestData(quint8 nSync, QByteArray can_data,
                               quint8 nChannel,ResultDataInfo& DataObj)
{
    if(nSync == 0){
        DataObj.m_nDataLen = (quint8)can_data.at(0) + (quint8)can_data.at(1)*256;
        DataObj.m_nSyncID = 1;
        if(can_data.size() == 3){
            m_isQCResult = (quint8)can_data.at(2);
        }else{
            m_isQCResult = false;
        }
    }else{
        if(DataObj.m_nSyncID == nSync){
            //放入数据到数据组
            DataObj.m_byteCanData.append(can_data);
            //如果数据到达指定长度，则计算结果
            if(DataObj.m_nDataLen == (quint32)DataObj.m_byteCanData.size()){
                //计算结果
                //unsigned int ScanData1[320];
                //unsigned int ScanData2[320];
                unsigned char* pRecord = NULL;
                //仪器数据接收完成,开始进行结果计算
                QString strTestMsg = QString("[通道%1] 仪器数据接收完成,开始进行结果计算").arg(nChannel);
                qDebug()<<strTestMsg;
                //新旧卡标志区分
                if(0 == DataObj.m_nCardFlag){ //旧卡数据
                    pRecord = CalcResult(DataObj);
                    //for(int i=0; i<32; i++){
                    //    qDebug()<<i<<pRecord[i];
                    //}
                    //小数位数
                    quint8 nDecimal = QString::number((quint8)pRecord[10]).toInt();
                    QString strTempResult;
                    if(48 == nDecimal){
                        strTempResult = QString("%1").arg(pRecord[12]*256 + pRecord[13]);
                    }else{
                        strTempResult = QString("%1.%2").arg(pRecord[12]*256 + pRecord[13]).arg(pRecord[11]);
                    }
                    //最大值最小值标记
                    quint8 nFlag = QString::number((quint8)pRecord[9]).toInt();
                    //qDebug()<<"nFlag:"<<nFlag;
                    if(0 == nFlag){
                        DataObj.m_strResult = strTempResult;
                    }else if(1 == nFlag){
                        DataObj.m_strResult = QString("<%1").arg(strTempResult);
                    }else if(2 == nFlag){
                        DataObj.m_strResult = QString(">%1").arg(strTempResult);
                    }
                    DataObj.m_RawTestInfo.m_fTestResult = strTempResult.toDouble();
                }else { //新卡数据
                    DataObj.m_strResult = CalcResult(DataObj,true);
                    //
                    DataObj.m_RawTestInfo.m_fTestResult = DataObj.m_strResult.toDouble();
                }

                //结果计算完成,本次测试正常结束
                strTestMsg = QString("[通道%1] 结果计算完成,本次测试正常结束,结果值:%2").arg(nChannel).arg(DataObj.m_strResult);
                qDebug()<<strTestMsg;

                //删除结果数组
                delete pRecord;
                if(m_isQCResult){
                    //发送结果数据给质控界面
                    emit UpdateQCResultMsg(DataObj.m_strResult);
                    //设置通道状态为空闲
                    SetTestStatus(nChannel,false);
                    //清除结果结构体
                    ClearResultDataInfo(DataObj);
                }else{
                    //结果数据已出，关闭取消测试
                    AutoTestButtonState(nChannel,false);
                    //显示结果数据
                    ShowTestResult(nChannel,DataObj.m_strResult,DataObj.m_strTestUnit);
                    //保存结果到数据库
                    SaveResultToDB(DataObj);
                    //如果为自动打印,则打印数据
                    if(m_isAutoPrint){
                        AutoPrint(nChannel);
                    }
                    //如果打开LIS连接,则测试完成后自动上传
                    int nMode = m_SetParam->value(LISMODE,0).toInt();
                    if(2 == nMode){
                        QDateTime curTime = QDateTime::currentDateTime();
                        QString strCurTime = curTime.toString("yyyyMMddhhmmss");
                        m_SimpleHL7.WriteHL7Msg(m_SerialHL7,QString::number(DataObj.m_nNumberID),
                                                DataObj.m_strName,DataObj.m_strAge,DataObj.m_strSex,
                                                DataObj.m_strTestName,DataObj.m_strResult,
                                                DataObj.m_strTestUnit,strCurTime,"ORU^R01");
                    }
                    //如果打开了PC连接模式,则测试完成后发送结果到仪器串口
                    nMode = m_SetParam->value(RFWRITEMODE,0).toInt();
                    if(3 == nMode){
                        QByteArray  byteDebugDataToSerial = PackageTestDataToSerial(DataObj,nChannel);
                        m_SerialHL7->write(byteDebugDataToSerial);
                    }

                    //恢复至新建时的按钮状态
                    //m_nCurrentDaleyChannel = nChannel;
                    //m_timerDaleyOpen.singleShot( 3000,  this , SLOT(DaleyOpenControl()) );
                    ProcessResult(nChannel);
                    //设置通道状态为空闲
                    SetTestStatus(nChannel,false);
                    //清除结果结构体
                    ClearResultDataInfo(DataObj);
                    //不管是不是自动测试,都关闭模式状态
                    ChangeAutoModeStates(nChannel,false);
                }
            }
            DataObj.m_nSyncID++;
        }
    }
}

/********************************************************
 *@Name:          PackageTestDataToSerial
 *@Author:        HuaT
 *@Description:   打包测试数据为字节数据,以便发送到仪器串口
 *@Param1:        测试中的各项数据，在此为结构体打包
 *@Return:        按指定协议封装好的字节数据
 *@Version:       1.0
 *@Date:          2016-7-28
********************************************************/
QByteArray TestWindow::PackageTestDataToSerial(ResultDataInfo DataObj, quint8 nChannel)
{
    QByteArray byteDebugDataToSerial;
    QByteArray byteContent;
    byteDebugDataToSerial[0] = (0x5A);
    byteDebugDataToSerial[1] = (0x1A);
    byteDebugDataToSerial[2] = (0xA1);
    byteDebugDataToSerial[3] = (0xA5);
    //卡号
    byteDebugDataToSerial[4] = (0x00);
    byteDebugDataToSerial[5] = (0x00);
    byteDebugDataToSerial[6] = (0x00);
    byteDebugDataToSerial[7] = (0x00);
    //命令-6在文档中定义为测试数据,以图形显示此数据
    byteDebugDataToSerial[8] = (0x00);
    byteDebugDataToSerial[9] = (0x00);
    byteDebugDataToSerial[10] = (0x00);
    byteDebugDataToSerial[11] = (0x06);
    //填充内容
    byteContent.append(DataObj.m_byteCanData);
    //测试信息顺序为,面积1，面积2，比值1，面积1，面积2，比值2，结果，计算方案，扫描起始点，质控峰积分长度，放大参数，检测峰积分长度，质控峰高度,测试通道号
    //面积1,长度3
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest1Area1/65536);
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest1Area1/256);
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest1Area1%256);
    //面积2
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest1Area2/65536);
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest1Area2/256);
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest1Area2%256);
    //比值1
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest1Ratio/65536);
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest1Ratio/256);
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest1Ratio%256);
    //面积1
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest2Area1/65536);
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest2Area1/256);
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest2Area1%256);
    //面积2
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest2Area2/65536);
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest2Area2/256);
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest2Area2%256);
    //比值2
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest2Ratio/65536);
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest2Ratio/256);
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nTest2Ratio%256);
    //结果,长度4
    byteContent.append(ValueToHex(QString::number(DataObj.m_RawTestInfo.m_fTestResult),4,2));
    //计算方案,长度1
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nComputeMothed);
    //扫描起始点
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nScanStart);
    //质控峰积分长度
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nQCIntegralBreadth);
    //放大参数
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nAmpParam);
    //检测峰积分长度
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nCheckIntegralBreadt);
    //质控峰高度
    byteContent.append((quint8)DataObj.m_RawTestInfo.m_nQCMinHeightValue);
    //测试通道号
    byteContent.append(nChannel);
    //长度
    byteDebugDataToSerial[12] = (quint8)(byteContent.size()/65536*256);
    byteDebugDataToSerial[13] = (quint8)(byteContent.size()/65536);
    byteDebugDataToSerial[14] = (quint8)(byteContent.size()/256);
    byteDebugDataToSerial[15] = (quint8)(byteContent.size()%256);

    byteDebugDataToSerial.append(byteContent);
    //校验数据
    qint32 nCheckSum,nTempTotal,nTemp;
    nTempTotal = 0;
    nTemp = 0;
    qint32 nCurrentLen = byteDebugDataToSerial.size();
    for(int n=12; n<nCurrentLen; n++){
        nTemp = (quint8)byteDebugDataToSerial.at(n);
        nTempTotal += nTemp;
    }
    nCheckSum = ~nTempTotal + 1;
    quint8 nCheck1 = (nCheckSum>>24);
    quint8 nCheck2 = (nCheckSum>>16);
    quint8 nCheck3 = (nCheckSum>>8);
    quint8 nCheck4 = (nCheckSum%(256));
    byteDebugDataToSerial.append(nCheck1);
    byteDebugDataToSerial.append(nCheck2);
    byteDebugDataToSerial.append(nCheck3);
    byteDebugDataToSerial.append(nCheck4);

    byteDebugDataToSerial.append(0xA5);
    byteDebugDataToSerial.append(0xA1);
    byteDebugDataToSerial.append(0x1A);
    byteDebugDataToSerial.append(0x5A);

    return byteDebugDataToSerial;
}

/********************************************************
 *@Name:        ValueToHex
 *@Author:      HuaT
 *@Description: 将指定值转换为十六进制
 *@Param1:      需要转换的字符串数值
 *@Param2:      十六进制位数
 *@Param3:      小数点位数
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-3-27
********************************************************/
QByteArray TestWindow::ValueToHex(QString strValue, quint8 nHexTotal, quint8 nDecimal)
{
    QByteArray byteResult;
    qint32 nInteger,nDecimalPoint;
    qint16  nFindDecimalPoint;
    quint8  nBit1,nBit2,nBit3,nBit4,nBit5;
    qint16  nSymbol;
    double  fTemp1,fTemp2;
    char cZero = 0x00;
    switch(nHexTotal){
    case 1:{
        nInteger = strValue.toInt();
        byteResult.append((quint8)nInteger);
        break;
    }
    case 2:{
        nFindDecimalPoint = strValue.indexOf(".");
        switch(nDecimal){
        //没有小数点位
        case 0:
            nBit1 = strValue.toInt()/256;
            nBit2 = strValue.toInt()%256;
            byteResult.append(nBit1);
            byteResult.append(nBit2);
            break;
        //一个字节的小数点位
        case 1:
            if(-1 == nFindDecimalPoint){
                nBit1 = strValue.toInt()/256;
                nBit2 = strValue.toInt()%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
            }else{
                nInteger = strValue.left(nFindDecimalPoint).toInt();
                fTemp1 = (strValue.toDouble()*100);
                nDecimalPoint = ((quint32)fTemp1)%100;
                byteResult.append(nInteger);
                byteResult.append(nDecimalPoint);
            }
            break;
        }

        break;
    }
    case 3:{//总个三个字节
        nFindDecimalPoint = strValue.indexOf(".");
        switch(nDecimal){
        case 0://没有小数字节
            nBit1 = strValue.toInt()/65536;
            nBit2 = strValue.toInt()/256;
            nBit3 = strValue.toInt()%256;
            byteResult.append(nBit1);
            byteResult.append(nBit2);
            byteResult.append(nBit3);
            break;
        case 1://一个小数字节
            if(-1 == nFindDecimalPoint){
                nBit1 = strValue.toInt()/256;
                nBit2 = strValue.toInt()%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(cZero);
            }else{
                nInteger = strValue.left(nFindDecimalPoint).toInt();
                fTemp1 = (strValue.toDouble()*100);
                nDecimalPoint = ((quint32)fTemp1)%100;
                nBit1 = nInteger/256;
                nBit2 = nInteger%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nDecimalPoint);
            }
            break;
        case 2://二个小数字节
            if(-1 == nFindDecimalPoint){
                nBit1 = strValue.toInt();
                byteResult.append(nBit1);
                byteResult.append(cZero);
                byteResult.append(cZero);
            }else{
                nInteger = strValue.left(nFindDecimalPoint).toInt();
                //nDecimalPoint = strValue.mid(nFindDecimalPoint+1).toInt();
                nBit1 = nInteger;
                fTemp1 = (strValue.toDouble()*100);
                fTemp2 = (strValue.toDouble()*10000);
                nBit2 = ((quint32)fTemp1)%100;
                nBit3 = ((quint32)fTemp2)%100;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
            }
            break;
        }
        break;
    }
    case 4:{//总共4个字节
        nFindDecimalPoint = strValue.indexOf(".");
        switch(nDecimal){
        case 0://没有小数点位
            nBit2 = strValue.toInt()/65536*256;
            nBit2 = strValue.toInt()/65536;
            nBit3 = strValue.toInt()/256;
            nBit4 = strValue.toInt()%256;
            byteResult.append(nBit1);
            byteResult.append(nBit2);
            byteResult.append(nBit3);
            byteResult.append(nBit4);
            break;
        case 1://一个小数点位
            if(-1 == nFindDecimalPoint){
                nBit1 = strValue.toInt()/65536;
                nBit2 = strValue.toInt()/256;
                nBit3 = strValue.toInt()%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
                byteResult.append(cZero);
            }else{
                nInteger = strValue.left(nFindDecimalPoint).toInt();
                fTemp1 = (strValue.toDouble()*100);
                nDecimalPoint = ((quint32)fTemp1)%100;
                nBit1 = nInteger/65536;
                nBit2 = nInteger/256;
                nBit3 = nInteger%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
                byteResult.append(nDecimalPoint);
            }
            break;
        case 2://2个小数点位
            if(-1 == nFindDecimalPoint){
                nBit1 = strValue.toInt()/256;
                nBit2 = strValue.toInt()%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(cZero);
                byteResult.append(cZero);
            }else{
                nInteger = strValue.left(nFindDecimalPoint).toInt();
                //nDecimalPoint = strValue.mid(nFindDecimalPoint+1).toInt();
                nBit1 = nInteger/256;
                nBit2 = nInteger%256;
                fTemp1 = (strValue.toDouble()*100);
                fTemp2 = (strValue.toDouble()*10000);
                nBit3 = ((quint32)fTemp1)%100;
                nBit4 = ((quint32)fTemp2)%100;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
                byteResult.append(nBit4);
            }
            break;
        }
        break;
    }
    case 5:{//总共5个字节
        nFindDecimalPoint = strValue.indexOf(".");
        nSymbol = strValue.indexOf("-");
        if(-1 == nSymbol){
            byteResult.append("+");
        }else{
            byteResult.append("-");
        }
        switch(nDecimal){
        case 0://没有小数点位
            nBit1 = qAbs( strValue.toInt() )/65536*256*256;
            nBit2 = qAbs( strValue.toInt() )/65536*256;
            nBit3 = qAbs( strValue.toInt() )/256*256;
            nBit4 = qAbs( strValue.toInt() )/256;
            nBit5 = qAbs( strValue.toInt() )%256;
            byteResult.append(nBit1);
            byteResult.append(nBit2);
            byteResult.append(nBit3);
            byteResult.append(nBit4);
            byteResult.append(nBit5);
            break;
        case 1://1个小数点位
            if(-1 == nFindDecimalPoint){
                nBit1 = qAbs( strValue.toInt() )/65536*256;
                nBit2 = qAbs( strValue.toInt() )/256*256;
                nBit3 = qAbs( strValue.toInt() )/256;
                nBit4 = qAbs( strValue.toInt() )%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
                byteResult.append(nBit4);
                byteResult.append(cZero);
            }else{
                nInteger = qAbs( strValue.left(nFindDecimalPoint).toInt() );
                fTemp1 = (qAbs(strValue.toDouble())*100);
                nDecimalPoint = ((quint32)fTemp1)%100;
                nBit1 = nInteger/65536*256;
                nBit2 = nInteger/65536;
                nBit3 = nInteger/256;
                nBit4 = nInteger%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
                byteResult.append(nBit4);
                byteResult.append(nDecimalPoint);
            }
            break;
        case 2://2个小数点位
            if(-1 == nFindDecimalPoint){
                nBit1 = qAbs( strValue.toInt() )/65536;
                nBit2 = qAbs( strValue.toInt() )/256;
                nBit3 = qAbs( strValue.toInt() )%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
                byteResult.append(cZero);
                byteResult.append(cZero);
            }else{
                nInteger = qAbs(strValue.left(nFindDecimalPoint).toInt());
                //nDecimalPoint = qAbs(strValue.mid(nFindDecimalPoint+1).toInt());
                nBit1 = nInteger/65536;
                nBit2 = nInteger/256;
                nBit3 = nInteger%256;
                fTemp1 = (qAbs(strValue.toDouble())*100);
                fTemp2 = (qAbs(strValue.toDouble())*10000);
                nBit4 = ((quint32)fTemp1)%100;
                nBit5 = ((quint32)fTemp2)%100;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
                byteResult.append(nBit4);
                byteResult.append(nBit5);
            }
            break;
        }
        break;
    }

    }
    return byteResult;
}


/********************************************************
 *@Name:          ProcessResult
 *@Author:        HuaT
 *@Description:   结果出来后,处理结果
 *@Param1:        结果的通道号
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-7-28
********************************************************/
void TestWindow::ProcessResult(quint8 nChannel)
{
    switch (nChannel) {
    case 1:
        //恢复至新建时的按钮状态
        m_timerDaleyOpen.singleShot( 1800,  this , SLOT(DaleyOpenControl1()) );
        break;
    case 2:
        m_timerDaleyOpen.singleShot( 1800,  this , SLOT(DaleyOpenControl2()) );
        break;
    case 3:
        m_timerDaleyOpen.singleShot( 1800,  this , SLOT(DaleyOpenControl3()) );
        break;
    default:
        break;
    }
}


/********************************************************
 *@Name:          ClearResultDataInfo
 *@Author:        HuaT
 *@Description:   清除结果结构体内容，以便下次使用
 *@Param1:        清除的结果结构体对象
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-7-28
********************************************************/
void TestWindow::ClearResultDataInfo(ResultDataInfo &DataObj)
{
    DataObj.m_RawTestInfo.Clear();
    DataObj.m_byteCanData.clear();
    DataObj.m_nDataLen = 0;
    DataObj.m_nCardFlag = 0;
    DataObj.m_nSyncID = 0;
    DataObj.m_strIDCardBarCode = "";
    DataObj.m_strIDMessage = "";
    DataObj.m_strResult = "";
    DataObj.m_strTestName = "";
    DataObj.m_strTestUnit = "";
    DataObj.m_nNumberID = 0;
    DataObj.m_nReactionTime = 0;
    DataObj.m_strValidDate = "";
    DataObj.m_strName = "";
    DataObj.m_strAge = "";
    DataObj.m_strSex = "";
}

/********************************************************
 *@Name:          InsertLogMsg
 *@Author:        HuaT
 *@Description:   插入日志信息到数据库
 *@Param1:        日志类型
 *@Param2:        日志内容
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-7-28
********************************************************/
bool TestWindow::InsertLogMsg(int nType, quint64 nNumber, QString strContent)
{
    QString strCMD;
    strCMD = QString("insert into log values(null,%1,%2,'%3',datetime('now','localtime'));")
            .arg(nType).arg(nNumber).arg(strContent);
    return m_Query->exec(strCMD);
}


/********************************************************
 *@Name:          CalcReactionTime
 *@Author:        HuaT
 *@Description:   计算测试时的剩余时间
 *@Param1:        无
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-7-29
********************************************************/
void TestWindow::CalcReactionTime1()
{
    if(m_TestStatus1){
        //ui->lcdNumberRecTime1->display(m_Candata1.m_nReactionTime);
        ui->wgRoundBar1->setValue(m_Candata1.m_nReactionTime);
        if(m_Candata1.m_nReactionTime <= -1){
            //发送开始测试
            QByteArray byteData;
            PackageCanMsg(QT_TESTOPT_STARTTEST,QT_NULL_SYNC,QT_HOST_SLAVE1,byteData);
            //停止倒计时
            m_timerClock1.stop();
            //时间显示为初始值0
            //ui->lcdNumberRecTime1->display(0);
            ui->wgRoundBar1->setValue(0);
        }
        m_Candata1.m_nReactionTime--;
    }
}

void TestWindow::CalcReactionTime2(){
    if(m_TestStatus2){
        //ui->lcdNumberRecTime2->display(m_Candata2.m_nReactionTime);
        ui->wgRoundBar2->setValue(m_Candata2.m_nReactionTime);
        if(m_Candata2.m_nReactionTime <= -1){
            //发送开始测试
            QByteArray byteData;
            PackageCanMsg(QT_TESTOPT_STARTTEST,QT_NULL_SYNC,QT_HOST_SLAVE2,byteData);
            //停止倒计时
            m_timerClock2.stop();
            //时间显示为初始值0
            //ui->lcdNumberRecTime2->display(0);
            ui->wgRoundBar2->setValue(0);
        }
        m_Candata2.m_nReactionTime--;
    }
}

void TestWindow::CalcReactionTime3(){
    if(m_TestStatus3){
        //ui->lcdNumberRecTime3->display(m_Candata3.m_nReactionTime);
        ui->wgRoundBar3->setValue(m_Candata3.m_nReactionTime);
        if(m_Candata3.m_nReactionTime <= -1){
            //发送开始测试
            QByteArray byteData;
            PackageCanMsg(QT_TESTOPT_STARTTEST,QT_NULL_SYNC,QT_HOST_SLAVE3,byteData);
            //停止倒计时
            m_timerClock3.stop();
            //时间显示为初始值0
            //ui->lcdNumberRecTime3->display(0);
            ui->wgRoundBar3->setValue(0);
        }
        m_Candata3.m_nReactionTime--;
    }
}

/********************************************************
 *@Name:          CalcResult
 *@Author:        HuaT
 *@Description:   计算结果
 *@Param1:        结果信息结构体对象
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-7-14
********************************************************/
unsigned char* TestWindow::CalcResult(ResultDataInfo &DataObj)
{
    QVector<int> vectorRawPoint;
    int nIndex;
    for(int n=0; n<DataObj.m_nDataLen; n+=2){
        nIndex = (quint8)DataObj.m_byteCanData.at(n) + (quint8)DataObj.m_byteCanData.at(n+1)*256;
        vectorRawPoint.append(nIndex);
        //qDebug() << vectorRawPoint;
    }
    if(vectorRawPoint.size() != 640){
        qDebug()<<"Calc Result: Failed";
        //return ;
    }
    unsigned int ScanData1[320];
    unsigned int ScanData2[320];
    for(int i=0,n=320; i<320; i++,n++){
        ScanData1[i] = vectorRawPoint.at(i);
        ScanData2[i] = vectorRawPoint.at(n);
    }
    //数据滤波
    m_ResultCalc.low_passfilter(ScanData1);
    m_ResultCalc.low_passfilter(ScanData2);
    //用ID卡数据计算结果
    unsigned char* pRecord = new unsigned char[50];
    unsigned char *cIDMessage = m_ResultCalc.ConvetIDCardToChar(DataObj.m_strIDMessage);
    memset(pRecord,0,50);
    //判断计算方案
    int nCalcNum = cIDMessage[148];
    //qDebug()<<nCalcNum;
    DataObj.m_RawTestInfo.m_nComputeMothed = nCalcNum;
    switch(nCalcNum){
    case 0:
        m_ResultCalc.calculateResult(DataObj,ScanData1,ScanData2,pRecord,cIDMessage);
        break;
    case 1:
        m_ResultCalc.calculateResult(DataObj,ScanData2,ScanData1,pRecord,cIDMessage);
        break;
    default:
        m_ResultCalc.calculateResult(DataObj,ScanData1,ScanData2,pRecord,cIDMessage);
        break;
    }
    //m_ResultCalc.calculateResult(ScanData1,ScanData2,pRecord,cIDMessage);
    delete [] cIDMessage;
    return pRecord;

}

/********************************************************
 *@Name:          CalcResult
 *@Author:        HuaT
 *@Description:   计算结果
 *@Param1:        同步字
 *@Param2:        测试数据
 *@Param3:        通道号
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-7-14
********************************************************/
QString TestWindow::CalcResult(ResultDataInfo &DataObj, bool bNewCard)
{
    bNewCard = false;
    QVector<int> vectorRawPoint;
    int nIndex;
    for(int n=0; n<DataObj.m_nDataLen; n+=2){
        nIndex = (quint8)DataObj.m_byteCanData.at(n) + (quint8)DataObj.m_byteCanData.at(n+1)*256;
        vectorRawPoint.append(nIndex);
        //qDebug() << vectorRawPoint;
    }
    if(vectorRawPoint.size() != 640){
        qDebug()<<"Calc Result: Failed";
        //return ;
    }
    unsigned int ScanData1[320];
    unsigned int ScanData2[320];
    for(int i=0,n=320; i<320; i++,n++){
        ScanData1[i] = vectorRawPoint.at(i);
        ScanData2[i] = vectorRawPoint.at(n);
    }
    //数据滤波
    m_ResultCalc.low_passfilter(ScanData1);
    m_ResultCalc.low_passfilter(ScanData2);
    //用ID卡数据计算结果
    unsigned char* pRecord = new unsigned char[50];
    unsigned char *cIDMessage = m_ResultCalc.ConvetIDCardToChar(DataObj.m_strIDMessage);
    memset(pRecord,0,50);
    //判断计算方案
    int nCalcNum = cIDMessage[23];
    DataObj.m_RawTestInfo.m_nComputeMothed = nCalcNum;
    //qDebug()<<nCalcNum;
    QString strResult;
    switch(nCalcNum){
    case 0:
        strResult = m_ResultCalc.calculateResult2(DataObj,ScanData1,ScanData2,pRecord,cIDMessage);
        break;
    case 1:
        strResult = m_ResultCalc.calculateResult2(DataObj,ScanData2,ScanData1,pRecord,cIDMessage);
        break;
    default:
        strResult = m_ResultCalc.calculateResult2(DataObj,ScanData1,ScanData2,pRecord,cIDMessage);
        break;
    }
    //m_ResultCalc.calculateResult(ScanData1,ScanData2,pRecord,cIDMessage);
    delete [] cIDMessage;
    return strResult;
}


/********************************************************
 *@Name:          ParseBarCode
 *@Author:        HuaT
 *@Description:   解析条码号，数据的第一和第二个字节，结合后转为二进制，然后从
 *                数据库中查出对应批号的ID卡信息，低位在前，高位在后
 *@Param1:        条码数据
 *@Param2:        新旧卡标志
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-7-25
********************************************************/
QString TestWindow::ParseBarCode(QByteArray can_data, bool bIsNewCard)
{
    quint16 n = (quint8)can_data.at(0) + (quint8)can_data.at(1)*256;
    QString str = QString::number(n,2);
    QString strResult;
    for(int n=0; n< str.size(); n++){
        if(str.at(n) == '1'){
            strResult += "01";
        }else{
            strResult += "00";
        }
    }
    //qDebug()<<strResult<<strResult.size()<<str<<str.size();
    if(bIsNewCard){
        return str;
    }else{
        return strResult;
    }
}


/********************************************************
 *@Name:          GetIDMessageInfo
 *@Author:        HuaT
 *@Description:   获得指定批号的ID卡信息
 *@Param1:        试剂批号-101000之类的条码号
 *@Return:        试剂卡信息内容,如果查询失败或者无对应数据，则返回空
 *@Version:       1.0
 *@Date:          2016-7-26
********************************************************/
QString TestWindow::GetIDMessageInfo(QString strBarCode)
{
    QString strIDMessage;
    QString strCMD;
    strCMD = QString("select data from idcard where barcode='%1';").arg(strBarCode);
    if(m_Query->exec(strCMD)){
        if(m_Query->next()){
            strIDMessage = m_Query->value(0).toString();
        }
    }
    return strIDMessage;
}

/********************************************************
 *@Name:          ParseIDMessageInfo
 *@Author:        HuaT
 *@Description:   解析ID卡数据，获得扫描起点和激光放大倍数
 *@Param1:        ID卡数据内容
 *@Param2:        扫描起始点
 *@Param3:        放大倍数
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-7-26
********************************************************/
void TestWindow::ParseIDMessageInfo(unsigned char *pIDMessage, quint8& StartPoint, quint8& Amp,
                                    ResultDataInfo& DataObj)
{
    switch(DataObj.m_nCardFlag){
    case 0://旧ID卡
    {
        StartPoint = (quint8)pIDMessage[108];
        DataObj.m_RawTestInfo.m_nScanStart = StartPoint;
        Amp = (quint8)pIDMessage[146];
        DataObj.m_RawTestInfo.m_nAmpParam = Amp;
        QString strTempName,strTempUnit;
        for(int n=151,m=166; n<166; n++,m++){
            //测试名称
            strTempName += pIDMessage[n];
            //测试单位
            strTempUnit += pIDMessage[m];
        }
        if(strTempName.contains("-HCG"))
        {
            strTempName = "β-HCG";
        }
        DataObj.m_strTestName = strTempName.trimmed();
        DataObj.m_strTestUnit = strTempUnit.trimmed();
        //反应时间
        QString strReactionTime=QString("%1%2").arg(pIDMessage[107]).arg(pIDMessage[106]);
        //试剂有效期
        QString year,month,day;
        year = QString("20%1%2").arg(pIDMessage[89]).arg(pIDMessage[90]);
        month = QString("%1%2").arg(pIDMessage[91]).arg(pIDMessage[92]);
        day = QString("%1%2").arg(pIDMessage[93]).arg(pIDMessage[94]);
        DataObj.m_strValidDate = QString("%1-%2-%3").arg(year).arg(month).arg(day);
        //qDebug()<<pIDMessage[106] << pIDMessage[107]<<ss.toInt()*60;
        DataObj.m_nReactionTime = strReactionTime.toInt() * 60;
        break;
    }
    case 1:
    {
        StartPoint = (quint8)pIDMessage[36];
        DataObj.m_RawTestInfo.m_nScanStart = StartPoint;
        Amp = (quint8)pIDMessage[22];
        DataObj.m_RawTestInfo.m_nAmpParam = Amp;
        QString strTempName,strTempUnit;
        for(int n=46; n<66; n++){
            //测试名称
            strTempName += pIDMessage[n];
        }
        for(int m=66; m<76; m++){
            //测试单位
            strTempUnit += pIDMessage[m];
        }
        /*if(strTempName.contains("-HCG"))
        {
            strTempName = "β-HCG";
        }*/
        DataObj.m_strTestName = strTempName.trimmed();
        DataObj.m_strTestUnit = strTempUnit.trimmed();
        //反应时间
        DataObj.m_nReactionTime = (quint8)pIDMessage[20]*256 + (quint8)pIDMessage[21];
        //试剂有效期
        QString year,month,day;
        year = QString("20%1").arg(pIDMessage[32]);
        month = QString("%1").arg(pIDMessage[33]);
        day = QString("%1").arg(pIDMessage[34]);
        DataObj.m_strValidDate = QString("%1-%2-%3").arg(year).arg(month).arg(day);
        //qDebug()<<pIDMessage[106] << pIDMessage[107]<<ss.toInt()*60;
        break;
    }
    default:
        QMessageBox::information(this,"提示","卡标志数据不符合,测试可能存在问题",QMessageBox::Ok);
        break;
    }


}


/********************************************************
 *@Name:          DaleyOpenControl1
 *@Author:        HuaT
 *@Description:   数据接收完成后，由于电机退出需要时间，所以延时打开下次新建控件按钮
 *@Param1:        延时打开的对象通道
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-8-1
********************************************************/
void TestWindow::DaleyOpenControl1()
{
    /*  int nChannel;
    nChannel = m_nCurrentDaleyChannel;
    QTime t;
    t.start();
    while(t.elapsed() < 3000)
    {
        QCoreApplication::processEvents();
    }*/
    //恢复至新建时的按钮状态
    InitChannelState(1,false);
    //得到结果，更新打印按钮
    UPdatePrintState(1);
    //更新测试按钮文本
    UpdateTestText(1);

    //如果打开老化功能，则自动新建进行下次操作
    int nAgingMode = m_SetParam->value(AGINGMODE,0).toInt();
    if(1 == nAgingMode){
        //InstantTest(1);
        //获取老化间隔时间,秒
        int nAgingTime = m_SetParam->value(AGINGTIME,0).toInt();
        m_timerAging1.singleShot(nAgingTime * 1000,this, SLOT(AgingTest1()) );
    }
}


/********************************************************
 *@Name:          DaleyOpenControl2
 *@Author:        HuaT
 *@Description:   数据接收完成后，由于电机退出需要时间，所以延时打开下次新建控件按钮
 *@Param1:        延时打开的对象通道
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-8-3
********************************************************/
void TestWindow::DaleyOpenControl2()
{
    //恢复至新建时的按钮状态
    InitChannelState(2,false);
    //得到结果，更新打印按钮
    UPdatePrintState(2);
    //更新测试按钮文本
    UpdateTestText(2);

    //如果打开老化功能，则自动新建进行下次操作
    int nAgingMode = m_SetParam->value(AGINGMODE,0).toInt();
    if(1 == nAgingMode){
        //InstantTest(2);
        //获取老化间隔时间,秒
        int nAgingTime = m_SetParam->value(AGINGTIME,0).toInt();
        m_timerAging2.singleShot(nAgingTime * 1000,this, SLOT(AgingTest2()) );
    }
}


/********************************************************
 *@Name:          DaleyOpenControl3
 *@Author:        HuaT
 *@Description:   数据接收完成后，由于电机退出需要时间，所以延时打开下次新建控件按钮
 *@Param1:        延时打开的对象通道
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-8-3
********************************************************/
void TestWindow::DaleyOpenControl3()
{
    //恢复至新建时的按钮状态
    InitChannelState(3,false);
    //得到结果，更新打印按钮
    UPdatePrintState(3);
    //更新测试按钮文本
    UpdateTestText(3);

    //如果打开老化功能，则自动新建进行下次操作
    int nAgingMode = m_SetParam->value(AGINGMODE,0).toInt();
    if(1 == nAgingMode){
        //InstantTest(3);
        //获取老化间隔时间,秒
        int nAgingTime = m_SetParam->value(AGINGTIME,0).toInt();
        m_timerAging3.singleShot(nAgingTime * 1000,this, SLOT(AgingTest3()) );
    }
}

/********************************************************
 *@Name:          AgingTest1
 *@Author:        HuaT
 *@Description:   通道1进行老化测试的槽
 *@Param1:        无
 *@Return:        无
 *@Version:       1.0
 *@Date:          2017-1-11
********************************************************/
void TestWindow::AgingTest1()
{
    NewTest(1);
    InstantTest(1);
}

/********************************************************
 *@Name:          AgingTest2
 *@Author:        HuaT
 *@Description:   通道2进行老化测试的槽
 *@Param1:        无
 *@Return:        无
 *@Version:       1.0
 *@Date:          2017-1-11
********************************************************/
void TestWindow::AgingTest2()
{
    NewTest(2);
    InstantTest(2);
}

/********************************************************
 *@Name:          AgingTest3
 *@Author:        HuaT
 *@Description:   通道3进行老化测试的槽
 *@Param1:        无
 *@Return:        无
 *@Version:       1.0
 *@Date:          2017-1-11
********************************************************/
void TestWindow::AgingTest3()
{
    NewTest(3);
    InstantTest(3);
}

/********************************************************
 *@Name:          StopAgingTimer
 *@Author:        HuaT
 *@Description:   停止老化计时器
 *@Param1:        无
 *@Return:        无
 *@Version:       1.0
 *@Date:          2017-1-11
********************************************************/
void TestWindow::StopAgingTimer()
{
    m_timerAging1.stop();
    m_timerAging2.stop();
    m_timerAging3.stop();
}

/********************************************************
 *@Name:          GetCanBus
 *@Author:        HuaT
 *@Description:   返回Can通信对象
 *@Param1:        无
 *@Return:        实例对象
 *@Version:       1.0
 *@Date:          2016-8-3
********************************************************/
#ifdef Q_OS_LINUX
CanBus &TestWindow::GetCanBus()
{
    return m_CanBus;
}
#endif

/********************************************************
 *@Name:          GetChannel1Status
 *@Author:        HuaT
 *@Description:   返回通道1是否空闲
 *@Param1:        无
 *@Return:        true为运行中,false为空闲
 *@Version:       1.0
 *@Date:          2016-8-3
********************************************************/
bool TestWindow::GetChannel1Status()
{
    return m_TestStatus1;
}

/********************************************************
 *@Name:          GetChannel2Status
 *@Author:        HuaT
 *@Description:   返回通道2是否空闲
 *@Param1:        无
 *@Return:        true为运行中,false为空闲
 *@Version:       1.0
 *@Date:          2016-8-3
********************************************************/
bool TestWindow::GetChannel2Status()
{
    return m_TestStatus2;
}

/********************************************************
 *@Name:          GetChannel3Status
 *@Author:        HuaT
 *@Description:   返回通道3是否空闲
 *@Param1:        无
 *@Return:        true为运行中,false为空闲
 *@Version:       1.0
 *@Date:          2016-8-3
********************************************************/
bool TestWindow::GetChannel3Status()
{
    return m_TestStatus3;
}

/********************************************************
 *@Name:          SetChannel1Status
 *@Author:        HuaT
 *@Description:   设置通道1状态
 *@Param1:        无
 *@Return:        true为运行中,false为空闲
 *@Version:       1.0
 *@Date:          2016-8-3
********************************************************/
void TestWindow::SetChannel1Status(bool bStatus)
{
    m_TestStatus1 = bStatus;
}

/********************************************************
 *@Name:          SetChannel2Status
 *@Author:        HuaT
 *@Description:   设置通道2状态
 *@Param1:        无
 *@Return:        true为运行中,false为空闲
 *@Version:       1.0
 *@Date:          2016-8-3
********************************************************/
void TestWindow::SetChannel2Status(bool bStatus)
{
    m_TestStatus2 = bStatus;
}

/********************************************************
 *@Name:          SetChannel3Status
 *@Author:        HuaT
 *@Description:   设置通道3状态
 *@Param1:        无
 *@Return:        true为运行中,false为空闲
 *@Version:       1.0
 *@Date:          2016-8-3
********************************************************/
void TestWindow::SetChannel3Status(bool bStatus)
{
    m_TestStatus3 = bStatus;
}

/********************************************************
 *@Name:          SetHardUpdateData
 *@Author:        HuaT
 *@Description:   设置固件升级数据
 *@Param1:        发送给仪器的数据内容，为十六进制
 *@Return:        无
 *@Version:       1.0
 *@Date:          2016-11-16
********************************************************/
void TestWindow::SetHardUpdateData(QByteArray byteUpdateData, HardGrading* pHard)
{
    //固件升级对象
    m_pHardGrading = pHard;
    //获取文本数据，进行处理
    m_byteHardUpdateData1.clear();
    m_byteHardUpdateData2.clear();
    m_byteHardUpdateData3.clear();
    m_byteHardUpdateData1 = byteUpdateData;
    m_byteHardUpdateData2 = byteUpdateData;
    m_byteHardUpdateData3 = byteUpdateData;
    //发送升级命令，等待回应
    //qDebug()<<m_byteHardUpdateData;
    QByteArray byteCanData;
    PackageCanMsg(QT_SYSOPT_REQUESTUPDATA,QT_NULL_SYNC,1,byteCanData);
    PackageCanMsg(QT_SYSOPT_REQUESTUPDATA,QT_NULL_SYNC,2,byteCanData);
    PackageCanMsg(QT_SYSOPT_REQUESTUPDATA,QT_NULL_SYNC,3,byteCanData);
}


/********************************************************
 *@Name:          eventFilter
 *@Author:        HuaT
 *@Description:   回车事件过滤函数
 *@Param1:        事件对象
 *@Param2:        事件内容
 *@Return:        是否处理过此事件
 *@Version:       1.0
 *@Date:          2016-9-12
********************************************************/
bool TestWindow::eventFilter(QObject *obj, QEvent *event)
{
    QKeyEvent* keyevent = (QKeyEvent*)event;
    if(obj == ui->leNumber1){
        if(keyevent->key() == Qt::Key_Return){
            ui->btnAutoTest1->setFocus();
            return true;
        }
    }
    if(obj == ui->leNumber2){
        if(keyevent->key() == Qt::Key_Return){
            ui->btnAutoTest2->setFocus();
            return true;
        }
    }
    if(obj == ui->leNumber3){
        if(keyevent->key() == Qt::Key_Return){
            ui->btnAutoTest3->setFocus();
            return true;
        }
    }
    return QWidget::eventFilter(obj,event);
}


/********************************************************
 *@Name:          MockTest
 *@Author:        HuaT
 *@Description:   模拟测试,通过串口发送数据，模拟测试
 *@Param1:        测试原始数据
 *@Param2:        事件内容
 *@Return:        是否处理过此事件
 *@Version:       1.0
 *@Date:          2016-9-12
********************************************************/
void TestWindow::MockTest(QByteArray byteData)
{
    bool b = false;
    QByteArray byteBarCode = byteData.mid(0,14);
    QByteArray byteRawData = byteData.mid(14);
    MockTestProcessBarCode(1,b,m_Candata1,byteBarCode);
    m_Candata1.m_nDataLen = 1280;
    m_Candata1.m_byteCanData = byteRawData;
    MockTestParseTestData(11,byteRawData,1,m_Candata1);
}

void TestWindow::MockTestParseTestData(quint8 nSync, QByteArray can_data, quint8 nChannel, ResultDataInfo &DataObj)
{
    if(DataObj.m_nDataLen == (quint32)DataObj.m_byteCanData.size()){
        //计算结果
        //unsigned int ScanData1[320];
        //unsigned int ScanData2[320];
        unsigned char* pRecord = NULL;
        //memset(pRecord,0,40);
        //新旧卡标志区分
        if(0 == DataObj.m_nCardFlag){ //旧卡数据
            pRecord = CalcResult(DataObj);
            //for(int i=0; i<32; i++){
            //    qDebug()<<i<<pRecord[i];
            //}
            //小数位数
            quint8 nDecimal = QString::number((quint8)pRecord[10]).toInt();
            QString strTempResult;
            if(48 == nDecimal){
                strTempResult = QString("%1").arg(pRecord[12]*256 + pRecord[13]);
            }else{
                strTempResult = QString("%1.%2").arg(pRecord[12]*256 + pRecord[13]).arg(pRecord[11]);
            }
            //最大值最小值标记
            quint8 nFlag = QString::number((quint8)pRecord[9]).toInt();
            //qDebug()<<"nFlag:"<<nFlag;
            if(0 == nFlag){
                DataObj.m_strResult = strTempResult;
            }else if(1 == nFlag){
                DataObj.m_strResult = QString("<%1").arg(strTempResult);
            }else if(2 == nFlag){
                DataObj.m_strResult = QString(">%1").arg(strTempResult);
            }
            DataObj.m_RawTestInfo.m_fTestResult = strTempResult.toDouble();
        }else { //新卡数据
            DataObj.m_strResult = CalcResult(DataObj,true);
            //
            DataObj.m_RawTestInfo.m_fTestResult = DataObj.m_strResult.toDouble();
        }


        //删除结果数组
        delete pRecord;
        if(m_isQCResult){
            //发送结果数据给质控界面
            emit UpdateQCResultMsg(DataObj.m_strResult);
            //设置通道状态为空闲
            SetTestStatus(nChannel,false);
            //清除结果结构体
            ClearResultDataInfo(DataObj);
        }else{
            //结果数据已出，关闭取消测试
            AutoTestButtonState(nChannel,false);
            //显示结果数据
            ShowTestResult(nChannel,DataObj.m_strResult,DataObj.m_strTestUnit);
            //保存结果到数据库
            SaveResultToDB(DataObj);
            //如果为自动打印,则打印数据
            if(m_isAutoPrint){
                AutoPrint(nChannel);
            }
            //如果打开LIS连接,则测试完成后自动上传
            int nMode = m_SetParam->value(LISMODE,0).toInt();
            if(2 == nMode){
                QDateTime curTime = QDateTime::currentDateTime();
                QString strCurTime = curTime.toString("yyyyMMddhhmmss");
                m_SimpleHL7.WriteHL7Msg(m_SerialHL7,QString::number(DataObj.m_nNumberID),
                                        DataObj.m_strName,DataObj.m_strAge,DataObj.m_strSex,
                                        DataObj.m_strTestName,DataObj.m_strResult,
                                        DataObj.m_strTestUnit,strCurTime,"ORU^R01");
            }
            //如果打开了PC连接模式,则测试完成后发送结果到仪器串口
            nMode = m_SetParam->value(RFWRITEMODE,0).toInt();
            if(3 == nMode){
                QByteArray  byteDebugDataToSerial = PackageTestDataToSerial(DataObj,nChannel);
                m_SerialHL7->write(byteDebugDataToSerial);
            }

            //恢复至新建时的按钮状态
            //m_nCurrentDaleyChannel = nChannel;
            //m_timerDaleyOpen.singleShot( 3000,  this , SLOT(DaleyOpenControl()) );
            ProcessResult(nChannel);
            //设置通道状态为空闲
            SetTestStatus(nChannel,false);
            //清除结果结构体
            ClearResultDataInfo(DataObj);
            //不管是不是自动测试,都关闭模式状态
            ChangeAutoModeStates(nChannel,false);
        }
    }
}


bool TestWindow::MockTestProcessBarCode(quint8 nChannel, bool& bTestStatus, ResultDataInfo &data, QByteArray can_data)
{
    //获得条码编号,先用新条码规则,新的条码格式为10100000000101
    data.m_strIDCardBarCode = ParseBarCode(can_data,true);
    //查询此批号的ID卡数据
    data.m_strIDMessage = GetIDMessageInfo(data.m_strIDCardBarCode);
    if(data.m_strIDMessage.isEmpty()){//如果是空,则调用旧条码规则,旧的条码格式为010001000000000010001
        data.m_strIDCardBarCode = ParseBarCode(can_data,false);
        //data.m_strIDCardBarCode = "10101000000101";//测试原始数据专用
        data.m_strIDMessage = GetIDMessageInfo(data.m_strIDCardBarCode);
    }
    QString strTitle,strContent,strLog;
    strTitle = QObject::tr("Note");
    if(data.m_strIDMessage.isEmpty()){
        //没找到批号,回复仪器
        QByteArray byteRespond;
        byteRespond[0] = QT_BARCODE_INVALID;
        PackageCanMsg(QT_SYSOPT_ACK,QT_TESTOPT_BARCODE,nChannel,byteRespond);
        //提示用户导入ID卡数据
        strContent = QObject::tr("There is no corresponding ID information, please import the relevant ID card");
        QMessageBox::information(this,strTitle,strContent,QMessageBox::Ok);
        //数据记录
        strLog = QString("无对应ID信息:%1,请导入对应ID卡信息").arg(data.m_strIDCardBarCode);
        InsertLogMsg(QT_LOGOPT_ERROR, data.m_nNumberID, strLog);
        //测试状态取消
        bTestStatus = false;
        //自动测试模式关闭
        ChangeAutoModeStates(nChannel,false);
        //恢复按钮状态
        //恢复至新建时的按钮状态
        InitChannelState(nChannel,false);
        //得到结果，更新打印按钮
        UPdatePrintState(nChannel);
        //更新测试按钮文本
        UpdateTestText(nChannel);
        //由于不能获取质控状态，不管是不是质控测试，出现异常恢复质控按钮状态
        emit UpdateQCControlStatus();
        return false;
    }
    //转换批号为无符号型指针
    unsigned char *cIDMessage = m_ResultCalc.ConvetIDCardToChar(data.m_strIDMessage);
    //新旧卡标志
    if(data.m_strIDMessage.size() > 400){
        //data.m_nCardFlag = (quint8)pIDMessage[39];
        data.m_nCardFlag = 1;
    }else{
        data.m_nCardFlag = 0;
    }
    //解析ID卡数据
    quint8 nStartPoint,nAmp;
    ParseIDMessageInfo(cIDMessage,nStartPoint,nAmp,data);
    //检测有效日期
    bool bRet = CheckValidDate(data.m_strValidDate);
    if(!bRet){
        //试剂过期,回复仪器
        QByteArray byteRespond;
        //当条码无效时仪器不管1-2字节的内容
        byteRespond[0] = QT_BARCODE_INVALID;
        PackageCanMsg(QT_SYSOPT_ACK,QT_TESTOPT_BARCODE,nChannel,byteRespond);
        //提示用户试剂卡已过期
        strContent = QObject::tr("Reagent Strip has expired");
        QMessageBox::information(this,strTitle,strContent,QMessageBox::Ok);
        //测试状态取消
        bTestStatus = false;
        //自动测试模式关闭
        ChangeAutoModeStates(nChannel,false);
        //恢复至新建时的按钮状态
        InitChannelState(nChannel,false);
        //得到结果，更新打印按钮
        UPdatePrintState(nChannel);
        //更新测试按钮文本
        UpdateTestText(nChannel);
        //由于不能获取质控状态，不管是不是质控测试，出现异常恢复质控按钮状态
        emit UpdateQCControlStatus();
        return false;
    }
    //显示测试项目
    ShowTestItem(nChannel,data.m_strTestName);
    //找到批号,响应仪器
    QByteArray byteRespond;
    byteRespond[0] = QT_BARCODE_VALID;
    byteRespond[1] = nStartPoint;
    byteRespond[2] = nAmp;
    PackageCanMsg(QT_SYSOPT_ACK,QT_TESTOPT_BARCODE,nChannel,byteRespond);
    return true;
}
