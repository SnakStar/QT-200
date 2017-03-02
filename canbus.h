/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: CanMsg.h
 *  简要描述: 头文件，实现Can口数据收发
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef CANBUS_H
#define CANBUS_H
#include<QString>
#include <QObject>
#include<QMessageBox>
#include<QDebug>
#include<QThread>
#include<QSocketNotifier>

#include <unistd.h>  //write、read
//#include<linux/socket.h>
#ifdef Q_OS_LINUX
#include<sys/socket.h>
#include<fcntl.h>
#include<linux/can.h>
#include<linux/can/error.h>
#include<linux/can/raw.h>
#include<net/if.h>
#include<sys/ioctl.h>
#endif
#include<stdio.h>
#include<string.h>

class CanBus : public QObject
{
    Q_OBJECT
private:
    //套接字对象
    int m_Soc;
    //套接字是否打开
    bool m_bOpen;
    //套接字地址
    struct sockaddr_can m_addr;
    //can端口
    QString m_CanPort;
    //can口数据
    QByteArray m_CanData;
    //帧ID
    quint32 m_CanID;
    //读取信号
    QSocketNotifier* m_Notify;

private:
    //初始化套接字
    bool InitSocket(char *cPort);
public:
    explicit CanBus(QObject *parent = 0);
    ~CanBus();
public:
    //打开Can口
    bool StartCan(char* cPort, int nBitrate);
    //端口是否打开
    bool isOpen();
    //发送数据
    bool SendMsg(QByteArray byteID, int nCanDlc, QByteArray byteData, bool bEff=true);
    //关闭Can口
    void StopCan();
    //获取错误贞信息
    void GetErrorFrameInfo(uint nID, QByteArray& data, int  dlc);
    //十六进制字节数组转十进制ID
    quint32 ByteToUIntID(QByteArray can_id);
    //ID号转字节数组
    QByteArray UIntIDToByte(quint32 can_id);

signals:
    //结果接收完成
    void RecvDataMsg(quint32 nCanID, QByteArray qba);
public slots:
    //接收结果
     bool RecvMsg();
};

#endif // CANBUS_H
