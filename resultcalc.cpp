#include "resultcalc.h"
#include"testwindow.h"


ResultCalc::ResultCalc()
{
    m_ScanDataSize = SCAN_DATA_SIZE1;
}


/********************
函数名称： calculateResult
功    能：处理扫描信号
参    数： scanData1，scanData2，*pRecord
返 回 值：无，计算结果通过*pRecord传递
****************************/
void ResultCalc::calculateResult(ResultDataInfo& datainfo,unsigned int *scanData1,unsigned int *scanData2,unsigned char *pRecord,unsigned char *pIdmessgae)
{
    unsigned int i;
    long temp1,temp2;
    unsigned int p[4]={0,0,0,0};
    long value,value1,value2;
    unsigned int Threshold1,Threshold2;
    long maxresult,minresult;
    long  area1,area2;
    //long Threshold = 0;
    unsigned char tempindex[7]={0};//存放索引信息
    unsigned int TempMax1,TempMax2;
    unsigned int MaxPosition[2]; //存放波峰位置
    unsigned int index; //记录当前的记录个数

    long Tempbase1 = 0,Tempbase2 = 0;
    long Tempresult;
    long bit5 = 10000;

    long   AdjustPoint[10];// ={1,2,3,4,5,6,7,8,9,10};   //校准曲线10点
    long   Adjust_a[9];//={1,2,3,4,5,6,7,8,9};      //校准曲线斜率9个
    long   Adjust_b[9];//={0,0,0,0,0,0,0,0,0};		//校准曲线截距9个

    unsigned char  PeakHighValue ;//= 50;	//质控峰最小高度值（相对基线）
    unsigned char  ResultLength,TSignallength,CSignallength;

    TSignallength = *(pIdmessgae+147);   //检测峰积分长度
    datainfo.m_RawTestInfo.m_nCheckIntegralBreadt = TSignallength;
    CSignallength = *(pIdmessgae+ID_NO_CT_CIntegralLength2);//质控峰积分长度
    datainfo.m_RawTestInfo.m_nQCIntegralBreadth = CSignallength;
    if(0 == CSignallength || CSignallength > MAX_CSignallength || *(pIdmessgae+ID_NO_CALNUM) > MAX_CALNUM)//旧的ID卡
    {
        CSignallength = 100;        //旧卡的质控峰积分长度都是100
    }

    //计算基准值
    //计算检测卡质控峰所在位置前40个数值的平均值作为基准值
    for(i=0;i<(m_ScanDataSize/8);i++)   //  求基准值1
    {
        Tempbase1 = Tempbase1 +scanData1[i];
    }
    Tempbase1 = Tempbase1/(m_ScanDataSize/8);

    //Result_index = Tempbase1; //for test
    for(i=280;i<(m_ScanDataSize-1);i++)   //  求基准值2
    {
        Tempbase2 = Tempbase2 +scanData2[i];
    }
    Tempbase2 = Tempbase2/(m_ScanDataSize/8);

    //Result_index = Tempbase2;//for test
    //寻找最大值位置
    TempMax1 = scanData1[20];
    MaxPosition[0] = 20;
    for(i=20;i<150;i++)
    {
        if((scanData1[i]>TempMax1))
        {
            TempMax1 = scanData1[i];
            MaxPosition[0]=i;
        }

    }
    //Result_index = TempMax1;
    //Result_index = MaxPosition[0];
    TempMax2 = scanData2[320 -20];
    MaxPosition[1] = 320 - 20;
    for(i=(320-20);i>170;i--)
    {
        if(scanData2[i] > TempMax2)
        {TempMax2 = scanData2[i];
            MaxPosition[1]=i;
        }
    }
    //Result_index =  MaxPosition[1];
    //Result_index = TempMax2;
    PeakHighValue = *(pIdmessgae+109);
    datainfo.m_RawTestInfo.m_nQCMinHeightValue = PeakHighValue;
    for (i=0;i<4;i++)
    {
        p[i]=0;
    }
    if( (TempMax1-Tempbase1)< PeakHighValue || (TempMax2-Tempbase2)< PeakHighValue)   //峰值过低
    {
        //// 生成空(零)记录
        for(i=0;i<RECORDLENTH;i++)
        {
            *pRecord++ = 0;
        }
    }
    else
    {

        Threshold1 =  (TempMax1 - Tempbase1)/2+Tempbase1;
        Threshold2 =  (TempMax2 - Tempbase2)/2+Tempbase2;
        /**///检测最大值 是否为峰值位置
        for(i=MaxPosition[0]-CSignallength/2;i<(MaxPosition[0]+CSignallength/2);i++)
        {
            if((scanData1[i-1]<Threshold1)&&(scanData1[i]>=Threshold1))
            {
                p[0]=i;
            }

            if((scanData1[i-1]>=Threshold1)&&(scanData1[i]<Threshold1))
            {
                p[1]=i;
            }
            if( (p[0] != 0) && (p[1]!=0) && (p[0]<p[1]) ){
                break;
            }
        }
        for(i=MaxPosition[1]-CSignallength/2;i<(MaxPosition[1]+CSignallength/2);i++)
        {
            if((scanData2[i-1]<Threshold2)&&(scanData2[i]>=Threshold2))
            {
                p[2]=i;
            }
            if((scanData2[i-1]>=Threshold2)&&(scanData2[i]<Threshold2))
            {
                p[3]=i;
            }
            if( (p[2] != 0) && (p[3]!=0) && (p[2]<p[3]) ){
                break;
            }
        }

        quint32 nMaxValue = scanData1[MaxPosition[0]];
        quint32 nRightValue1 = scanData1[MaxPosition[0]]+1;
        quint32 nRightValue2 = scanData1[MaxPosition[0]]+2;
        //  如果 最大值在 边上 且 不是峰 ，则缩减范围 寻找峰值
        if( ((nMaxValue < nRightValue1)&&(nRightValue1<nRightValue2)) && (MaxPosition[0]>140 || MaxPosition[1]<180))
        {
            for (i=0;i<4;i++)
            {
                p[i]=0;
            }
            TempMax1 = scanData1[20];
            MaxPosition[0] = 20;
            for(i=20;i<120;i++)
            {
                if((scanData1[i]>TempMax1))
                {
                    TempMax1 = scanData1[i];
                    MaxPosition[0]=i;
                }
            }
            TempMax2 = scanData2[320 -20];
            MaxPosition[1] = 320 - 20;
            for(i=(320-20);i>200;i--)
            {
                if(scanData2[i] > TempMax2)
                {
                    TempMax2 = scanData2[i];
                    MaxPosition[1]=i;
                }
            }
            Threshold1 =  (TempMax1 - Tempbase1)/2+Tempbase1;
            Threshold2 =  (TempMax2 - Tempbase2)/2+Tempbase2;
            /**///检测最大值 是否为峰值位置
            for(i=MaxPosition[0]-CSignallength/2-10;i<(MaxPosition[0]+CSignallength/2+10);i++)
            {
                if((scanData1[i-1]<Threshold1)&&(scanData1[i]>=Threshold1))
                {
                    p[0]=i;
                }

                if((scanData1[i-1]>=Threshold1)&&(scanData1[i]<Threshold1))
                {
                    p[1]=i;
                }
                if( (p[0] != 0) && (p[1]!=0) && (p[0]<p[1]) ){
                    break;
                }
            }
            for(i=MaxPosition[1]-CSignallength/2-10;i<(MaxPosition[1]+CSignallength/2+10);i++)
            {
                if((scanData2[i-1]<Threshold2)&&(scanData2[i]>=Threshold2))
                {
                    p[2]=i;
                }
                if((scanData2[i-1]>=Threshold2)&&(scanData2[i]<Threshold2))
                {
                    p[3]=i;
                }
                if( (p[2] != 0) && (p[3]!=0) && (p[2]<p[3]) ){
                    break;
                }
            }
            // MaxPosition[0] = (p[1]+p[0])/2;
            //  MaxPosition[1] = (p[3]+p[2])/2;
        }

        if((p[0]==0 || p[1]==0) && (MaxPosition[0] > CSignallength/2))
        {
            p[0] = MaxPosition[0]/2;
            p[1] = MaxPosition[0]/2;
        }

        if((p[2]==0 || p[3]==0) && (MaxPosition[1] > CSignallength/2+TSignallength))
        {
            p[2] = MaxPosition[1]/2;
            p[3] = MaxPosition[1]/2;
        }

        if(p[0]==0|| p[1]==0|| p[2]==0|| p[3]==0 || (MaxPosition[0]>150 && MaxPosition[1]<170))
        {
            // 生成空(零)记录
            for(i=0;i<RECORDLENTH;i++)
            {
                *pRecord++ = 0;
            }
        }
        else
        {
            MaxPosition[0] = (p[0]+p[1])/2;
            MaxPosition[1] = (p[2]+p[3])/2;
            //记录波峰波谷位置，to PC Draw;
            datainfo.m_RawTestInfo.m_nCrestPos1 = MaxPosition[0];
            datainfo.m_RawTestInfo.m_nTroughPosLeft1 = MaxPosition[0]-CSignallength/2;
            datainfo.m_RawTestInfo.m_nTroughPosRight1 = MaxPosition[0]+CSignallength/2;
            datainfo.m_RawTestInfo.m_nCrestPos2 = MaxPosition[1];
            datainfo.m_RawTestInfo.m_nTroughPosLeft2 = MaxPosition[1]-CSignallength/2;
            datainfo.m_RawTestInfo.m_nTroughPosRight2 = MaxPosition[1]+CSignallength/2;

            //计算第一次的扫描数据的 面积积分
            area1 =0; //面积一
            for(i=MaxPosition[0]-CSignallength/2;i<MaxPosition[0]+CSignallength/2;i++)
            {
                if(i< m_ScanDataSize)
                {
                    area1 =area1 + scanData1[i];
                }
            }

            temp1 = scanData1[MaxPosition[0]-CSignallength/2];
            temp2 = scanData1[MaxPosition[0]+CSignallength/2];
            area1 = area1-(temp1+temp2)*CSignallength/2;
            datainfo.m_RawTestInfo.m_nTest1Area1 = area1;
            area2 =0;//面积二
            for(i=MaxPosition[0]+CSignallength/2;i<MaxPosition[0]+CSignallength/2+TSignallength;i++)
            {
                if(i< m_ScanDataSize)
                {
                    area2 =area2 + scanData1[i];
                }
            }

            temp1 = scanData1[MaxPosition[0]+CSignallength/2];
            temp2 = scanData1[MaxPosition[0]+CSignallength/2+TSignallength];
            area2 = area2-(temp1+temp2)*TSignallength/2;
            datainfo.m_RawTestInfo.m_nTest1Area2 = area2;
            if(area2<0){
                area2 = 0;
                datainfo.m_RawTestInfo.m_nTest1Area2 = area2;
            }
            if(0 == area1){
                value1 = 0;
                datainfo.m_RawTestInfo.m_nTest1Ratio = 0;
            }else{
                value1 = (area2*bit5)/area1; //比值1
                datainfo.m_RawTestInfo.m_nTest1Ratio = value1;
            }
            //计算第二次的扫描数据的 面积积分
            area1 =0; //面积一
            for(i=MaxPosition[1]+CSignallength/2;i>MaxPosition[1]-CSignallength/2;i--)
            {
                if(i< m_ScanDataSize)
                {
                    area1 =area1 + scanData2[i];
                }
            }
            temp1 = scanData2[MaxPosition[1]+CSignallength/2];
            temp2 = scanData2[MaxPosition[1]-CSignallength/2];
            area1 = area1-(temp1+temp2)*CSignallength/2;
            datainfo.m_RawTestInfo.m_nTest2Area1 = area1;
            area2 =0;//temp1 = 0;temp2 = 0;//面积二
            for(i=MaxPosition[1]-CSignallength/2;i>MaxPosition[1]-CSignallength/2-TSignallength;i--)
            {
                if(i< m_ScanDataSize)
                {
                    area2 = area2 + scanData2[i];
                }
            }
            temp1 = scanData2[MaxPosition[1]-CSignallength/2];
            temp2 = scanData2[MaxPosition[1]-CSignallength/2-TSignallength];

            area2 = area2-(temp1 + temp2)*TSignallength/2;
            datainfo.m_RawTestInfo.m_nTest2Area2 = area2;
            if(area2<0){
                area2 = 0;
                datainfo.m_RawTestInfo.m_nTest2Area2 = 0;
            }
            if(0 == area1){
                value2 = 0;
                datainfo.m_RawTestInfo.m_nTest2Ratio = 0;
            }else{
                value2 = (area2*bit5)/area1; //比值 2
                datainfo.m_RawTestInfo.m_nTest2Ratio = value2;
            }

            if( MaxPosition[0]>150 ||  MaxPosition[1]<170)  //提防 信号位置偏出扫描区域
            {
                if(MaxPosition[0]>150){
                    value = value2;
                }
                else{
                    value = value1;
                }
            }
            else
            {
                value = (value1 + value2)/2;   // 平均值
            }

            //Result_index = value;

            for(i=0;i<30;) //获取曲线校准点
            {
                AdjustPoint[i/3] = *(pIdmessgae+14+i)*bit5 + *(pIdmessgae+15+i)*100+ *(pIdmessgae+16+i);
                //Result_index = AdjustPoint[i/3];
                i=i+3;
            }
            for(i=0;i<36;) //获取校准参数A
            {
                Adjust_a[i/4] = *(pIdmessgae+111+i);
                Adjust_a[i/4] =Adjust_a[i/4]<<8;
                Adjust_a[i/4] += *(pIdmessgae+110+i) ;
                Adjust_a[i/4] = Adjust_a[i/4]*bit5+ *(pIdmessgae+112+i)*100 + *(pIdmessgae+113+i) ;
                //Result_index = Adjust_a[i/4];
                i=i+4;
            }
            for(i=0;i<36;)//获取校准参数B
            {
                Adjust_b[i/4] = *(pIdmessgae+46+i);
                Adjust_b[i/4] = Adjust_b[i/4]<<8;
                Adjust_b[i/4] += *(pIdmessgae+45+i) ;
                Adjust_b[i/4] =  Adjust_b[i/4]*100 + *(pIdmessgae+47+i);
                //Result_index = Adjust_b[i/4];
                if(*(pIdmessgae+44+i) == 0)
                    Adjust_b[i/4] = -Adjust_b[i/4];

                i=i+4;
            }

            GetMaxResultFromIDMessage(&maxresult,NO_TEST_NAME1,pIdmessgae);

            minresult = *(pIdmessgae+102);
            minresult = minresult<<8;
            minresult += *(pIdmessgae+101);
            minresult = minresult*100 + *(pIdmessgae+103);
            //Result_index = minresult;

            if(value<=AdjustPoint[0])
            {Tempresult = 0;}
            else if(AdjustPoint[0]<value && value<=AdjustPoint[1])
            {
                Tempresult =(value/100.0)*(Adjust_a[0]/100.0)/100;
                Tempresult=Tempresult+Adjust_b[0];
                /*Tempresult = (value/100)*(Adjust_a[0]/100)/100+Adjust_b[0];*/
            }

            else if(AdjustPoint[1]<value && value<=AdjustPoint[2])
            {
                Tempresult = (value/100.0)*(Adjust_a[1]/100.0)/100+Adjust_b[1];
                // Tempresult = (value/10)*(Adjust_a[1]/10)/bit5+Adjust_b[1];
                // Tempresult = value*Adjust_a[1]/bit5+Adjust_b[1];
            }

            else if(AdjustPoint[2]<value && value<=AdjustPoint[3])
            {
                Tempresult = (value/100.0)*(Adjust_a[2]/100.0)/100+Adjust_b[2];
                //Tempresult = (value/10)*(Adjust_a[2]/10)/bit5+Adjust_b[2];
                // Tempresult = value*Adjust_a[2]/bit5+Adjust_b[2];
            }

            else if(AdjustPoint[3]<value && value<=AdjustPoint[4])
            {
                Tempresult = (value/100.0)*(Adjust_a[3]/100.0)/100+Adjust_b[3];
                // Tempresult = (value/10)*(Adjust_a[3]/10)/bit5+Adjust_b[3];
                //Tempresult = value*Adjust_a[3]/bit5+Adjust_b[3];
            }

            else if(AdjustPoint[4]<value && value<=AdjustPoint[5])
            {
                Tempresult = (value/100.0)*(Adjust_a[4]/100.0)/100+Adjust_b[4];
                //Tempresult = (value/10)*(Adjust_a[4]/10)/bit5+Adjust_b[4];
                //Tempresult = value*Adjust_a[4]/bit5+Adjust_b[4];
            }

            else if(AdjustPoint[5]<value && value<=AdjustPoint[6])
            {
                Tempresult = (value/100.0)*(Adjust_a[5]/100.0)/100+Adjust_b[5];
                //Tempresult = (value/10)*(Adjust_a[5]/10)/bit5+Adjust_b[5];
                // Tempresult = value*Adjust_a[5]/bit5+Adjust_b[5];
            }

            else if(AdjustPoint[6]<value && value<=AdjustPoint[7])
            {
                Tempresult = (value/100.0)*(Adjust_a[6]/100.0)/100+Adjust_b[6];
                // Tempresult = (value/10)*(Adjust_a[6]/10)/bit5+Adjust_b[6];
                //Tempresult = value*Adjust_a[6]/bit5+Adjust_b[6];
            }

            else if(AdjustPoint[7]<value && value<=AdjustPoint[8])
            {
                Tempresult = (value/100.0)*(Adjust_a[7]/100.0)/100+Adjust_b[7];
                // Tempresult = (value/10)*(Adjust_a[7]/10)/bit5+Adjust_b[7];
                // Tempresult = value*Adjust_a[7]/bit5+Adjust_b[7];
            }

            else if(AdjustPoint[8]<value && value<AdjustPoint[9])
            {
                Tempresult = (value/100.0)*(Adjust_a[8]/100.0)/100+Adjust_b[8];
                //Tempresult = (value/10)*(Adjust_a[8]/10)/bit5+Adjust_b[8];
                // Tempresult = value*Adjust_a[8]/bit5+Adjust_b[8];
            }

            else if(value>AdjustPoint[9])
                // {Tempresult = bit5*2;}
                Tempresult = maxresult;

            //Result_index = Tempresult; //9.6 10.6

            // 生成记录
            ResultLength = *(pIdmessgae+100); //????

            //Result_index = ReadIndex(tempindex); //读取当前记录编号及索引信息
            //Delayms(500);
            index = (tempindex[0]<<8) + tempindex[1];
            //Result_index = index;
            if(index == MAXRECODENUM+1)
            {index = 1;}
            else
            { index = index+1;}

            //Result_index = index;

            tempindex[0] = index>>8;   //更新记录编号信息
            tempindex[1] = (unsigned char ) index&0x00FF;
            // Result_index =  index>>8;   //更新记录编号信息
            // Result_index = (unsigned char ) index&0x00FF;

            index = (tempindex[2]<<8) + tempindex[3]; //当前结果索引号更新
            index = index+1;
            //Result_index = index;
            if(index == (MAXRECODENUM+1))
            {
                index = 1;
            }
            tempindex[2] =  index>>8;//更新索引信息
            tempindex[3] = (unsigned char ) index & 0x00FF;
            *pRecord = index>>8;   //存放记录编号
            *(pRecord+1) = (unsigned char ) index&0x00FF;

            index = (tempindex[4]<<8) + tempindex[5]; //当前记录数目
            if(index != MAXRECODENUM)
            {
                index++;
                tempindex[4] =  index>>8;
                tempindex[5] = (unsigned char ) index&0x00FF;
            }

            //Result_index = WriteIndex(tempindex); //更新索引信息
            //Delayms(500);

            //DS1302_GetData(ReadingData); // 获取当前时间
            //for(i=0;i<7;i++)
            //{*(pRecord+2+i) = ReadingData[i];}  //存放测试时间


            //Result_index = Tempresult;

            switch(ResultLength)
            {
            case 0:
                //qDebug()<<"Tempresult:"<<Tempresult;
                *(pRecord+9) = 0;  //最大最小值标记 1最小值，2最大值
                if(Tempresult<=minresult)
                {Tempresult = minresult;
                    *(pRecord+9) = 1;}
                if(Tempresult>=maxresult)
                {Tempresult = maxresult;
                    *(pRecord+9) = 2;}
                *(pRecord+10) = 0x30; //小数点位数
                *(pRecord+11) = 0x30; //小数值
                *(pRecord+12) =  (unsigned char)((Tempresult/100)>>8); //高8位
                *(pRecord+13) = (unsigned char ) (Tempresult/100)&0x00FF;//低8位
                //qDebug()<<QString::number((quint8)pRecord[9]).toInt()<<"maxresult:"<<maxresult;
                //qDebug()<<"Tempresult:"<<Tempresult;
                //qDebug()<<QString::number((quint8)pRecord[10],16).toInt();
                break;
            case 1:
                *(pRecord+9) = 0;  //最大最小值标记 1最小值，2最大值
                if(Tempresult<=minresult)
                {Tempresult = minresult;
                    *(pRecord+9) = 1;}
                if(Tempresult>=maxresult)
                {Tempresult = maxresult;
                    *(pRecord+9) = 2;}
                *(pRecord+10) = 0x31; //小数点位数
                *(pRecord+11) = (Tempresult%100)/10; //小数值
                //Result_index  = *(pRecord+11);
                *(pRecord+12) =  (unsigned char)((Tempresult/100)>>8); //高8位
                //Result_index  = *(pRecord+12);
                *(pRecord+13) = (unsigned char ) (Tempresult/100)&0x00FF;//低8位
                //Result_index  = *(pRecord+13);
                break;
            default:
                *(pRecord+9) = 0;  //最大最小值标记 1最小值，2最大值
                if(Tempresult<=minresult)
                {Tempresult = minresult;
                    *(pRecord+9) = 1;}
                if(Tempresult>=maxresult)
                {Tempresult = maxresult;
                    *(pRecord+9) = 2;}
                *(pRecord+10) = 0x32; //小数点位数
                *(pRecord+11) = (Tempresult%100); //小数值
                *(pRecord+12) =  (unsigned char)((Tempresult/100)>>8); //高8位
                *(pRecord+13) = (unsigned char ) (Tempresult/100)&0x00FF;//低8位
                break;
            }

            //测试单位
            //GetTestUnitFromIDMessage(pRecord+RECORD_NO_TEST_UNIT,NO_TEST_NAME1,pIdmessgae);

            //测试名称
            //GetTestNameFromIDMessage(pRecord+RECORD_NO_TEST_NAME,NO_TEST_NAME1,pIdmessgae);
        }
    }

    //if(pRecord[0]!=0 || pRecord[1]!=0)//记录不为空
    //{
    //     index = (tempindex[0]<<8) + tempindex[1]+2; //获取当前索引号
    //     Result_index = WriteRecord(index,pRecord);        //存储记录
    //}

    //ResultNumToShow = 1;             //单联卡只有一个结果需要显示
    //NowShowResultNum = 0;
    //PrintResultMark[0] = 0;          //已打印标志清零
}

QString ResultCalc::calculateResult2(ResultDataInfo& datainfo,unsigned int *scanData1,unsigned int *scanData2,unsigned char *pRecord,unsigned char *pIdmessgae)
{
    /*QByteArray byteScan1,byteScan2;
    for(int n=0; n<320; n++){
        byteScan1.append((quint8)scanData1[n]);
        byteScan2.append((quint8)scanData2[n]);
        qDebug()<<n<<"   "<<"1:"<<scanData1[n]<<"    2:"<<scanData2[n];
    }*/
    //测试日志信息
    QString strTestLogInfo = "测试信息: ";
    QString strTempLogInfo;
    unsigned int i;
    long temp1,temp2;
    unsigned int p[4]={0,0,0,0};
    float value,value1,value2;
    unsigned int Threshold1,Threshold2;
    unsigned int TempMaxPos1,TempMaxPos2;
    float maxresult,minresult;
    long  area1,area2;
    //long Threshold = 0;
    unsigned int TempMax1,TempMax2;
    //存放波峰位置
    unsigned int MaxPosition[4];


    long Tempbase1 = 0,Tempbase2 = 0;
    float Tempresult;
    long bit5 = 10000;

    double AdjustPoint[15];// ={1,2,3,4,5,6,7,8,9,10};   //校准曲线10点
    long   Adjust_Formula[15];//={1,2,3,4,5,6,7,8,9};    //校准曲线公式
    double Adjust_Param1[15];//={0,0,0,0,0,0,0,0,0};     //校准曲线参数1
    double Adjust_Param2[15];//={0,0,0,0,0,0,0,0,0};	 //校准曲线参数2
    double Adjust_Param3[15];//={0,0,0,0,0,0,0,0,0};	 //校准曲线参数3
    double Adjust_Param4[15];//={0,0,0,0,0,0,0,0,0};	 //校准曲线参数4
    double Adjust_Param5[15];//={0,0,0,0,0,0,0,0,0};	 //校准曲线参数5

    unsigned char  PeakHighValue ;//= 50;	//质控峰最小高度值（相对基线）
    unsigned char  TSignallength,CSignallength;

    TSignallength = *(pIdmessgae+77);   //检测峰积分长度
    datainfo.m_RawTestInfo.m_nCheckIntegralBreadt = TSignallength;
    CSignallength = *(pIdmessgae+35);   //质控峰积分长度
    datainfo.m_RawTestInfo.m_nQCIntegralBreadth = CSignallength;
    //TSignallength = 120;
    //CSignallength = 80;
    if(0 == CSignallength || CSignallength > MAX_CSignallength || *(pIdmessgae+ID_NO_CALNUM) > MAX_CALNUM)//旧的ID卡
    {
        CSignallength = 100;        //旧卡的质控峰积分长度都是100
    }

    //计算基准值
    //计算检测卡质控峰所在位置前40个数值的平均值作为基准值
    for(i=0;i<(m_ScanDataSize/8);i++)   //  求基准值1
    {
        Tempbase1 = Tempbase1 +scanData1[i];
    }
    Tempbase1 = Tempbase1/(m_ScanDataSize/8);

    //Result_index = Tempbase1; //for test
    for(i=280;i<(m_ScanDataSize-1);i++)   //  求基准值2
    {
        Tempbase2 = Tempbase2 +scanData2[i];
    }
    Tempbase2 = Tempbase2/(m_ScanDataSize/8);

    //Result_index = Tempbase2;//for test
    //寻找最大值位置
    TempMax1 = scanData1[20];
    //MaxPosition[0] = 20;
    for(i=0;i<320;i++)
    {
        if((scanData1[i]>TempMax1))
        {
            TempMax1 = scanData1[i];
            TempMaxPos1=i;
        }

    }
    //Result_index = TempMax1;
    //Result_index = MaxPosition[0];
    TempMax2 = scanData2[320 -20];
    //MaxPosition[2] = 320 - 20;
    for(i=0;i<320;i++)
    {
        if(scanData2[i] > TempMax2)
        {
            TempMax2 = scanData2[i];
            TempMaxPos2=i;
        }
    }
    //Result_index =  MaxPosition[1];
    //Result_index = TempMax2;
    PeakHighValue = *(pIdmessgae+37);
    datainfo.m_RawTestInfo.m_nQCMinHeightValue = PeakHighValue;
    for (i=0;i<4;i++)
    {
        p[i]=0;
        MaxPosition[i]=0;
    }
    if( (TempMax1-Tempbase1)< PeakHighValue || (TempMax2-Tempbase2)< PeakHighValue)   //峰值过低
    {
        //// 生成空(零)记录
        for(i=0;i<RECORDLENTH;i++)
        {
            *pRecord++ = 0;
        }
        return "0";
    }
    else
    {

        Threshold1 =  (TempMax1 - Tempbase1)/2+Tempbase1;
        Threshold2 =  (TempMax2 - Tempbase2)/2+Tempbase2;


        if(TempMaxPos1>150){
            TempMax1 = scanData1[TempMaxPos1 - TSignallength/2];
            for(int n=TempMaxPos1 - TSignallength/2; n>TempMaxPos1 - TSignallength/2 - CSignallength; n--){
                if(TempMax1<scanData1[n]){
                    MaxPosition[0] = n;
                    TempMax1 = scanData1[n];
                }
            }
            MaxPosition[1] = TempMaxPos1;
        }else{
            TempMax1 = scanData1[TempMaxPos1 + CSignallength/2];
            for(int n=TempMaxPos1 + CSignallength/2; n<TempMaxPos1+CSignallength/2+TSignallength; n++){
                if(TempMax1<scanData1[n]){
                    MaxPosition[1] = n;
                    TempMax1 = scanData1[n];
                }
            }

            MaxPosition[0] = TempMaxPos1;
        }
        //判断右峰峰顶是否超出了范围内,如果超出则使用偏移的方法
        if(MaxPosition[1]<MaxPosition[0]+CSignallength/2+TSignallength/4 || MaxPosition[1]<MaxPosition[0]){
            MaxPosition[1] = MaxPosition[0]+CSignallength/2+TSignallength/2;
        }
        if(TempMaxPos2>150){
            TempMax2 = scanData2[TempMaxPos2 - CSignallength/2];
            for(int n=TempMaxPos2 - CSignallength/2; n>TempMaxPos2-CSignallength/2-TSignallength; n--){
                if(TempMax2<scanData2[n]){
                    MaxPosition[3] = n;
                    TempMax2 = scanData2[n];
                }
            }

            MaxPosition[2] = TempMaxPos2;
        }else{
            TempMax2 = scanData2[TempMaxPos2 + TSignallength/2];
            for(int n=TempMaxPos2 + TSignallength/2; n<TempMaxPos2+TSignallength/2+CSignallength; n++){
                if(TempMax2<scanData2[n]){
                    MaxPosition[2] = n;
                    TempMax2 = scanData2[n];
                }
            }

            MaxPosition[3] = TempMaxPos2;
        }
        //判断左峰峰顶是否超出了范围内,如果超出则使用偏移的方法
        if(MaxPosition[3]>MaxPosition[2]-CSignallength/2-TSignallength/4 || MaxPosition[3]>MaxPosition[2]){
            MaxPosition[3] = MaxPosition[2] - CSignallength/2 - TSignallength/2;
        }

        /*
        //位置1的峰。
        MaxPosition[0] = TempMaxPos1;

        //位置2的峰，
        MaxPosition[2] = TempMaxPos2;

        ///判断右边是不是一个峰。//
        for(i=MaxPosition[0]+CSignallength/2;i<(MaxPosition[0]+CSignallength/2)+TSignallength;i++)
        {
            if(i>320){
                break;
            }
            if((scanData1[i-1]<Threshold1)&&(scanData1[i]>=Threshold1))
            {
                p[0]=i;
            }

            if((scanData1[i-1]>=Threshold1)&&(scanData1[i]<Threshold1))
            {
                p[1]=i;
            }
            if ((p[0]!=0)&&(p[1]!=0)&&(p[0]<p[1]))
            {
                break;
            }
        }
        //判断左边是不是一个峰。
        for(i=MaxPosition[0]-CSignallength/2; i>(MaxPosition[0]-CSignallength/2-TSignallength);i--){
            if(i<0){
                break;
            }
            if((scanData1[i-1]<Threshold1)&&(scanData1[i]>=Threshold1))
            {
                p[2]=i;
            }

            if((scanData1[i-1]>=Threshold1)&&(scanData1[i]<Threshold1))
            {
                p[3]=i;
            }
            if ((p[2]!=0)&&(p[3]!=0)&&(p[2]<p[3]))
            {
                break;
            }
        }
        //如果右边有峰，可能2个峰都在大于160以内，所以maxposition[0]需要把减少的CSignallength/2偏移量加回来。
        if( (p[0] != 0) && (p[1] != 0) ){
            MaxPosition[0] =  MaxPosition[0];
        }else if( (p[2] != 0) && (p[3]!=0)){//左边有峰，移动到左边
            MaxPosition[0] = TempMaxPos1 - CSignallength/2-TSignallength/2;
        }
        for(int n=0; n<4; n++){
            p[n]=0;
        }

        //第二个波峰图形,左边是否有峰
        for(i=MaxPosition[1]-CSignallength/2;i>(MaxPosition[1]-CSignallength/2-TSignallength);i--)
        {
            if(i<0){
                break;
            }
            if((scanData2[i-1]<Threshold2)&&(scanData2[i]>=Threshold2))
            {
                p[2]=i;
            }
            if((scanData2[i-1]>=Threshold2)&&(scanData2[i]<Threshold2))
            {
                p[3]=i;
            }
            if ((p[2]!=0)&&(p[3]!=0)&&(p[2]<p[3]))
            {
                break;
            }
        }
        //查找右边是否有峰
        for(i=MaxPosition[1]+CSignallength/2;i<(MaxPosition[1]+CSignallength/2+TSignallength);i++)
        {
            if(i>320){
                break;
            }
            if((scanData2[i-1]<Threshold2)&&(scanData2[i]>=Threshold2))
            {
                p[0]=i;
            }
            if((scanData2[i-1]>=Threshold2)&&(scanData2[i]<Threshold2))
            {
                p[1]=i;
            }
            if ((p[0]!=0)&&(p[1]!=0)&&(p[0]<p[1]))
            {
                break;
            }
        }
        //如果左边有峰,则为需要寻找的右峰
        if( (p[2] != 0) && (p[3] != 0) ){
            MaxPosition[1] = TempMaxPos2;
        }else if( (p[0] != 0) && (p[1] != 0)){
            MaxPosition[1] = MaxPosition[1] + CSignallength/2 + TSignallength/2;
        }*/
        //strTempLogInfo = QString("p[0]:%1,p[1]:%2,p[2]:%3,p[3]:%4").arg(p[0]).arg(p[1]).arg(p[2]).arg(p[3]);
        //strTestLogInfo = strTestLogInfo + strTempLogInfo;




        if(MaxPosition[0] == 0)
        {
            // 生成空(零)记录
            for(i=0;i<RECORDLENTH;i++)
            {
                *pRecord++ = 0;
            }
            return 0;
        }
        else
        {
            //MaxPosition[0] = (p[0]+p[1])/2;
            //MaxPosition[1] = (p[2]+p[3])/2;
            strTempLogInfo = QString(" 波峰1:%1,波峰2:%2").arg(MaxPosition[0]).arg(MaxPosition[2]);
            strTestLogInfo = strTestLogInfo + strTempLogInfo;
            //记录波峰波谷位置，to PC Draw;
            datainfo.m_RawTestInfo.m_nCrestPos1 = MaxPosition[0];
            datainfo.m_RawTestInfo.m_nTroughPosLeft1 = MaxPosition[0]-CSignallength/2;
            datainfo.m_RawTestInfo.m_nTroughPosRight1 = MaxPosition[0]+CSignallength/2;
            datainfo.m_RawTestInfo.m_nCrestPos2 = MaxPosition[2];
            datainfo.m_RawTestInfo.m_nTroughPosLeft2 = MaxPosition[2]-CSignallength/2;
            datainfo.m_RawTestInfo.m_nTroughPosRight2 = MaxPosition[2]+CSignallength/2;

            //计算第一次的扫描数据的 面积积分
            area1 =0; //面积一
            for(i=MaxPosition[0]-CSignallength/2;i<MaxPosition[0]+CSignallength/2;i++)
            {
                if(i< m_ScanDataSize)
                {
                    area1 =area1 + scanData1[i];
                }
            }

            temp1 = scanData1[MaxPosition[0]-CSignallength/2];
            temp2 = scanData1[MaxPosition[0]+CSignallength/2];
            area1 = area1-(temp1+temp2)*CSignallength/2;
            if(area1<0){
                area1 = 0;
            }
            datainfo.m_RawTestInfo.m_nTest1Area1 = area1;
            area2 =0;//面积二
            for(i=MaxPosition[1]-TSignallength/2;i<MaxPosition[1]+TSignallength/2;i++)
            {
                if(i< m_ScanDataSize)
                {
                    area2 =area2 + scanData1[i];
                }
            }

            temp1 = scanData1[MaxPosition[1]-TSignallength/2];
            temp2 = scanData1[MaxPosition[1]+TSignallength/2];
            area2 = area2-(temp1+temp2)*TSignallength/2;
            datainfo.m_RawTestInfo.m_nTest1Area2 = area2;
            strTempLogInfo = QString(" [1][面积1:%1,面积2:%2]").arg(area1).arg(area2);
            strTestLogInfo = strTestLogInfo + strTempLogInfo;
            if(area2<0){
                area2 = 0;
                datainfo.m_RawTestInfo.m_nTest1Area2 = 0;
            }
            if(0 == area1){
                value1 = 0;
            }else{
                value1 = (area2*bit5)/area1; //比值1
            }
            datainfo.m_RawTestInfo.m_nTest1Ratio = value1;

            /*
            //计算第二次的扫描数据的 面积积分
            area1 =0; //面积一
            for(i=MaxPosition[2]+CSignallength/2;i>MaxPosition[2]-CSignallength/2;i--)
            {
                if(i< m_ScanDataSize)
                {
                    area1 =area1 + scanData2[i];
                }
            }
            temp1 = scanData2[MaxPosition[2]+CSignallength/2];
            temp2 = scanData2[MaxPosition[2]-CSignallength/2];
            area1 = area1-(temp1+temp2)*CSignallength/2;
            if(area1<0){
                area1 = 0;
            }
            datainfo.m_RawTestInfo.m_nTest2Area1 = area1;
            area2 =0;//temp1 = 0;temp2 = 0;//面积二
            for(i=MaxPosition[3]+TSignallength/2;i>MaxPosition[3]-TSignallength/2;i--)
            {
                if(i< m_ScanDataSize)
                {
                    area2 = area2 + scanData2[i];
                }
            }
            temp1 = scanData2[MaxPosition[3]-TSignallength/2];
            temp2 = scanData2[MaxPosition[3]+TSignallength/2];

            area2 = area2-(temp1 + temp2)*TSignallength/2;
            datainfo.m_RawTestInfo.m_nTest2Area2 = area2;
            strTempLogInfo = QString(" [2][面积1:%1,面积2:%2]").arg(area1).arg(area2);
            strTestLogInfo = strTestLogInfo + strTempLogInfo;
            if(area2<0){
                area2 = 0;
                datainfo.m_RawTestInfo.m_nTest2Area2 = 0;
            }
            if(0 == area1){
                value2 = 0;
            }else{
                value2 = (area2*bit5)/area1; //比值 2
            }
            datainfo.m_RawTestInfo.m_nTest2Ratio = value2;

            if( MaxPosition[0]>150 ||  MaxPosition[2]<130)  //提防 信号位置偏出扫描区域
            {
                if(MaxPosition[0]>150)
                {value = value2;}
                else
                {value = value1;}
                qDebug()<<"结果计算过程:信号位置偏出扫描区域!";
            }
            else
            {
                value = (value1 + value2)/20000;   // 平均值
            }*/
            value = (value1)/10000;
            strTempLogInfo = QString(" 检测结果:%1").arg(value);
            strTestLogInfo = strTestLogInfo + strTempLogInfo;
            qDebug()<<strTestLogInfo;
            strTestLogInfo.clear();


            //Result_index = value;
            /////////////////////////////////////开始进行公式套入////////////////////////////////////////
            QString strAdustPoint;
            QString strAdust_Param1;
            QString strAdust_Param2;
            QString strAdust_Param3;
            QString strAdust_Param4;
            QString strAdust_Param5;
            double  fTemp;
            for(i=0;i<170;i+=34) //获取曲线校准点
            {
                fTemp =  *(pIdmessgae+87+i)+(double)*(pIdmessgae+88+i)/100+(double)*(pIdmessgae+89+i)/10000;
                //校准点
                strAdustPoint = QString("%1").arg( fTemp );
                AdjustPoint[i/34] = strAdustPoint.toDouble() ;
                //公式
                Adjust_Formula[i/34] = *(pIdmessgae+90+i);
                //参数1
                fTemp = (*(pIdmessgae+92+i)*65536) + (*(pIdmessgae+93+i)*256) + *(pIdmessgae+94+i) + (double)*(pIdmessgae+95+i)/100 + (double)*(pIdmessgae+96+i)/10000;
                strAdust_Param1 = QString("%1%2").arg((char)*(pIdmessgae+91+i)).arg( fTemp );
                Adjust_Param1[i/34] = strAdust_Param1.toDouble();
                //参数2
                fTemp = (*(pIdmessgae+98+i)*65536) + (*(pIdmessgae+99+i)*256) + *(pIdmessgae+100+i) + (double)*(pIdmessgae+101+i)/100 + (double)*(pIdmessgae+102+i)/10000;
                strAdust_Param2 = QString("%1%2").arg((char)*(pIdmessgae+97+i)).arg( fTemp );
                Adjust_Param2[i/34] = strAdust_Param2.toDouble();
                //参数3
                fTemp = (*(pIdmessgae+104+i)*65536) + (*(pIdmessgae+105+i)*256) + *(pIdmessgae+106+i) + (double)*(pIdmessgae+107+i)/100 + (double)*(pIdmessgae+108+i)/10000;
                strAdust_Param3 = QString("%1%2").arg((char)*(pIdmessgae+103+i)).arg( fTemp );
                Adjust_Param3[i/34] = strAdust_Param3.toDouble();
                //参数4
                fTemp = (*(pIdmessgae+110+i)*65536) + (*(pIdmessgae+111+i)*256) + *(pIdmessgae+112+i) + (double)*(pIdmessgae+113+i)/100 + (double)*(pIdmessgae+114+i)/10000;
                strAdust_Param4 = QString("%1%2").arg((char)*(pIdmessgae+109+i)).arg( fTemp );
                Adjust_Param4[i/34] = strAdust_Param4.toDouble();
                //参数5
                fTemp = (*(pIdmessgae+116+i)*65536) + (*(pIdmessgae+117+i)*256) + *(pIdmessgae+118+i) + (double)*(pIdmessgae+119+i)/100 + (double)*(pIdmessgae+120+i)/10000;
                strAdust_Param5 = QString("%1%2").arg((char)*(pIdmessgae+115+i)).arg( fTemp );
                Adjust_Param5[i/34] = strAdust_Param5.toDouble();

                //Result_index = AdjustPoint[i/3];
            }

            GetMaxResultFromIDMessage(&minresult,&maxresult,NO_TEST_NAME1,pIdmessgae);

            //Result_index = minresult;
            QString strMinRatioValue;
            strMinRatioValue = QString("%1").arg( *(pIdmessgae+84) + (double)*(pIdmessgae+85)/100 + (double)*(pIdmessgae+86)/10000 );
            float fMinRatioValue = strMinRatioValue.toFloat();

            if(value <= fMinRatioValue)
            {Tempresult = 0;}
            else if(fMinRatioValue < value && value <=AdjustPoint[0])
            {
                int nFormulaType = Adjust_Formula[0];
                Tempresult = SelectFormulaCalc(nFormulaType,value,Adjust_Param1[0],
                                               Adjust_Param2[0],Adjust_Param3[0],
                                               Adjust_Param4[0],Adjust_Param5[0]);
            }
            else if(AdjustPoint[0]<value && value<=AdjustPoint[1])
            {
                int nFormulaType = Adjust_Formula[1];
                Tempresult = SelectFormulaCalc(nFormulaType,value,Adjust_Param1[1],
                                               Adjust_Param2[1],Adjust_Param3[1],
                                               Adjust_Param4[1],Adjust_Param5[1]);
            }

            else if(AdjustPoint[1]<value && value<=AdjustPoint[2])
            {
                int nFormulaType = Adjust_Formula[2];
                Tempresult = SelectFormulaCalc(nFormulaType,value,Adjust_Param1[2],
                                               Adjust_Param2[2],Adjust_Param3[2],
                                               Adjust_Param4[2],Adjust_Param5[2]);
            }

            else if(AdjustPoint[2]<value && value<=AdjustPoint[3])
            {
                int nFormulaType = Adjust_Formula[3];
                Tempresult = SelectFormulaCalc(nFormulaType,value,Adjust_Param1[3],
                                               Adjust_Param2[3],Adjust_Param3[3],
                                               Adjust_Param4[3],Adjust_Param5[3]);
            }

            else if(AdjustPoint[3]<value && value<=AdjustPoint[4])
            {
                int nFormulaType = Adjust_Formula[4];
                Tempresult = SelectFormulaCalc(nFormulaType,value,Adjust_Param1[4],
                                               Adjust_Param2[4],Adjust_Param3[4],
                                               Adjust_Param4[4],Adjust_Param5[4]);
            }
            else if(value>AdjustPoint[4]){
                Tempresult = maxresult;
            }

            //小数点精度
            int nPrecision = *(pIdmessgae+76);
            qDebug()<<Tempresult;
            QString strResult,strTempResult;
            strTempResult = ProcessDecimalPointPricision(QString::number(Tempresult),nPrecision);
            strResult = strTempResult;
            if(strTempResult.toFloat() < minresult){
                strResult = QString("<%1").arg(minresult);
            }
            if(strTempResult.toFloat() > maxresult){
                strResult = QString(">%1").arg(maxresult);
            }
            return strResult;
        }
    }
}

/********************
函数名称： SelectFormulaCalc
功    能：根据公式类型计算结果
参    数：公式类型(指数、对数、。。。)
         1、Logistic,4个未知数
         2、对数,2个未知数
         3、指数,2个未知数
         4、幂,  2个未知数
         5、多项式
返 回 值： 无
****************************/
float ResultCalc::SelectFormulaCalc(int nFormulaType,float fValue, float fParam1,float fParam2,float fParam3,float fParam4,float fParam5)
{
    float fResult = 0;
    switch(nFormulaType){
    case 1:
        fResult = (fParam1 - fParam2)/ (1 + qPow(fValue/fParam3,fParam4) ) + fParam2;
        break;
    case 2:
        fResult = fParam1 * qLn(fValue) + fParam2;
        break;
    case 3:
        fResult = fParam1 * qExp(fParam2*fValue);
        break;
    case 4:
        fResult = fParam1 * qPow(fValue,fParam2);
        break;
    case 5:
        if(0 == fParam1){
            fResult = 0;
        }else if( 0 == fParam2 && fParam1 != 0){
            fResult = fParam1;
        }else if( 0 == fParam3 && fParam2 != 0){
            fResult = fParam1*fValue + fParam2;
        }else if( 0 == fParam4 && fParam3 != 0){
            fResult = fParam1*qPow(fValue,2) + fParam2* fValue + fParam3;
        }else if( 0 == fParam5 && fParam4 != 0){
            fResult = fParam1*qPow(fValue,3) + fParam2*qPow(fValue,2) + fParam3*fValue + fParam4;
        }else{
            fResult = fParam1*qPow(fValue,4) + fParam2*qPow(fValue,3) + fParam3*qPow(fValue,2) + fParam4*fValue + fParam5;
        }
        break;
    default:
        break;
    }
    return fResult;
}

QString ResultCalc::ProcessDecimalPointPricision(QString strValue, quint8 nPrecision)
{
    int nDecimalPointIndex = strValue.indexOf('.');
    QString strResult;
    if(-1 == nDecimalPointIndex){
        switch(nPrecision){
        case 0:
            strResult = QString("%1").arg(strValue);
            break;
        case 1:
            strResult = QString("%1.0").arg(strValue);
            break;
        case 2:
            strResult = QString("%1.00").arg(strValue);
            break;
        case 3:
            strResult = QString("%1.000").arg(strValue);
            break;
        case 4:
            strResult = QString("%1.0000").arg(strValue);
            break;
        }
    }else{
        switch(nPrecision){
        case 0:
            strResult = QString("%1").arg(strValue.left(nDecimalPointIndex));
            break;
        case 1:
            strResult = QString("%1").arg(strValue.left(nDecimalPointIndex+2));
            break;
        case 2:
            strResult = QString("%1").arg(strValue.left(nDecimalPointIndex+3));
            break;
        case 3:
            strResult = QString("%1").arg(strValue.left(nDecimalPointIndex+4));
            break;
        case 4:
            strResult = QString("%1").arg(strValue.left(nDecimalPointIndex+5));
            break;
        }

    }
    return strResult;
}

/*
void ResultCalc::findPeak(const QVector<int> &v, QVector<int> &peakPositions)
{
    QVector<int> diff_v(v.size() - 1, 0);
    // 计算V的一阶差分和符号函数trend
    for (QVector<int>::size_type i = 0; i != diff_v.size(); i++)
    {
        if (v[i + 1] - v[i]>0)
            diff_v[i] = 1;
        else if (v[i + 1] - v[i] < 0)
            diff_v[i] = -1;
        else
            diff_v[i] = 0;
    }
    // 对Trend作了一个遍历
    for (int i = diff_v.size() - 1; i >= 0; i--)
    {
        if (diff_v[i] == 0 && i == diff_v.size() - 1)
        {
            diff_v[i] = 1;
        }
        else if (diff_v[i] == 0)
        {
            if (diff_v[i + 1] >= 0)
                diff_v[i] = 1;
            else
                diff_v[i] = -1;
        }
    }

    for (QVector<int>::size_type i = 0; i != diff_v.size() - 1; i++)
    {
        if (diff_v[i + 1] - diff_v[i] == -2)
            peakPositions.push_back(i + 1);
    }
}*/



/********************
函数名称： low_passfilter
功    能：寻找荧光信号的波峰范围
参    数： scanData,size
返 回 值：位置P1 P2 P3 P4
****************************/
void ResultCalc::low_passfilter(unsigned int *scanData)
{
    unsigned int i,k;
    unsigned int temp[SCAN_DATA_MAX];
    long tempdata;
    //unsigned int nShow;
    for(k=0;k<4;k++)
    {
        for(i=0;i<m_ScanDataSize;i++){
            temp[i] = scanData[i];
        }
        for(i=6;i<(m_ScanDataSize-6);i++)
        {
            tempdata =temp[i-6]+temp[i-5]+temp[i-4]+temp[i-3]+temp[i-2]+temp[i-1]+temp[i]+temp[i+1]+temp[i+2]+temp[i+3]+temp[i+4]+temp[i+5]+temp[i+6];
            scanData[i]=ceil(tempdata/13);
            //nShow = scanData[i];
            //qDebug()<<i<<":"<<nShow<<"  "<<"tempdata:"<<tempdata;
        }

    }
}


/*******************
*函数：GetMaxResultFromIDMessage
*功能：从ID卡信息中提取测试名称
*参数：保存单位数组，最大值编号(多联卡有多个编号)，ID卡信息
*返回：无
**********************/
void ResultCalc::GetMaxResultFromIDMessage(long *MaxResult,unsigned char No,unsigned char *IDMessage)
{
    //unsigned int i;
    unsigned char NewCardTag;
    long MaxValue,lTemp,MaxBase = 65536;

    switch(No)
    {
    case NO_TEST_NAME2:
    {
        MaxValue =  *(IDMessage+ID_NO_TEST2_MAX_RESULT+1) ;
        MaxValue = MaxValue<<8;
        MaxValue += *(IDMessage+ID_NO_TEST2_MAX_RESULT);
        lTemp = (*(IDMessage+ID_NO_TEST2_MAX_RESULT+2))*MaxBase;
        MaxValue += lTemp;
        MaxValue = MaxValue*100;
    }break;

    case NO_TEST_NAME3:
    {
        MaxValue =  *(IDMessage+ID_NO_TEST3_MAX_RESULT+1) ;
        MaxValue = MaxValue<<8;
        MaxValue += *(IDMessage+ID_NO_TEST3_MAX_RESULT);
        lTemp = (*(IDMessage+ID_NO_TEST3_MAX_RESULT+2))*MaxBase;
        MaxValue += lTemp;
        MaxValue = MaxValue*100;
    }break;

    case NO_TEST_NAME1:
    default:
    {
        NewCardTag = IDMessage[ID_NO_NEW_CARD_FLAG];
        switch(NewCardTag)
        {
        case ID_NEW_CARD_TAG://新卡的测试名称长度9->15，测试单位5->15，测试最大值65535->2^22，是在旧卡的数据后额外添加数据
        {
            MaxValue =  *(IDMessage+ID_NO_TEST1_MAX_RESULT+1);
            MaxValue = MaxValue<<8;
            MaxValue += *(IDMessage+ID_NO_TEST1_MAX_RESULT);
            lTemp = (*(IDMessage+ID_NO_TEST1_MAX_RESULT+2))*MaxBase;
            MaxValue += lTemp;
            MaxValue = MaxValue*100;
        }break;

        default:
        {
            MaxValue =  *(IDMessage+105) ;
            MaxValue = MaxValue<<8;
            MaxValue += *(IDMessage+104);
            MaxValue = MaxValue*100;
        }break;
        }
    }break;
    }

    *MaxResult = MaxValue;
}

void ResultCalc::GetMaxResultFromIDMessage(float *MinResult, float *MaxResult, unsigned char No, unsigned char *IDMessage)
{
    //unsigned int i;
    unsigned char NewCardTag;
    float MaxValue;
    long lTemp,MaxBase = 65536;
    float MinValue;
    QString strTemp;

    switch(No)
    {
    case 1:
    {
        strTemp = QString("%1").arg( *(IDMessage+78)*256 + *(IDMessage+79) + (double)*(IDMessage+80)/100);
        MinValue = strTemp.toFloat();
        strTemp = QString("%1").arg( *(IDMessage+81)*256 + *(IDMessage+82) + (double)*(IDMessage+83)/100);
        MaxValue =  strTemp.toFloat() ;
        break;
    }
    case 2:
        break;
    case 3:
        break;
    default:
    {
        strTemp = QString("%1").arg( *(IDMessage+78)*256 + *(IDMessage+79) + (double)*(IDMessage+80)/100);
        MinValue = strTemp.toFloat();
        strTemp = QString("%1").arg( *(IDMessage+81)*256 + *(IDMessage+82) + (double)*(IDMessage+83)/100);
        MaxValue =  strTemp.toFloat() ;
        break;
    }break;
    }

    *MaxResult = MaxValue;
    *MinResult = MinValue;
}

QString ResultCalc::ConvetIDCardToStr(QByteArray byteIDCardData)
{
    /*QString strIDCardData;
    for(int i=0; i<byteIDCardData.size(); i++){
        strIDCardData += QString("%1").arg((quint8)byteIDCardData.at(i),2,16,QLatin1Char('0'));
        qDebug()<<strIDCardData;
    }*/
    return QString(byteIDCardData.toHex().toUpper());
}

unsigned char *ResultCalc::ConvetIDCardToChar(QString IDCardData)
{
    int nLen = IDCardData.size();
    //qDebug()<<nLen;
    unsigned char* pRecord = new unsigned char[nLen/2 + 1];
    memset(pRecord,0,nLen/2 + 1);
    //QString strID = "0100010000010001010100000001000F3603394B0E14481438401D1B291D1B291D1B291D1B291D1B291D1B29000000040004000D003D00460088000000880000008800000088000000880000008800006873435250202020200107000503016D672F4C2001000032C8000300570502004F1303004F32070056140B0023000B0023000B0023000B0023000B0023000B00230006780064106873435250202020202020202020206D672F4C2020202020202020202020C80000";
    QStringList listStr;
    for(int i=0; i<nLen; i+=2){
        listStr.append(IDCardData.mid(i,2));
        //qDebug()<<IDCardData.mid(i,2);
    }
    //qDebug()<<listStr.size()<< listStr;
    for(int n=0; n<listStr.size(); n++){
        pRecord[n] = listStr.at(n).toInt(0,16);
    }
    //qDebug()<<pRecord[181];
    return pRecord;
}


unsigned char *ResultCalc::ConvetIDCardToChar(QByteArray byteIDCardData)
{
    int nLen = byteIDCardData.size();
    unsigned char* pRecord = new unsigned char[nLen];
    memset(pRecord,0,nLen);
    //QString strID = "0100010000010001010100000001000F3603394B0E14481438401D1B291D1B291D1B291D1B291D1B291D1B29000000040004000D003D00460088000000880000008800000088000000880000008800006873435250202020200107000503016D672F4C2001000032C8000300570502004F1303004F32070056140B0023000B0023000B0023000B0023000B0023000B00230006780064106873435250202020202020202020206D672F4C2020202020202020202020C80000";

    for(int n=0; n<nLen; n++){
        pRecord[n] = (quint8)byteIDCardData.at(n);
    }
    //qDebug()<<pRecord[181];
    return pRecord;
}
