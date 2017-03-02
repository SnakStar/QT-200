/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: mainwindow.h
 *  简要描述: 主框架头文件，进行对象声明
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include"testwindow.h"
#include"qcwindow.h"
#include"querywindow.h"
#include"setwindow.h"
#include"settimer.h"
#include"cqtprodb.h"
#include"cutilsettings.h"
#ifdef Q_OS_LINUX
#include"canbus.h"
#endif
#include"qextserial/qextserialport.h"

#include<QMainWindow>
#include<QTimer>
#include<QDateTime>
#include<QEvent>
#include<QKeyEvent>
#include<QProcess>

#define FRAMESTART 0X5A1AA1A5
#define FRAMEEND   0XA5A11A5A

struct IDCardInfo{
    quint32    m_IDCardNumber;
    quint32    m_IDCardCMD;
    quint32    m_IDCardLen;
    QString    m_IDCardData;
    QString    m_IDCardBatchNo;
    quint32    m_IDCardCheck;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //获取数据库查询对象
    QSqlQuery* GetSqlQuery();
    //获取数据库对象
    CQtProDB* GetQtProDb();
    //获取数据库对象
    QSqlDatabase* GetSqlDatabase();
    //获取配置文件对象
    CUtilSettings* GetUtilSetting();
    //获取查询窗口对象
    QueryWindow* GetQueryWindow();
    //获取测试窗口对象
    TestWindow* GetTestWindow();
    //获取打印串口对象
    QextSerialPort* GetPrintSerialPort();
    //获取HL7串口对象
    QextSerialPort* GetHL7SerialPort();
    //获取Can通信对象
#ifdef Q_OS_LINUX
    CanBus *GetCanBus();
#endif
    //获取通道1状态
    bool GetChannel1Status();
    //获取通道2状态
    bool GetChannel2Status();
    //获取通道3状态
    bool GetChannel3Status();

    //设置通道1状态
    void SetChannel1Status(bool bStatus);
    //设置通道2状态
    void SetChannel2Status(bool bStatus);
    //设置通道3状态
    void SetChannel3Status(bool bStatus);
    //设置固件升级数据
    void SetHardUpdateData(QByteArray byteUpdateData, HardGrading *pHard);
    
private slots:
    //测试按钮事件
    void on_btnTest_clicked();

    //查询按钮事件
    void on_btnQuery_clicked();

    //质控按钮事件
    void on_btnQC_clicked();

    //设置按钮事件
    void on_btnSet_clicked();

    //计时器1事件
    void on_btnTimer_1_clicked();

    //计时器2事件
    void on_btnTimer_2_clicked();

    //计时器3事件
    void on_btnTimer_3_clicked();

private:
    Ui::MainWindow *ui;
    //RF串口对象
    QextSerialPort m_SerialRF;
    //打印串口对象
    QextSerialPort m_SerialPrint;
    //HL7串口对象
    QextSerialPort m_SerialHL7;
    //配置文件对象
    CUtilSettings m_settings;
    //配置文件参数表
    QMap<QString,QString>* m_SetParam;
    //数据库对象
    CQtProDB m_db;
    //软件版本
    QString m_Version;
    //测试界面实例对象
    TestWindow    *m_TestWin;
    //查询界面实例对象
    QueryWindow   *m_QueryWin;
    //质控界面实例对象
    QCWindow      *m_QCWin;
    //设置界面实例对象
    SetWindow     *m_SetWin;
    //设置计时器时间
    SetTimerObj   *m_SetTimer;
    //蜂鸣器定时器
    QTimer        m_BuzzTimer;
    //蜂鸣器状态
    bool          m_bBuzzState;
    bool          m_bBuzzChange;

//按钮1属性
    //按钮1定时器
    QTimer         m_Timer1;
    //按钮1倒计时时间
    int            m_Time1;
    //按钮1运行时状态
    int            m_btnState1;
    //定时器1是否在运行
    bool           m_isRun1;
//按钮2属性
    //按钮2定时器
    QTimer         m_Timer2;
    //按钮2倒计时时间
    int            m_Time2;
    //按钮2运行时状态
    int            m_btnState2;
    //定时器2是否在运行
    bool           m_isRun2;
//按钮3属性
    //按钮3定时器
    QTimer         m_Timer3;
    //按钮3倒计时时间
    int            m_Time3;
    //按钮3运行时状态
    int            m_btnState3;
    //定时器3是否在运行
    bool           m_isRun3;

    //颜色变换定时器
    QTimer         m_ColorTimer1;
    QTimer         m_ColorTimer2;
    QTimer         m_ColorTimer3;
    bool           m_ColorState1;
    bool           m_ColorState2;
    bool           m_ColorState3;
    //系统时间定时器
    QTimer         m_SystemTimer;
    //电池定时器
    QTimer         m_BatteryTimer;
    //射频卡数据
    QByteArray     m_RfidData;
    //更新硬件版本
    bool           m_bUpdateHardVersion;
private slots:
    //接收射频串口数据
    void RecvRfSerialData();
    //接收打印机串口数据
    void RecvPrintSerialData();
    //接收HL7串口数据
    void RecvHL7SerialData();
    //显示系统时间
    void ShowSysTime();
    //计算倒计时时间
    void StartTime(int time,int SerialNum);
    //倒计时槽
    void TimeChange1();
    void TimeChange2();
    void TimeChange3();
    //颜色槽
    void ColorChange1();
    void ColorChange2();
    void ColorChange3();
    //蜂鸣器槽
    void BuzzSound();
    //检测并设置电池图标状态
    void SetBatteryState();
private:
    //
    bool event(QEvent *event);
    //
    void changeEvent(QEvent *e);
    //检测配置文件是否存在
    void CheckSettingFile();
    //初始化RF卡串口
    bool InitRfSerial();
    //初始化打印串口
    bool InitPrintSerial();
    //初始化HL7串口
    bool InitHl7Serial();
    //更改电池图标
    void ChangeBatteryImage(QString strPath);

};

#endif // MAINWINDOW_H
