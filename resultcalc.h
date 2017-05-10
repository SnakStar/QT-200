#ifndef RESULTCALC_H
#define RESULTCALC_H

#include"CalcDef.h"
#include<qmath.h>
#include<QByteArray>
#include<QString>
#include<QStringList>
#include<QDebug>

struct ResultDataInfo;


class ResultCalc
{
public:
    ResultCalc();
    void calculateResult(ResultDataInfo& datainfo,unsigned int *scanData1,unsigned int *scanData2,unsigned char *pRecord,unsigned char *pIdmessgae);
    QString calculateResult2(ResultDataInfo& datainfo,unsigned int *scanData1,unsigned int *scanData2,unsigned char *pRecord,unsigned char *pIdmessgae);
    void low_passfilter(unsigned int *scanData);
    void GetMaxResultFromIDMessage(long *MaxResult,unsigned char No,unsigned char *IDMessage);
    void GetMaxResultFromIDMessage(float* MinResult,float *MaxResult,unsigned char No,unsigned char *IDMessage);
    QString ConvetIDCardToStr(QByteArray byteIDCardData);
    unsigned char* ConvetIDCardToChar(QString IDCardData);
    unsigned char* ConvetIDCardToChar(QByteArray byteIDCardData);
    float SelectFormulaCalc(int nFormulaType,float fValue,float fParam1,float fParam2,float fParam3,float fParam4,float fParam5);
    QString ProcessDecimalPointPricision(QString strValue, quint8 nPrecision);
private:
    unsigned int m_ScanDataSize;
};

#endif // RESULTCALC_H
