#include "resultcalc.h"



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
void ResultCalc::calculateResult(unsigned int *scanData1,unsigned int *scanData2,unsigned char *pRecord,unsigned char *pIdmessgae)
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
   CSignallength = *(pIdmessgae+ID_NO_CT_CIntegralLength2);//质控峰积分长度
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
    }

    //  如果 最大值在 边上 且 不是峰 ，则缩减范围 寻找峰值
     if( (p[0]==0|| p[1]==0|| p[2]==0|| p[3]==0) && (MaxPosition[0]>140 || MaxPosition[1]<180))
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
        if(area2<0){
          area2 = 0;
        }
        if(0 == area1){
            value1 = 0;
        }else{
            value1 = (area2*bit5)/area1; //比值1
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
        if(area2<0){
          area2 = 0;
        }
        if(0 == area1){
            value2 = 0;
        }else{
            value2 = (area2*bit5)/area1; //比值 2
        }

        if( MaxPosition[0]>150 ||  MaxPosition[1]<170)  //提防 信号位置偏出扫描区域
        {
          if(MaxPosition[0]>150)
          {value = value2;}
          else
          {value = value1;}
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
    unsigned char* pRecord = new unsigned char[185];
    memset(pRecord,0,185);
    //QString strID = "0100010000010001010100000001000F3603394B0E14481438401D1B291D1B291D1B291D1B291D1B291D1B29000000040004000D003D00460088000000880000008800000088000000880000008800006873435250202020200107000503016D672F4C2001000032C8000300570502004F1303004F32070056140B0023000B0023000B0023000B0023000B0023000B00230006780064106873435250202020202020202020206D672F4C2020202020202020202020C80000";
    QStringList listStr;
    for(int i=0; i<IDCardData.size(); i+=2){
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
