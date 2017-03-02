/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: CanMsg.cpp
 *  简要描述: 实现文件，实现Can口数据收发
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "canbus.h"

CanBus::CanBus(QObject *parent) :
    QObject(parent)
{
    m_bOpen = false;
}

CanBus::~CanBus()
{
    StopCan();
}

/********************************************************
 *@Action:     初始化套接字并进行绑定
 *@param1:  需要绑定的端口
 *@param2:   无
 *@Return:    绑定端口是否成功
 *@Author:    HuaT
 *@Date:        2016-05-22
 *@Vesion:     1.0
 ********************************************************/
bool CanBus::InitSocket(char* cPort)
{
    struct ifreq ifr;
    struct sockaddr_can addr;
    QString msgTitel = QObject::tr("Faile");
    QString msgContain = QObject::tr("Open the socket failed");

    m_Soc= socket(PF_CAN,SOCK_RAW,CAN_RAW);
    if(m_Soc < 0)
    {
        QMessageBox::StandardButton sb = QMessageBox::information(NULL,msgTitel,msgContain,QMessageBox::Ok);
        if(sb == QMessageBox::Ok){
            qDebug()<<"Open the socket failed";
			return false;
        }
    }
    m_Notify = new QSocketNotifier(m_Soc,QSocketNotifier::Read,this);
    connect(m_Notify,SIGNAL(activated(int)), this, SLOT(RecvMsg()) );

    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name,cPort);
    if(ioctl(m_Soc,SIOCGIFINDEX,&ifr))
    {
        msgContain = QObject::tr("ioctl error");
        QMessageBox::information(NULL,msgTitel,msgContain,QMessageBox::Ok|QMessageBox::Cancel);
        qDebug()<<"ioctl error";
        return false;
    }
    addr.can_ifindex = ifr.ifr_ifindex;
    //fcntl(m_Soc, F_SETFL, O_NONBLOCK);
    if(bind(m_Soc,(struct sockaddr*)&addr,sizeof(addr)) < 0)
    {
        msgContain = QObject::tr("bind error");
        QMessageBox::information(NULL,msgTitel,msgContain,QMessageBox::Ok|QMessageBox::Cancel);
        qDebug()<<"bind error";
        return false;
    }
    m_bOpen = true;
    return true;
}

/********************************************************
 *@Action:     打开Can口并绑定套接字
 *@param1:  打开的端口号
 *@param2:  需要设置的波特率
 *@Return:    绑定套接字是否成功
 *@Author:    HuaT
 *@Date:        2016-5-22
 *@Vesion:     1.0
 ********************************************************/
bool CanBus::StartCan(char *cPort, int nBitrate)
{
    int nResult;
    m_CanPort = cPort;
    QString strCmd ;
    //strCmd = QString("canconfig can%1 stop").arg(nPort);
    //system(strCmd.toLatin1().data());
    strCmd = QString("canconfig %1  bitrate  %2").arg(cPort).arg(nBitrate);
    nResult= system(strCmd.toLatin1().data());
    strCmd = QString("canconfig %1 start").arg(cPort);
    nResult = system(strCmd.toLatin1().data());
    qDebug()<<QString("%1").arg(WEXITSTATUS(nResult));
    return  InitSocket(cPort);
}

/********************************************************
 *@Action:     Can口是否打开
 *@param1:     无
 *@Return:     返回打开状态,真为打开,假为关闭
 *@Author:     HuaT
 *@Date:       2016-5-22
 *@Vesion:     1.0
 ********************************************************/
bool CanBus::isOpen()
{
    return m_bOpen;
}


/********************************************************
 *@Action:     发送消息
 *@param1:   帧序号
 *@param2:   帧长度
 *@param3:   发送的数据内容
 *@param4:   是否为扩展帧
 *@Return:    是否发送成功
 *@Author:    HuaT
 *@Date:        2016-06-25
 *@Vesion:     1.0
 ********************************************************/
bool CanBus::SendMsg(QByteArray byteID, int nCanDlc, QByteArray byteData, bool bEff)
{
    quint32 nCanID = ByteToUIntID(byteID);
    struct can_frame frame;
    if(bEff){
        frame.can_id = nCanID | CAN_EFF_FLAG;
    }else{
        frame.can_id = nCanID;
    }
    frame.can_dlc = nCanDlc;
    //strcpy((char*)frame.data,strMsg.c_str());
    for(int i=0; i<nCanDlc; i++){
        frame.data[i] = byteData[i];
    }

    int nSendCount;
    nSendCount = write(m_Soc,&frame,sizeof(struct can_frame));
    if(nSendCount != sizeof(struct can_frame))
    {
        return false;
    }else{
        return true;
    }
}

/********************************************************
 *@Action:   接收Can口数据
 *@param:    无
 *@Return:   返回是否有数据接收
 *@Author:
 *@Date:     2016-5-22
 *@Vesion:   0.1
 ********************************************************/
bool CanBus::RecvMsg()
{
    m_CanData.clear();
    struct can_frame* frame = new can_frame;
    int nBytes=-1;

    struct timeval tvTimeout;
    tvTimeout.tv_sec = 5;
    tvTimeout.tv_usec = 5000;

    fd_set fs_read;
    FD_ZERO(&fs_read);
    FD_SET(m_Soc,&fs_read);
    int ret = 0;
    ret = select((int)m_Soc+1,&fs_read,NULL,NULL,&tvTimeout);
    //recv timeout
    if(ret ==0){
        return false;
    }
    //select error
    if(ret < 0){
        return false;
    }

    if( (nBytes = read(m_Soc,frame,sizeof(struct can_frame))) < 0){
        return false;
    }else{
        //char* strMsg = new char[10];
       //strcpy(strMsg,(char*)frame->data);
       //QMessageBox::information(NULL,"recv",strMsg,QMessageBox::Ok);
        for(int i=0; i<frame->can_dlc; i++){
            m_CanData.append ((int) frame->data[i] );
        }

        //frame->can_id = 9f100103
        //m_canid = 1f100103,   m_canid&CAN_EFF_FLAG=80000000;
        //IF:m_canid=0x123, m_canid&CAN_EFF_FLAG=0;
        if(frame->can_id & CAN_EFF_FLAG){
            m_CanID = frame->can_id & CAN_EFF_MASK;
        }else{
            m_CanID = frame->can_id & CAN_SFF_MASK;
        }
        GetErrorFrameInfo(frame->can_id,m_CanData,frame->can_dlc);

        emit RecvDataMsg(m_CanID,m_CanData);
        return true;
    }
}

void CanBus::StopCan()
{
    QString strCmd;
    strCmd = QString("canconfig %1 stop").arg(m_CanPort);
    system(strCmd.toLatin1().data());
    close(m_Soc);
    m_bOpen = false;
}

void CanBus::GetErrorFrameInfo(uint nID, QByteArray &data, int dlc)
{
    bool unspecified;
    int c = nID& CAN_EFF_FLAG;
    //qDebug()<<c <<QString::number(nID,16).toInt(0)<<nID;
    //qDebug()<<QString::number(nID,16).toInt(0) & CAN_SFF_MASK << n << nID;
    switch (c)
    {
    case CAN_ERR_TX_TIMEOUT:
        qDebug()<< "发送超时";
        break;
    case CAN_ERR_LOSTARB:
        qDebug()<< "总线仲裁错误";
        break;
    case CAN_ERR_CRTL:
        if (data.length()>0 && dlc >= 2)
        {
            switch (data.at(1))
            {
            case CAN_ERR_CRTL_RX_OVERFLOW:
                qDebug()<< "接收缓存溢出";
                break;
            case CAN_ERR_CRTL_TX_OVERFLOW:
                qDebug()<< "发送缓存溢出";
                break;
            case CAN_ERR_CRTL_RX_WARNING:
                qDebug()<< "接收报警";
                break;
            case CAN_ERR_CRTL_TX_WARNING:
                qDebug()<< "发送报警";
                break;
            case CAN_ERR_CRTL_RX_PASSIVE:
                qDebug()<< "接收被动错误";
                break;
            case CAN_ERR_CRTL_TX_PASSIVE:
                qDebug()<< "发送被动错误";
                break;
            default:
                unspecified = true;
                qDebug()<< "unspecified";
                break;
            }
        }
        else
        {
            unspecified = true;
            qDebug()<< "unspecified to ";
        }

        if (unspecified)
        {
            qDebug()<< "CAN控制器错误 ";
        }
        break;

    case CAN_ERR_PROT:
        if (data.length()>0 && dlc >= 4)
        {
            if (data.at(2))
            {
                switch (data.at(2))
                {
                case CAN_ERR_PROT_BIT:
                    qDebug()<< "位错误 ";
                    break;
                case CAN_ERR_PROT_FORM:
                    qDebug()<< "帧格式错误 ";
                    break;
                case CAN_ERR_PROT_STUFF:
                    qDebug()<< "位填充错误 ";
                    break;
                case CAN_ERR_PROT_BIT0:
                    qDebug()<< "(接收站)不能发送占有位(dominant bit) ";
                    break;
                case CAN_ERR_PROT_BIT1:
                    qDebug()<< "(发送站)不能发送空闲位(recessive bit) ";
                    break;
                case CAN_ERR_PROT_OVERLOAD:
                    qDebug()<< "总线超负荷 ";
                    break;
                case CAN_ERR_PROT_ACTIVE:
                    qDebug()<< "主动错误 ";
                    break;
                case CAN_ERR_PROT_TX:
                    qDebug()<< "传输错误 ";
                    break;
                default: unspecified = true;   break; /* unspecified */
                }
            }
            else
            {
                unspecified = true;
            }

            if (unspecified && data.at(3))
            {
                switch (data.at(3))
                {
                //case CAN_ERR_PROT_LOC_SOF:  l += sprintf(p + l, "");         break;
                default:
                    unspecified = true;
                    break; /* unspecified */
                }
            }
        }
        else
        {
            unspecified = true;
        }

        if (unspecified)
        {
            qDebug()<< "协议违反 ";
           // l += sprintf(p + l, "协议违反");
        }
        break;

    case CAN_ERR_TRX:
        qDebug()<< "CAN_ERR_TRX ";
        break;

    case CAN_ERR_ACK:
        qDebug()<< "应答错误 ";
        break;
    case CAN_ERR_BUSOFF:
         qDebug()<< "总线关闭 ";
        break;
    case CAN_ERR_BUSERROR:
        qDebug()<< "总线错误(可能溢出) ";
        break;
    case CAN_ERR_RESTARTED:
        qDebug()<< "CAN控制器重启";
        break;
    default:
        break;
    }

}

/********************************************************
 *@Action:   字节ID转换成无符号整型ID
 *@param:    字节ID
 *@Return:   返回无符号整形ID
 *@Author:
 *@Date:      2016-7-3
 *@Vesion:    0.1
 ********************************************************/
quint32 CanBus::ByteToUIntID(QByteArray can_id)
{
    quint32 frameID = can_id.toHex().toUInt(0,16);
    return  frameID;
}

/********************************************************
 *@Action:    无符号整型ID转换成字节ID
 *@param:     无符号整型ID
 *@Return:    返回字节ID
 *@Author:
 *@Date:      2016-7-3
 *@Vesion:    0.1
 ********************************************************/
QByteArray CanBus::UIntIDToByte(quint32 can_id)
{
    //can_id=65827, hex=10123
    QByteArray byteID;
    byteID[0] = (quint8)( (can_id >> 24)&0xFF );
    byteID[1] = (quint8)( (can_id >> 16)&0xFF );
    byteID[2] = (quint8)( (can_id >> 8)&0xFF );
    byteID[3] = (quint8)(  can_id  & 0xFF );
    //qDebug()<<"hex0:"<<(quint8)byteID.at(0)<<"hex1:"<<(quint8)byteID.at(1)<<"hex2:"<<(quint8)byteID.at(2)<<"hex3:"<<(quint8)byteID.at(3);
    return byteID;
}


