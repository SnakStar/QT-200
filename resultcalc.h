#ifndef RESULTCALC_H
#define RESULTCALC_H


#include"CalcDef.h"
#include<qmath.h>
#include<QByteArray>
#include<QString>
#include<QStringList>
#include<QDebug>

class ResultCalc
{
public:
    ResultCalc();
    void calculateResult(unsigned int *scanData1,unsigned int *scanData2,unsigned char *pRecord,unsigned char *pIdmessgae);
    void low_passfilter(unsigned int *scanData);
    void GetMaxResultFromIDMessage(long *MaxResult,unsigned char No,unsigned char *IDMessage);
    QString ConvetIDCardToStr(QByteArray byteIDCardData);
    unsigned char* ConvetIDCardToChar(QString IDCardData);
private:
    unsigned int m_ScanDataSize;
};

#endif // RESULTCALC_H
