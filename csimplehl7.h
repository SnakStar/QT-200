#ifndef CSIMPLEHL7_H
#define CSIMPLEHL7_H
#include<QDateTime>
#include "qextserial/qextserialport.h"
#include"cutilsettings.h"

class CSimpleHL7
{
public:
    CSimpleHL7();
public:
    void WriteHL7Msg(QextSerialPort* SerialHL7,QString strNumberID,
                     QString strName,QString strAge,QString strSex,
                     QString strItem,QString strResult,QString strUnit,
                     QString strCheckTime,QString strEventType);
    void ParseHL7Msg();
private:
    bool m_bTest;
    CUtilSettings m_Settings;
};

#endif // CSIMPLEHL7_H
