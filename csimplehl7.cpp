#include "csimplehl7.h"

CSimpleHL7::CSimpleHL7()
{
}

/********************************************************
 *@Name:        WriteHL7Msg
 *@Author:      HuaT
 *@Description: 转换数据为HL7格式，并发送到指定串口
 *@Param1:      串口对象
 *@Param2:      病人ID
 *@Param3:      病人姓名
 *@Param4:      病人年龄
 *@Param5:      病人性别
 *@Param6:      测试项目
 *@Param7:      检测结果
 *@Param8:      检测时间
 *@Param9:      事件类型
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-2-20
********************************************************/
void CSimpleHL7::WriteHL7Msg(QextSerialPort *SerialHL7,
                             QString strNumberID, QString strName,
                             QString strAge, QString strSex, QString strItem,
                             QString strResult, QString strUnit,
                             QString strCheckTime, QString strEventType)
{
    QByteArray byteResult;
    QString strMSH,strPID,strOBX;
    byteResult.append(0x0B);
    //MSH
    QDateTime currentTime;
    QString strCurrentTime;
    currentTime = QDateTime::currentDateTime();
    strCurrentTime = currentTime.toString("yyyyMMddhhmmss");
    strMSH = QString("MSH|^~\&|IMPORVE|QT200|%1|||%2||P|2.3.1|||ASCII||").arg(strEventType).arg(strCurrentTime);
    byteResult.append(strMSH);
    //MSH结果
    byteResult.append(0x0D);
    //PID
    strPID = QString("PID|1|%1||||%2|%3||%4||||").arg(strNumberID).arg(strName).arg(strAge).arg(strSex);
    byteResult.append(m_Settings.Utf8ToGbk(strPID));
    //PID结束
    byteResult.append(0x0D);
    //OBX
    strOBX = QString("OBX|1|||%1|%2|%3|%4||||F|").arg(strItem).arg(strResult).arg(strUnit).arg(strCheckTime);
    byteResult.append(strOBX);
    //OBX结束
    byteResult.append(0x1C);
    byteResult.append(0x0D);
    SerialHL7->write(byteResult);
}
