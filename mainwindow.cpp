/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: mainwindow.cpp
 *  简要描述: 主框架源文件
 *          实现计时器、系统时间显示、电量显示、版本显示、子窗口切换功能
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include<QDebug>
#include<QDesktopServices>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //连接数据库
    m_db.ConnectDB();
    //检测配置文件
    CheckSettingFile();
    //蜂鸣器状态
    m_bBuzzState = false;
    m_bBuzzChange = false;
    //倒计时总时间初始化
    m_Time1 = 0;
    m_Time2 = 0;
    m_Time3 = 0;
    //倒计时状态初始化，初始时为不开始倒计时
    m_isRun1 = false;
    m_isRun2 = false;
    m_isRun3 = false;
    //版本赋值
    m_Version = QObject::tr("V1.0.0");
    //RF串口初始化
    InitRfSerial();
    connect(&m_SerialRF,SIGNAL(readyRead()), this, SLOT(RecvRfSerialData()));
    //打印串口初始化
    InitPrintSerial();
    connect(&m_SerialPrint,SIGNAL(readyRead()), this, SLOT(RecvPrintSerialData()));
    //Lis串口初始化
    InitHl7Serial();
    connect(&m_SerialHL7,SIGNAL(readyRead()), this, SLOT(RecvHL7SerialData()));
    QByteArray bytePrintConfig;
    bytePrintConfig[0] = 0x1D;
    bytePrintConfig[1] = 0x61;
    bytePrintConfig[2] = 0x24;
    m_SerialPrint.write(bytePrintConfig);
    //按钮初始化状态,默认状态为0，闪烁状态为1
    m_btnState1 = 0;
    m_btnState2 = 0;
    m_btnState3 = 0;
    ui->setupUi(this);
    m_TestWin  = new TestWindow(this);
    m_QueryWin = new QueryWindow(this);
    m_QCWin    = new QCWindow(this);
    m_SetWin   = new SetWindow(this);
    ui->stackedWidget->addWidget(m_TestWin);
    ui->stackedWidget->addWidget(m_QueryWin);
    ui->stackedWidget->addWidget(m_QCWin);
    m_SetWin->SetDbObj(&m_db);
    ui->stackedWidget->addWidget(m_SetWin);
    //连接免密码信号
    connect(this,SIGNAL(NoPassword()), m_SetWin, SLOT(AutoUnlock()) );
    //设置默认widget
    ui->stackedWidget->setCurrentWidget(m_TestWin);
    //选择时间后，传递时间给按钮
    m_SetTimer = new SetTimerObj(this);
    connect(m_SetTimer,SIGNAL(CoutDown(int,int)),this,SLOT(StartTime(int,int)));
    //连接定时1与按钮1
    connect(&m_Timer1,SIGNAL(timeout()),this,SLOT(TimeChange1()));
    //连接定时器2与按钮2
    connect(&m_Timer2,SIGNAL(timeout()),this,SLOT(TimeChange2()));
    //连接定时器3与按钮3
    connect(&m_Timer3,SIGNAL(timeout()),this,SLOT(TimeChange3()));
    //连接测试与质控结果信号
    connect(m_TestWin, SIGNAL(UpdateQCResultMsg(QString)), m_QCWin, SLOT(UpdateResult(QString)) );
    connect(m_TestWin, SIGNAL(UpdateQCControlStatus()), m_QCWin, SLOT(UpdateControlStatus()) );
    //颜色转换
    connect(&m_ColorTimer1,SIGNAL(timeout()),this,SLOT(ColorChange1()));
    connect(&m_ColorTimer2,SIGNAL(timeout()),this,SLOT(ColorChange2()));
    connect(&m_ColorTimer3,SIGNAL(timeout()),this,SLOT(ColorChange3()));
    //蜂鸣器转换
    connect(&m_BuzzTimer,SIGNAL(timeout()), this, SLOT(BuzzSound()) );
    //显示系统时间
    connect(&m_SystemTimer,SIGNAL(timeout()),this,SLOT(ShowSysTime()));
    m_SystemTimer.singleShot(1,this,SLOT(ShowSysTime()));
    m_SystemTimer.start(1000);
    //更新电池状态
    connect(&m_BatteryTimer,SIGNAL(timeout()), this, SLOT(SetBatteryState()) );
    m_BatteryTimer.start(1000);
    //打开默认显示测试界面
    ui->btnTest->setChecked(true);
    //是否更新硬件版本信息
    m_bUpdateHardVersion = true;
    //读取配置文件信息
    m_SetParam = m_settings.ReadSettingsInfoToMap();
    //关闭射频烧写模式
    m_settings.SetParam(RFWRITEMODE,"0");
    m_settings.WriteSettingsInfoToMap();
    //
    connect(this,SIGNAL(MockTest(QByteArray)),m_TestWin, SLOT(MockTest(QByteArray)) );
}

MainWindow::~MainWindow()
{
    delete ui;
}

/********************************************************
 *@Name:        CheckSettingFile
 *@Author:      HuaT
 *@Description: 检测配置文件是否存在
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-6
********************************************************/
void MainWindow::CheckSettingFile()
{
    //检测配置文件
    if(!m_settings.CheckFileExist()){
        //创建默认配置文件
        m_settings.DefaultSettings();
    }
}

/********************************************************
 *@Name:        InitRfSerial
 *@Author:      HuaT
 *@Description: 初始化RF串口
 *@Param:       无
 *@Return:      打开RF串口是否成功
 *@Version:     1.0
 *@Date:        2016-9-1
********************************************************/
bool MainWindow::InitRfSerial()
{
    m_SerialRF.setBaudRate(BAUD19200);
#ifdef Q_OS_WIN32
    m_SerialRF.setPortName("\\\\.\\com6");
#elif defined(Q_OS_LINUX)
    m_SerialRF.setPortName("/dev/ttyO1");
#endif
    bool bOpen = m_SerialRF.open(QIODevice::ReadWrite);
    if(!bOpen){
        qDebug()<<"RF serial open the fail";
    }
    return bOpen;
}

/********************************************************
 *@Name:        InitPrintSerial
 *@Author:      HuaT
 *@Description: 初始化打印串口
 *@Param:       无
 *@Return:      打开打印串口是否成功
 *@Version:     1.0
 *@Date:        2016-9-1
********************************************************/
bool MainWindow::InitPrintSerial()
{
    //串口初始化
    m_SerialPrint.setBaudRate(BAUD9600);
#ifdef Q_OS_WIN32
    m_SerialPrint.setPortName("\\\\.\\com1");
#else
    m_SerialPrint.setPortName("/dev/ttyO2");
#endif
    bool bOpen = m_SerialPrint.open(QIODevice::ReadWrite);
    if(!bOpen){
        qDebug()<<"Print Serial open the fail";
    }
    return bOpen;
}

/********************************************************
 *@Name:        InitHl7Serial
 *@Author:      HuaT
 *@Description: 初始化HL7串口
 *@Param:       无
 *@Return:      打开HL7串口是否成功
 *@Version:     1.0
 *@Date:        2017-2-10
********************************************************/
bool MainWindow::InitHl7Serial()
{
    //串口初始化
    m_SerialHL7.setBaudRate(BAUD115200);
#ifdef Q_OS_WIN32
    m_SerialHL7.setPortName("\\\\.\\com3");
#else
    m_SerialHL7.setPortName("/dev/ttyO0");
#endif
    bool bOpen = m_SerialHL7.open(QIODevice::ReadWrite);
    if(!bOpen){
        qDebug()<<"HL7 Serial open the fail";
    }
    return bOpen;
}

/********************************************************
 *@Name:        SetBatteryState
 *@Author:      HuaT
 *@Description: 检测并设置电池图标状态
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-11-14
********************************************************/
void MainWindow::SetBatteryState()
{
    QProcess pc;
    QByteArray qbaBatteryInfo ;
    QStringList listBatteryInfo;
    QString strBatteryInfo;
    pc.start("cat /sys/class/power_supply/battery/uevent");
    if(pc.waitForStarted(-1)){
        while(pc.waitForReadyRead(-1)){
            qbaBatteryInfo  += pc.readAllStandardOutput();
            //qDebug()<<qbaBatteryInfo;
        }
    }
    strBatteryInfo = qbaBatteryInfo;
    listBatteryInfo = strBatteryInfo.split("\n");
    //qDebug()<<listBatteryInfo<<listBatteryInfo.size() ;
    if(listBatteryInfo.size() < 8){
        qDebug()<<"read battery file error!";
        return;
    }
    QString strBatteryStatus;
    quint32 nBatteryCapacity;
    QString strTemp;
    QStringList listTemp;
    strTemp = listBatteryInfo[1];
    listTemp = strTemp.split("=");
    strBatteryStatus = listTemp[1];
    strTemp = listBatteryInfo[5];
    listTemp = strTemp.split("=");
    nBatteryCapacity = listTemp[1].toInt();
    //qDebug()<< strBatteryStatus << nBatteryCapacity;
    if(0 == strBatteryStatus.compare("Charging") ){
        ChangeBatteryImage(":/resource/image/battery-charged.png");
    }else if(nBatteryCapacity < 20){
        ChangeBatteryImage(":/resource/image/battery-empty.png");
    }else if(nBatteryCapacity < 50 && nBatteryCapacity>20){
        ChangeBatteryImage(":/resource/image/battery-half.png");
    }else if(nBatteryCapacity > 50){
        ChangeBatteryImage(":/resource/image/battery-full.png");
    }
}

/********************************************************
 *@Name:        ChangeBatteryImage
 *@Author:      HuaT
 *@Description: 更改电池图片
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-11-14
********************************************************/
void MainWindow::ChangeBatteryImage(QString strPath)
{
    QPixmap pixmap = QPixmap(strPath);
    pixmap = pixmap.scaled(QSize(41,36));
    ui->lbBattery->setPixmap(pixmap);
}


/********************************************************
 *@Name:        GetPrintSerialPort
 *@Author:      HuaT
 *@Description: 获取打印串口对象
 *@Param:       无
 *@Return:      返回打印串口对象
 *@Version:     1.0
 *@Date:        2016-9-1
********************************************************/
QextSerialPort *MainWindow::GetPrintSerialPort()
{
    return &m_SerialPrint;
}

/********************************************************
 *@Name:        GetCanBus
 *@Author:      HuaT
 *@Description: 获取Can通信对象指针
 *@Param1:      无
 *@Return:      通信对象地址
 *@Version:     0.1
 *@Date:        2016-8-12
********************************************************/
#ifdef Q_OS_LINUX
CanBus* MainWindow::GetCanBus()
{
    return &m_TestWin->GetCanBus();
}
#endif


/********************************************************
 *@Name:        GetHL7SerialPort
 *@Author:      HuaT
 *@Description: 获取HL7串口对象
 *@Param:       无
 *@Return:      返回HL7串口对象
 *@Version:     1.0
 *@Date:        2017-2-13
********************************************************/
QextSerialPort *MainWindow::GetHL7SerialPort()
{
    return &m_SerialHL7;
}

/********************************************************
 *@Name:        GetRFSerialPort
 *@Author:      HuaT
 *@Description: 获取RF串口对象
 *@Param:       无
 *@Return:      返回RF串口对象
 *@Version:     1.0
 *@Date:        2017-3-10
********************************************************/
QextSerialPort *MainWindow::GetRFSerialPort()
{
    return &m_SerialRF;
}

/********************************************************
 *@Name:        GetSqlQuery
 *@Author:      HuaT
 *@Description: 获取数据库查询对象
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-16
********************************************************/
QSqlQuery *MainWindow::GetSqlQuery()
{
    return m_db.GetSqlQuery();
}

/********************************************************
 *@Name:        GetQtProDb
 *@Author:      HuaT
 *@Description: 获取数据库对象
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-12-30
********************************************************/
CQtProDB *MainWindow::GetQtProDb()
{
    return &m_db;
}

/********************************************************
 *@Name:        GetSqlDatabase
 *@Author:      HuaT
 *@Description: 获取数据库对象
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-11
********************************************************/
QSqlDatabase *MainWindow::GetSqlDatabase()
{
    return m_db.GetDatabase();
}

/********************************************************
 *@Name:        GetSqlDatabase
 *@Author:      HuaT
 *@Description: 获取数据库对象
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-6
********************************************************/
CUtilSettings *MainWindow::GetUtilSetting()
{
    return &m_settings;
}

/********************************************************
 *@Name:        GetQueryWindow
 *@Author:      HuaT
 *@Description: 获取查询窗口对象
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-6
********************************************************/
QueryWindow *MainWindow::GetQueryWindow()
{
    return m_QueryWin;
}

/********************************************************
 *@Name:        GetTestWindow
 *@Author:      HuaT
 *@Description: 获取测试窗口对象
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-1-11
********************************************************/
TestWindow *MainWindow::GetTestWindow()
{
    return m_TestWin;
}

/********************************************************
 *@Name:        GetChannel1Status
 *@Author:      HuaT
 *@Description: 获取通道1运行状态
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-12
********************************************************/
bool MainWindow::GetChannel1Status()
{
    return m_TestWin->GetChannel1Status();
}

/********************************************************
 *@Name:        GetChannel2Status
 *@Author:      HuaT
 *@Description: 获取通道2运行状态
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-12
********************************************************/
bool MainWindow::GetChannel2Status()
{
    return m_TestWin->GetChannel2Status();
}

/********************************************************
 *@Name:        GetChannel3Status
 *@Author:      HuaT
 *@Description: 获取通道3运行状态
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-12
********************************************************/
bool MainWindow::GetChannel3Status()
{
    return m_TestWin->GetChannel3Status();
}

/********************************************************
 *@Name:        SetChannel1Status
 *@Author:      HuaT
 *@Description: 设置通道1运行状态
 *@Param:       true为运行中,false为空闲
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-12
********************************************************/
void MainWindow::SetChannel1Status(bool bStatus)
{
    m_TestWin->SetChannel1Status(bStatus);
}

/********************************************************
 *@Name:        SetChannel2Status
 *@Author:      HuaT
 *@Description: 设置通道2运行状态
 *@Param:       true为运行中,false为空闲
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-12
********************************************************/
void MainWindow::SetChannel2Status(bool bStatus)
{
    m_TestWin->SetChannel2Status(bStatus);
}

/********************************************************
 *@Name:        SetChannel3Status
 *@Author:      HuaT
 *@Description: 获取通道3运行状态
 *@Param:       true为运行中,false为空闲
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-8-12
********************************************************/
void MainWindow::SetChannel3Status(bool bStatus)
{
    return m_TestWin->SetChannel3Status(bStatus);
}

/********************************************************
 *@Name:        SetHardUpdateData
 *@Author:      HuaT
 *@Description: 设置固件升级的数据
 *@Param:       从U盘中读取的固件数据，十六进制
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-11-16
********************************************************/
void MainWindow::SetHardUpdateData(QByteArray byteUpdateData,HardGrading* pHard)
{
    m_TestWin->SetHardUpdateData(byteUpdateData,pHard);
}


/********************************************************
 *@Name:        on_btnTest_clicked
 *@Author:      HuaT
 *@Description: 显示测试界面
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void MainWindow::on_btnTest_clicked()
{
    ui->stackedWidget->setCurrentWidget(m_TestWin);
    ui->btnTest->setChecked(true);
    ui->btnQC->setChecked(false);
    ui->btnQuery->setChecked(false);
    ui->btnSet->setChecked(false);

}

/********************************************************
 *@Name:        on_btnQuery_clicked
 *@Author:      HuaT
 *@Description: 显示查询界面
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void MainWindow::on_btnQuery_clicked()
{
    ui->stackedWidget->setCurrentWidget(m_QueryWin);
    ui->btnTest->setChecked(false);
    ui->btnQC->setChecked(false);
    ui->btnQuery->setChecked(true);
    ui->btnSet->setChecked(false);
}

/********************************************************
 *@Name:        on_btnQC_clicked
 *@Author:      HuaT
 *@Description: 显示质控界面
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void MainWindow::on_btnQC_clicked()
{
    ui->stackedWidget->setCurrentWidget(m_QCWin);
    ui->btnTest->setChecked(false);
    ui->btnQC->setChecked(true);
    ui->btnQuery->setChecked(false);
    ui->btnSet->setChecked(false);
}

/********************************************************
 *@Name:        on_btnSet_clicked
 *@Author:      HuaT
 *@Description: 显示设置界面
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void MainWindow::on_btnSet_clicked()
{
    ui->stackedWidget->setCurrentWidget(m_SetWin);
    m_SetWin->SetDbObj(&m_db);
    m_SetWin->UpdateDbToRenf();
    ui->btnTest->setChecked(false);
    ui->btnQC->setChecked(false);
    ui->btnQuery->setChecked(false);
    ui->btnSet->setChecked(true);
    if(m_bUpdateHardVersion){
        //更新完成后,以后不在更新
        m_bUpdateHardVersion = false;
        m_SetWin->UpdateHardVersion(m_SetParam->value(HARDVERSION));
    }
}

/********************************************************
 *@Name:        on_btnTimer_1_clicked
 *@Author:      HuaT
 *@Description: 弹出定时器1时间选择框，为闪烁状态时修改为初始状态
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void MainWindow::on_btnTimer_1_clicked()
{
    //正常按钮状态
    if(m_btnState1 == 0){
        m_SetTimer->SetTimeSn(1);
        m_SetTimer->ClearLineEdit();
        QPoint pos = this->pos();
        m_SetTimer->move(pos.x()+273,pos.y()+30);
        m_SetTimer->show();
    }
    //闪烁按钮状态
    else if( m_btnState1 == 1){
        ui->btnTimer_1->setText(QObject::tr("Timer1"));
        ui->btnTimer_1->setStyleSheet("border-image:url(:/resource/image/Timer.png);");
        m_ColorTimer1.stop();
        m_btnState1 = 0;
        //
        if(m_bBuzzState){
            m_BuzzTimer.stop();
            m_bBuzzState = false;
            //当计时器停止时如果为响铃状态，会导致关掉后一起响，so
            system("echo 0 > /sys/class/leds/usr_beep/brightness");
        }
    }
}

/********************************************************
 *@Name:        on_btnTimer_2_clicked
 *@Author:      HuaT
 *@Description: 弹出定时器2时间选择框，为闪烁状态时修改为初始状态
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void MainWindow::on_btnTimer_2_clicked()
{
    if(m_btnState2 == 0){
        m_SetTimer->SetTimeSn(2);
        m_SetTimer->ClearLineEdit();
        QPoint pos = this->pos();
        m_SetTimer->move(pos.x()+273,pos.y()+30);
        m_SetTimer->show();
    }else if( m_btnState2 == 1){
        ui->btnTimer_2->setText(QObject::tr("Timer2"));
        ui->btnTimer_2->setStyleSheet("border-image:url(:/resource/image/Timer.png);");
        m_ColorTimer2.stop();
        m_btnState2 = 0;
        //
        if(m_bBuzzState){
            m_BuzzTimer.stop();
            m_bBuzzState = false;
            //当计时器停止时如果为响铃状态，会导致关掉后一起响，so
            system("echo 0 > /sys/class/leds/usr_beep/brightness");
        }
    }
}

/********************************************************
 *@Name:        on_btnTimer_3_clicked
 *@Author:      HuaT
 *@Description: 弹出定时器3时间选择框，为闪烁状态时修改为初始状态
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void MainWindow::on_btnTimer_3_clicked()
{
    if(m_btnState3 == 0){
        m_SetTimer->SetTimeSn(3);
        m_SetTimer->ClearLineEdit();
        QPoint pos = this->pos();
        m_SetTimer->move(pos.x()+273,pos.y()+30);
        m_SetTimer->show();
    }else if( m_btnState3 == 1){
        ui->btnTimer_3->setText(QObject::tr("Timer3"));
        ui->btnTimer_3->setStyleSheet("border-image:url(:/resource/image/Timer.png);");
        m_ColorTimer3.stop();
        m_btnState3 = 0;
        //
        if(m_bBuzzState){
            m_BuzzTimer.stop();
            m_bBuzzState = false;
            //当计时器停止时如果为响铃状态，会导致关掉后一起响，so
            system("echo 0 > /sys/class/leds/usr_beep/brightness");
        }
    }
}

/********************************************************
 *@Name:        RecvPrintSerialData
 *@Author:      HuaT
 *@Description: 解析打印串口数据
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-9-12
********************************************************/
void MainWindow::RecvPrintSerialData()
{
    QByteArray PrintData;
    PrintData.append(m_SerialPrint.readAll());
    //qDebug()<<PrintData.toHex().toUpper();
    if(0x04 == (int)PrintData.at(0)){
        //qDebug()<<"ok";
        QString strTitle = QObject::tr("Note");
        QString strContent = QObject::tr("printer is out of paper");
        QMessageBox::warning(this,strTitle,strContent,QMessageBox::Ok);
    }
    //原始数据测试使用
    //emit MockTest(m_SerialPrint.readAll());
}

/********************************************************
 *@Name:        RecvHL7SerialData
 *@Author:      HuaT
 *@Description: 解析HL7串口数据
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-2-13
********************************************************/
void MainWindow::RecvHL7SerialData()
{
    //查看是否开启了PC连接模式，模式序号为3.
    int nRFSerialMode = m_SetParam->value(RFWRITEMODE,0).toInt();
    if(nRFSerialMode == 3){//转发数据模式开启,把收电脑端收到的数据发给射频卡
        QByteArray byteHL7Data = m_SerialHL7.readAll();
        m_bytePCSerialData.append(byteHL7Data);
        qDebug()<<"m_SerialHL7:"<<byteHL7Data.size()<<byteHL7Data.toHex().toUpper();
        QByteArray baFrameStart,baFrameEnd;
        baFrameStart = m_bytePCSerialData.left(4);
        baFrameEnd = m_bytePCSerialData.right(4);
        if(baFrameStart.toHex().toUInt(0,16) == FRAMESTART && baFrameEnd.toHex().toUInt(0,16) == FRAMEEND){
            m_SerialRF.write(m_bytePCSerialData);
            m_bytePCSerialData.clear();
        }
    }
    //qDebug()<<m_SerialHL7.readAll().toHex().toUpper();
}


/********************************************************
 *@Name:        RecvRfSerialData
 *@Author:      HuaT
 *@Description: 解析射频串口数据
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-18
********************************************************/
void MainWindow::RecvRfSerialData()
{
    //查看调试射频模式是否开启
    int nRFSerialMode = m_SetParam->value(RFWRITEMODE,0).toInt();
    if(nRFSerialMode == 3){//PC连接模式,收到射频卡发给电脑的数据,直接转发
        m_RfidData.append(m_SerialRF.readAll());
        //qDebug()<<m_RfidData.toHex().toUpper();
        QByteArray FrameStart,FrameEnd;
        FrameStart = m_RfidData.left(4);
        FrameEnd = m_RfidData.right(4);
        if(FrameEnd.toHex().toUInt(0,16) == FRAMEEND && FrameStart.toHex().toUInt(0,16) == FRAMESTART){
            m_SerialHL7.write(m_RfidData);
            m_RfidData.clear();
        }
        return;
    }else if(nRFSerialMode == 1 || nRFSerialMode == 2){
        return;
    }
    IDCardInfo CardInfo;
    m_RfidData.append(m_SerialRF.readAll());
    //qDebug()<<m_RfidData.toHex().toUpper();
    QByteArray FrameStart,FrameEnd;
    FrameStart = m_RfidData.left(4);
    FrameEnd = m_RfidData.right(4);
    if(FrameEnd.toHex().toUInt(0,16) == FRAMEEND && FrameStart.toHex().toUInt(0,16) == FRAMESTART){
        quint32 nCheck=0;
        //4-7为卡号
        CardInfo.m_IDCardNumber = m_RfidData.mid(4,4).toHex().toUInt(0,16);
        //8-11为命令
        CardInfo.m_IDCardCMD = m_RfidData.mid(8,4).toHex().toUInt(0,16);
        //12-15为数据长度
        CardInfo.m_IDCardLen = m_RfidData.mid(12,4).toHex().toUInt(0,16);
        //16-len为数据
        CardInfo.m_IDCardData = m_RfidData.mid(16,CardInfo.m_IDCardLen).toHex().toUpper();
        //测试项目
        //有效期
        //导入时间
        QString strCardData = m_RfidData.mid(16,CardInfo.m_IDCardLen);
        if(0 == strCardData.compare("IMPROVE QT-200")){
            //密钥卡,发送免密码信号，并清空数据
            emit NoPassword();
            m_RfidData.clear();
            QString strTitle,strContent;
            strTitle = "提示";
            strContent = "免密钥卡数据填写完成";
            QMessageBox::information(this,strTitle,strContent,QMessageBox::Ok);
            return;
        }
        //16到16+14为条码编号
        if(m_RfidData.size() > 210){
            CardInfo.m_IDCardBarCode = m_RfidData.mid(16,14);
        }else{
            CardInfo.m_IDCardBarCode = m_RfidData.mid(16,14).toHex().toUpper();
        }

        //len+16-len+20为检验码
        CardInfo.m_IDCardCheck = m_RfidData.mid(CardInfo.m_IDCardLen+16,4).toHex().toUInt(0,16);
        for(int n=12; n<CardInfo.m_IDCardLen+4+12; n++){
            nCheck += (quint8)m_RfidData.at(n);
            //qDebug()<<n<<(quint8)m_RfidData.at(n);
        }
        quint32 nCheckFinal = ~nCheck+1;
        if(CardInfo.m_IDCardCheck != nCheckFinal){
            //qDebug()<<"IDCard: "<<CardInfo.m_IDCardCheck;
            //qDebug()<<"nCheckFinal: "<<nCheckFinal;
            qDebug()<<"Check RFID Data Error";
            return ;
        }
        //qDebug()<<CardInfo.m_IDCardData;
        //检验过后，如果正确则插入数据库，否则提示
        QString msgTitle,msgContain;
        QString strQueryCMD,strUpdataCMD,strInsertCMD;
        strQueryCMD = QString("select BarCode from IDCard where BarCode='%1'")
                .arg(CardInfo.m_IDCardBarCode);
        //qDebug()<<strQueryCMD;
        QSqlQuery* sqlQuery = m_db.GetSqlQuery();
        //插入项目名称
        QString strCMD = "select item from itemtype";
        QStringList listItemName = m_db.ExecQuery(strCMD);
        //项目名称,有效期,录入时间
        QString strItemName,strValidData,strInputData;
        //批号,卡号(5-4)
        QString strBatchNumber,strCardNumber;
        if(m_RfidData.size() > 210){
            strItemName = m_RfidData.mid(62,20).trimmed();
            strValidData = QString("20%1-%2-%3").arg((quint8)m_RfidData.at(48)).arg(m_RfidData.at(49),2,10,QLatin1Char('0')).arg(m_RfidData.at(50),2,10,QLatin1Char('0'));
            //批号
            strBatchNumber = QString("20%1").arg(QString(m_RfidData.mid(26,4)));;
            //卡号
            strCardNumber = QString("%1-%2").arg(CardInfo.m_IDCardBarCode.mid(3,4).toInt(0,2)+1)
                                            .arg(CardInfo.m_IDCardBarCode.mid(8,5).toInt(0,2)+1);
        }else{
            //项目名称
            strItemName = m_RfidData.mid(167,15).trimmed();
            //有效期
            strValidData = QString("20%1%2-%3%4-%5%6").arg((quint8)m_RfidData.at(105)).arg((quint8)m_RfidData.at(106)).arg((quint8)m_RfidData.at(107))
                    .arg((quint8)m_RfidData.at(108)).arg((quint8)m_RfidData.at(109)).arg((quint8)m_RfidData.at(110));
            //批号
            strBatchNumber = "";
            QString strCardNo;
            quint8 nTemp=0;
            for(int n=0; n<CardInfo.m_IDCardBarCode.size(); n+=2){
                nTemp = CardInfo.m_IDCardBarCode.mid(n,2).toInt();
                strCardNo.append(QString::number(nTemp));
            }
            //卡号
            strCardNumber = QString("%1-%2").arg(strCardNo.mid(3,4).toInt(0,2)+1)
                                            .arg(strCardNo.mid(8,5).toInt(0,2)+1);
        }

        QDateTime dt = QDateTime::currentDateTime();
        strInputData = dt.toString("yyyy-MM-dd hh:mm:ss");
        if(!listItemName.contains(strItemName)){
            strCMD = QString("insert into itemtype values(null,'%1')").arg(strItemName);
            if(!m_db.Exec(strCMD)){
                qDebug()<<"item name Insert error";
            }
        }
        if(sqlQuery->exec(strQueryCMD)){
            if(sqlQuery->next()){
                //如果有数据，则更新
                //msgTitle = QObject::tr("Note");
                //msgContain = QObject::tr("Batch number already exists. Overwrite?");
                //int nRet = QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Ok|QMessageBox::Cancel);
                //if(nRet == QMessageBox::Ok){
                strUpdataCMD = QString("update IDCard set cardno=%1,BarCode='%2',item='%3',BatchNumber='%4',CardNumber='%5',validDate='%6',inputtime='%7',data='%8' where BarCode='%9';")
                            .arg(CardInfo.m_IDCardNumber)
                            .arg(CardInfo.m_IDCardBarCode)
                            .arg(strItemName)
                            .arg(strBatchNumber)
                            .arg(strCardNumber)
                            .arg(strValidData)
                            .arg(strInputData)
                            .arg(CardInfo.m_IDCardData)
                            .arg(CardInfo.m_IDCardBarCode);
                    //qDebug()<<strUpdataCMD;
                    if(sqlQuery->exec(strUpdataCMD)){
                        msgContain=QObject::tr("Data Update is successful");
                        QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Ok);
                    }else{
                        msgContain=QObject::tr("Data Update is failed");
                        QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Ok);
                    }
                //}
            }else{
                //没有数据，则插入
                strInsertCMD = QString("insert into IDCard values(null,%1,'%2','%3','%4','%5','%6','%7','%8');")
                        .arg(CardInfo.m_IDCardNumber)
                        .arg(CardInfo.m_IDCardBarCode)
                        .arg(strItemName)
                        .arg(strBatchNumber)
                        .arg(strCardNumber)
                        .arg(strValidData)
                        .arg(strInputData)
                        .arg(CardInfo.m_IDCardData);
                //qDebug()<<strCMD;
                if(sqlQuery->exec(strInsertCMD)){
                    msgContain=QObject::tr("Data import is successful");
                    QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Ok);
                }else{
                    msgContain=QObject::tr("Data import is failed");
                    QMessageBox::information(this,msgTitle,msgContain,QMessageBox::Ok);
                }
            }
        }else{
            qDebug()<<"RecvSerialData: select failed";
        }
        m_RfidData.clear();
    }
}


/********************************************************
 *@Name:        on_btnTimer_3_clicked
 *@Author:      HuaT
 *@Description: 接收对应按钮的倒计时槽函数
 *@Param1:      倒计时的时间，单位为分钟
 *@Param2:      倒计时的按钮号，分别为按钮1、按钮2、按钮3
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void MainWindow::StartTime(int time, int SerialNum)
{
    switch(SerialNum){
    case 1:
        m_isRun1 = true;
        //m_Time1 = time;
        m_Time1 = time * 60;
        m_Timer1.start(1000);
        ui->btnTimer_1->setDisabled(true);
        break;
    case 2:
        m_isRun2 = true;
        m_Time2 = time * 60;
        m_Timer2.start(1000);
        ui->btnTimer_2->setDisabled(true);
        break;
    case 3:
        m_isRun3 = true;
        m_Time3 = time * 60;
        m_Timer3.start(1000);
        ui->btnTimer_3->setDisabled(true);
        break;
    default:
        qDebug()<<"SerialNum 输入数据不符合要求";
        break;
    }
}

/********************************************************
 *@Name:        TimeChange1
 *@Author:      HuaT
 *@Description: 每秒改变按钮1的时间
 *@Param1:      无
 *@Param2:      无
 *@Return:      无
 *@Version:     0.1
 *@Date:        2016-6-3
********************************************************/
void MainWindow::TimeChange1()
{
    if(m_isRun1){
        if(m_Time1 < 0 ){
            //打开按钮
            ui->btnTimer_1->setEnabled(true);
            //转换运行状态为关闭
            m_isRun1 = false;
            //停止定时器
            m_Timer1.stop();
            //切换按钮状态为闪烁状态
            m_btnState1 = 1;
            //开启颜色转换
            m_ColorTimer1.start(250);
            //
            if(!m_bBuzzState){
                m_BuzzTimer.start(250);
                m_bBuzzState = true;
            }
        }else{
            QString showTime = QString("%1S").arg(m_Time1);
            ui->btnTimer_1->setText(showTime);
            m_Time1--;
        }
    }

}

/********************************************************
 *@Name:        TimeChange2
 *@Author:      HuaT
 *@Description: 每秒改变按钮2的时间
 *@Param1:      无
 *@Param2:      无
 *@Return:      无
 *@Version:     0.1
 *@Date:        2016-6-3
********************************************************/
void MainWindow::TimeChange2()
{
    if(m_isRun2){
        if(m_Time2 < 0 ){
            //打开按钮
            ui->btnTimer_2->setEnabled(true);
            //转换运行状态为关闭
            m_isRun2 = false;
            //停止定时器
            m_Timer2.stop();
            //切换按钮状态为闪烁状态
            m_btnState2 = 1;
            //开启颜色转换
            m_ColorTimer2.start(250);
            //
            if(!m_bBuzzState){
                m_BuzzTimer.start(250);
                m_bBuzzState = true;
            }
        }else{
            QString showTime = QString("%1S").arg(m_Time2);
            ui->btnTimer_2->setText(showTime);
            m_Time2--;
        }
    }

}

/********************************************************
 *@Name:        TimeChange3
 *@Author:      HuaT
 *@Description: 每秒改变按钮3的时间
 *@Param1:      无
 *@Return:      无
 *@Version:     0.1
 *@Date:        2016-6-3
********************************************************/
void MainWindow::TimeChange3()
{
    if(m_isRun3){
        if(m_Time3 < 0 ){
            //打开按钮
            ui->btnTimer_3->setEnabled(true);
            //转换运行状态为关闭
            m_isRun3 = false;
            //停止定时器
            m_Timer3.stop();
            //切换按钮状态为闪烁状态
            m_btnState3 = 1;
            //开启颜色转换
            m_ColorTimer3.start(250);
            //
            if(!m_bBuzzState){
                m_BuzzTimer.start(250);
                m_bBuzzState = true;
            }
        }else{
            QString showTime = QString("%1S").arg(m_Time3);
            ui->btnTimer_3->setText(showTime);
            m_Time3--;
        }
    }

}

/********************************************************
 *@Name:        ColorChange1
 *@Author:      HuaT
 *@Description: 按钮颜色改变
 *@Param1:      无
 *@Return:      无
 *@Version:     0.1
 *@Date:        2016-6-4
********************************************************/
void MainWindow::ColorChange1()
{
    if(m_ColorState1){
        m_ColorState1 = false;
        ui->btnTimer_1->setStyleSheet("border-image:url(:/resource/image/Timer.png);");
    }else{
        m_ColorState1 = true;
        ui->btnTimer_1->setStyleSheet("border-image:url(:/resource/image/TimerChange.png);");
    }
}

/********************************************************
 *@Name:        ColorChange2
 *@Author:      HuaT
 *@Description: 按钮颜色改变
 *@Param1:      无
 *@Return:      无
 *@Version:     0.1
 *@Date:        2016-6-4
********************************************************/
void MainWindow::ColorChange2()
{
    if(m_ColorState2){
        m_ColorState2 = false;
        ui->btnTimer_2->setStyleSheet("border-image:url(:/resource/image/Timer.png);");
    }else{
        m_ColorState2 = true;
        ui->btnTimer_2->setStyleSheet("border-image:url(:/resource/image/TimerChange.png);");
    }
}

/********************************************************
 *@Name:        ColorChange3
 *@Author:      HuaT
 *@Description: 按钮颜色改变
 *@Param1:      无
 *@Return:      无
 *@Version:     0.1
 *@Date:        2016-6-4
********************************************************/
void MainWindow::ColorChange3()
{
    if(m_ColorState3){
        m_ColorState3 = false;
        ui->btnTimer_3->setStyleSheet("border-image:url(:/resource/image/Timer.png);");
    }else{
        m_ColorState3 = true;
        ui->btnTimer_3->setStyleSheet("border-image:url(:/resource/image/TimerChange.png);");
    }
}

void MainWindow::BuzzSound()
{
    if(m_bBuzzChange){
        m_bBuzzChange = false;
        system("echo 1 > /sys/class/leds/usr_beep/brightness");
    }else{
        m_bBuzzChange = true;
        system("echo 0 > /sys/class/leds/usr_beep/brightness");
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
void MainWindow::changeEvent(QEvent *e)
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

/********************************************************
 *@Name:        event
 *@Author:      HuaT
 *@Description: 关机事件
 *@Param:       事件类型
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-20
********************************************************/
bool MainWindow::event(QEvent *event)
{
    QString msgTitle,msgConTain;
    //qDebug()<<event->type();
    switch(event->type()){
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    {
        QKeyEvent *ke = (QKeyEvent *) event;
        qDebug()<< ke->key();
        if(ke->key() == 16777483){
            msgTitle = QObject::tr("Note");
            msgConTain = QObject::tr("Close the instrument?");
            if(QMessageBox::Yes == QMessageBox::information(this, msgTitle, msgConTain, QMessageBox::Yes | QMessageBox::No)){
                //qApp->exit(0);
                //system("poweroff");
                //
                QByteArray byteRespond;
                byteRespond[0] = 0x00;
                m_TestWin->PackageCanMsg(QT_TESTOPT_SHUTDOWN,QT_NULL_SYNC,1,byteRespond);
                m_TestWin->PackageCanMsg(QT_TESTOPT_SHUTDOWN,QT_NULL_SYNC,2,byteRespond);
                m_TestWin->PackageCanMsg(QT_TESTOPT_SHUTDOWN,QT_NULL_SYNC,3,byteRespond);
            }
            return true;
        }
        break;
    }
    }
    QWidget::event(event);
}

/********************************************************
 *@Name:        ShowSysTime
 *@Author:      HuaT
 *@Description: 显示系统时间，每分钟更新一次
 *@Param1:      无
 *@Return:      无
 *@Version:     0.1
 *@Date:        2016-6-4
********************************************************/
void MainWindow::ShowSysTime()
{
    QDateTime time = QDateTime::currentDateTime();
    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");
    ui->lbDate->setText(strTime);
}


