/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: testwindow.h
 *  简要描述: 测试界面头文件，实现数据检测、存储
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include<QWidget>
#include<QDebug>
#include<QSqlQuery>
#include<QSqlDatabase>
#include<QTimer>
#include<QDateTime>
#include"cutilsettings.h"
#include"resultcalc.h"
#include"CanCmd.h"
#ifdef Q_OS_LINUX
#include"canbus.h"
#endif
#include"qextserial/qextserialport.h"
#include"hardgrading.h"
#include"cqtprodb.h"
#include"csimplehl7.h"

namespace Ui {
class TestWindow;
}

struct ResultDataInfo{
    //本次测试的样本编号
    quint64 m_nNumberID;
    //姓名
    QString m_strName;
    //年龄
    QString m_strAge;
    //性别
    QString m_strSex;
    //测试数据的同步字编号
    quint8 m_nSyncID;
    //测试数据的长度-1280
    quint32 m_nDataLen;
    //测试数据
    QByteArray m_byteCanData;
    //测试批号
    QString m_strIDCardBatchNo;
    //测试ID卡信息
    QString m_strIDMessage;
    //测试名称
    QString m_strTestName;
    //测试单位
    QString m_strTestUnit;
    //测试结果
    QString m_strResult;
    //反应时间
    qint32 m_nReactionTime;
    //试剂有效期
    QString m_strValidDate;
    //初始化
    ResultDataInfo(){
        m_byteCanData.clear();
        m_nDataLen = 0;
        m_nSyncID = 0;
        m_strIDCardBatchNo = "";
        m_strIDMessage = "";
        m_strResult = "";
        m_strTestName = "";
        m_strTestUnit = "";
        m_nNumberID = 0;
        m_nReactionTime = 0;
        m_strValidDate = "";
        m_strName = "";
        m_strAge = "";
        m_strSex = "";
    }
};

class TestWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TestWindow(QWidget *parent = 0);
    ~TestWindow();

private slots:
    void on_btnNew1_clicked();
    void on_btnNew2_clicked();
    void on_btnNew3_clicked();
    void on_btnPrint1_clicked();
    void on_btnAutoTest1_clicked();
    void on_btnInstantTest1_clicked();
    void on_btnPrint2_clicked();
    void on_btnAutoTest2_clicked();
    void on_btnInstantTest2_clicked();
    void on_btnPrint3_clicked();
    void on_btnAutoTest3_clicked();
    void on_btnInstantTest3_clicked();

private slots:
    void RecvCanMsg(quint32 can_id,QByteArray can_data);
    //计算反应时间
    void CalcReactionTime1();
    void CalcReactionTime2();
    void CalcReactionTime3();
    //接收串口数据
    void RecvSerialData();
    //接收HL7串口数据
    void RecvHL7SerialData();
    //打印模式更改
    void PrintModelChange();
    //设置更改
    void SettingChange();
    //延时打开控件
    void DaleyOpenControl1();
    void DaleyOpenControl2();
    void DaleyOpenControl3();
    //执行老化测试
    void AgingTest1();
    void AgingTest2();
    void AgingTest3();
    //停止老化计时器
    void StopAgingTimer();

signals:
    void UpdateQCResultMsg(QString strResult);
    void UpdateQCControlStatus();

public:
    //获取通信对象
#ifdef Q_OS_LINUX
    CanBus& GetCanBus();
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

    //设置升级文件数据
    void SetHardUpdateData(QByteArray byteUpdateData, HardGrading *pHard);

    //获取参考值
    //QString GetRenfValue(QString strAge, QString strSex, QString strItem);
    //获取结果标志
    //QString GetResultFlag(QString strRenf,QString strResult);

private:
    Ui::TestWindow *ui;
    //自动测试模式
    bool m_bAutoMode1;
    bool m_bAutoMode2;
    bool m_bAutoMode3;
    //配置文件对象
    CUtilSettings* m_settings;
    //配置文件参数表
    QMap<QString,QString>* m_SetParam;
    //是否为自动打印模式
    bool m_isAutoPrint;
    //是否为自动编码模式
    bool m_isAutoEncode;
    //是否为质控结果
    bool m_isQCResult;
    //当前有效的编号
    quint64 m_nValidNumber;
    //数据库对象引用
    QSqlDatabase *m_SqlDB;
    //数据库查询对象
    QSqlQuery    *m_Query;
    //通道1的测试状态
    bool m_TestStatus1;
    //通道2的测试状态
    bool m_TestStatus2;
    //通道3的测试状态
    bool m_TestStatus3;
#ifdef Q_OS_LINUX
    //Can通信对象
    CanBus m_CanBus;
#endif
    //结果计算对象
    ResultCalc m_ResultCalc;
    //通道1计时时间
    QTimer m_timerClock1;
    QTimer m_timerClock2;
    QTimer m_timerClock3;
    //老化测试计时时间
    QTimer m_timerAging1;
    QTimer m_timerAging2;
    QTimer m_timerAging3;
    //通道延时时间
    QTimer m_timerDaleyOpen;
    //最后有效编号
    quint64 m_nLastValidNumber;
    //重发标致
    bool m_bSetupTestFlag;
    bool m_bStartTestFlag;
    bool m_bStopTestFlag;
    //结果信息成员
    ResultDataInfo m_Candata1;
    ResultDataInfo m_Candata2;
    ResultDataInfo m_Candata3;
    //打印串口
    QextSerialPort* m_SerialPrint;
    //HL7串口
    QextSerialPort* m_SerialHL7;
    //固件升级数据
    QByteArray m_byteHardUpdateData1;
    QByteArray m_byteHardUpdateData2;
    QByteArray m_byteHardUpdateData3;
    //数据发送索引
    quint32 m_nIndex1;
    quint32 m_nIndex2;
    quint32 m_nIndex3;
    //数据发送标致,是否为初始帧
    bool m_bIsStart;
    //数据长度
    quint32 m_nUpdataDataLeng1;
    quint32 m_nUpdataDataLeng2;
    quint32 m_nUpdataDataLeng3;
    //固件升级框对象
    HardGrading* m_pHardGrading;
    //数据库指针对象
    CQtProDB* m_db;
    //
    CSimpleHL7 m_SimpleHL7;

private:
    //回车事件过滤函数
    bool eventFilter(QObject *obj, QEvent *event);

public:
    //封装的Can消息发送
    void PackageCanMsg(quint8 nCMD, quint8 nSync, quint8 nChannel, QByteArray can_data);
    //设置测试状态
    void SetTestStatus(int nChannel, bool bStatus);
private:
    //解析返回的ACK数据
    void ParseACK(int nSync, QByteArray can_data, quint8 nChannel);
    //解析测试数据
    void ParseTestData(quint8 nSync, QByteArray can_data, quint8 nChannel, ResultDataInfo& DataObj);
    //计算结果
    unsigned char *CalcResult(ResultDataInfo DataObj);
    //结果出来后，处理结果
    void ProcessResult(quint8 nChannel);
    //解析条码号
    QString ParseBarCode(QByteArray can_data);
    //获得指定批号的ID卡信息
    QString GetIDMessageInfo(QString strBatchNo);
    //解析ID卡数据
    void ParseIDMessageInfo(unsigned char* pIDMessage, quint8& StartPoint, quint8& Amp, ResultDataInfo &DataObj);
    //处理接收到的条码数据
    bool ProcessBarCode(quint8 nChannel, bool& bTestStatus, ResultDataInfo &data, QByteArray can_data);
    //处理警告消息
    void ProcessAlarm(quint8 nChannel, QByteArray byteCanData);
    //显示测试项目
    void ShowTestItem(quint8 nChannel,QString strTestName);
    //显示测试结果
    void ShowTestResult(quint8 nChannel,QString strTestResult,QString strTestUnit);
    //保存测试结果到数据库
    void SaveResultToDB(ResultDataInfo DataObj);
    //清除结果结构体数据
    void ClearResultDataInfo(ResultDataInfo& DataObj);
    //添加日志内容
    bool InsertLogMsg(int nType, quint64 nNumber, QString strContent);
    //检测试剂卡有效期
    bool CheckValidDate(QString strValidDate);
    //检测测试通道状态
    bool CheckTestStatus(quint8 nChannel);
    //开始发送固件升级数据
    void StartHardUpdate(quint8 nChannel, quint32 nIndex);

private:
    //初始化进度条
    void InitRoundBar();
    //初始化通道状态
    void InitChannelState(quint8 nChannel, bool bEnable);
    //新建时通道状态
    void SetChannelStateToNew(quint8 nChannel, bool bEnable);
    //自动测试时通道状态
    void SetChannelStateToAutoTest(quint8 nChannel, bool bEnable);
    //自动测试结束时按钮状态
    void AutoTestButtonState(quint8 nChannel, bool bEnable);
    //即时测试时通道状态
    void SetChannelStateToInTest(int nChannel, bool bEnable);
    //初始化通道2状态
    //void InitChannelState2(bool bEnable);
    //初始化通道3状态
    //void InitChannelState3(bool bEnable);
    //清除通道信息
    void CleareChannelInfo(int nChannel);
    //插入测试用户信息到数据库
    int InsertTestInfoToDb(int nChannel);
    //过虑语言事件
    void changeEvent(QEvent *e);
    //获取有效可用编号
    quint64 GetValidNumber();
    //初始化测试模式状态，获取检测时需要的状态
    void InitRunState();
    //更新打印按钮文本
    void UpdatePrintText();
    //更新测试按钮文本
    void UpdateTestText(int nChannel);
    //更新打印状态
    void UPdatePrintState(int nChannel);
    //检测用户输入编号是否存在
    bool CheckInputInfo(quint64 nNumber);
    //自动测试
    void AutoTest(int nChannel, bool& bIsAutoTesting, ResultDataInfo &DataObj, QTimer& timer);
    //即时测试
    void InstantTest(int nChannel);
    //新建测试
    void NewTest(int nChannel);
    //自动输入编码
    void AutoEncode(int nChannel);
    //扫描输入编码
    void ScanningEncode(int nChannel);
    //数据打印
    void PrintData(QString strName, QString strNumber, QString strAge, QString strSex, QString strItem, QString strResult);
    //自动打印
    void AutoPrint(int nChannel);
    //更改测试模式状态
    void ChangeAutoModeStates(int nChannel, bool States);
    //获取偏移量信息
    void GetAPOffSetInfo();
    //获取硬件版本信息
    void GetHardVersion();

};

#endif // TESTWINDOW_H
