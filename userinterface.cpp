/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: userinterface.cpp
 *  简要描述: 软件调试主界面实现文件
 *
 *  创建日期: 2017-01-11
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "userinterface.h"
#include "ui_userinterface.h"

UserInterface::UserInterface(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserInterface)
{
    ui->setupUi(this);
}

UserInterface::UserInterface(QWidget *parent,CQtProDB* db, CUtilSettings* settings,QextSerialPort* rfSerialPort) :
    QDialog(parent),
    ui(new Ui::UserInterface)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    //
    //this->setWindowFlags(Qt::FramelessWindowHint);
    //this->move(0,0);
    m_nTotal = 0;
    ui->leRFWriteTotal->setText(QString::number(m_nTotal));
    //初始化成员变量
    m_db = db;
    m_settings = settings;
    m_SetParam = m_settings->ReadSettingsInfoToMap();
    m_RFSerialPort = rfSerialPort;
    //初始化RF界面控件
    InitRFUIControl();
    //初始化接口导航列表
    InitListControl();
    //初始化radio控件
    InitRadioControl();
    //初始化偏移量参数
    InitAPOffSetShowValue();
    //调试界面射频串口数据信号
    connect(m_RFSerialPort,SIGNAL(readyRead()), this, SLOT(RecvRFSerialPortData()) );
    //连接列表控件和栈控件
    connect(ui->lwNavBar,SIGNAL(currentRowChanged(int)) , ui->stackedWidget, SLOT(setCurrentIndex(int)) );
    //根据配置文件更新控件
    //UpdateControl();
    ui->lbRFWriteModeState->setFont(QFont("simsun",26));
    ui->leRFWriteTotal->setFont(QFont("simsun",26));
}

UserInterface::~UserInterface()
{
    //qDebug()<<"~userinterface";
    m_settings->SetParam(RFWRITEMODE,"0");
    m_settings->WriteSettingsInfoToMap();
    delete ui;
}

/********************************************************
 *@Name:        RecvRFSerialPortData
 *@Author:      HuaT
 *@Description: 在调试界面解析射频串口数据
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-3-13
********************************************************/
void UserInterface::RecvRFSerialPortData()
{
    m_baRFSerialData.append(m_RFSerialPort->readAll());
    qDebug()<<m_baRFSerialData.size();
    quint32 nIDCardNumber,nIDCardCMD,nIDCardLen,nIDCardCheck;
    QString strIDCardData,strIDCardBatchNo,strTestName;
    QByteArray baFrameStart,baFrameEnd;
    baFrameStart = m_baRFSerialData.left(4);
    baFrameEnd = m_baRFSerialData.right(4);
    int nRfMode = m_SetParam->value(RFWRITEMODE,"0").toInt();

    if(baFrameStart.toHex().toUInt(0,16) == FRAMESTART && baFrameEnd.toHex().toUInt(0,16) == FRAMEEND){
        quint32 nCheck=0;
        //4-7为卡号
        nIDCardNumber = m_baRFSerialData.mid(4,4).toHex().toUInt(0,16);
        //8-11为命令
        nIDCardCMD = m_baRFSerialData.mid(8,4).toHex().toUInt(0,16);
        //12-15为数据长度
        nIDCardLen = m_baRFSerialData.mid(12,4).toHex().toUInt(0,16);
        //16-len为数据
        strIDCardData = m_baRFSerialData.mid(16,nIDCardLen).toHex().toUpper();
        //16到16+14为批号
        if(nIDCardLen <= 1){
            strIDCardBatchNo = "";
        }else{
            for(int n=16; n<16+14; n++){
                strIDCardBatchNo += QString::number((quint8)m_baRFSerialData.at(n));
            }
        }
        //strIDCardBatchNo = m_baRFSerialData.mid(16,14);
        qDebug()<<"strIDCardBatchNo:"<<strIDCardBatchNo;
        //167到167+15为项目名称
        strTestName = m_baRFSerialData.mid(167,15).trimmed();
        //len+16-len+20为检验码
        nIDCardCheck = m_baRFSerialData.mid(nIDCardLen+16,4).toHex().toUInt(0,16);
        for(int n=12; n<nIDCardLen+4+12; n++){
            nCheck += (quint8)m_baRFSerialData.at(n);
            //qDebug()<<n<<(quint8)m_RfidData.at(n);
        }
        quint32 nCheckFinal = ~nCheck+1;
        //校验数据,如果出错,则返回
        if(nIDCardCheck != nCheckFinal){
            //qDebug()<<"IDCard: "<<CardInfo.m_IDCardCheck;
            //qDebug()<<"nCheckFinal: "<<nCheckFinal;
            qDebug()<<"Check RFID Data Error";
            if(ui->teRFOptLog->toPlainText().length()>2000){
                ui->teRFOptLog->clear();
                ui->teRFOptLog->append("数据过多,清除当前缓存记录!");
            }
            QDateTime dt = QDateTime::currentDateTime();
            QString strLog = QString("%1    校验数据出错,请检验ID卡数据!").arg(dt.toString("yyyy-MM-dd hh:mm:ss"));
            ui->teRFOptLog->append(strLog);
            m_baRFSerialData.clear();
            return ;
        }
        if(1 == nRfMode){//读模式
            ui->leRFBarCode->setText(strIDCardBatchNo);
            ui->leRFItem->setText(strTestName);
            //复制数据
            m_baWaitCopySerialData.clear();
            m_baWaitCopySerialData.append(m_baRFSerialData);
            //提示
            if(ui->teRFOptLog->toPlainText().length()>2000){
                ui->teRFOptLog->clear();
                ui->teRFOptLog->append("数据过多,清除当前缓存记录!");
            }
            QDateTime dt = QDateTime::currentDateTime();
            QString strLog = QString("%1    读取数据成功,数据长度:%2").arg(dt.toString("yyyy-MM-dd hh:mm:ss")).arg(QString::number(nIDCardLen));
            ui->teRFOptLog->append(strLog);
        }else if(2 == nRfMode){ //写模式
            //模式流程:写入数据-接收写入结果-读取数据-进行数据校验
            //1、写入数据
            if(1 == nIDCardCMD){
                if(ui->teRFOptLog->toPlainText().length()>2000){
                    ui->teRFOptLog->clear();
                    ui->teRFOptLog->append("数据过多,清除当前缓存记录!");
                }
                //烧录数据后,射频卡发来的ACK,0为无错误,1为错误
                QDateTime dt = QDateTime::currentDateTime();
                QString strLog;
                if(0 == strIDCardData.toUInt(0,16)){
                    strLog = QString("%1    数据烧录成功,数据长度:%2,将进行数据校验!").arg(dt.toString("yyyy-MM-dd hh:mm:ss")).arg(QString::number(m_baWaitCopySerialData.size()));
                    ui->teRFOptLog->append(strLog);
                }else{
                    strLog = QString("%1    数据烧录失败!").arg(dt.toString("yyyy-MM-dd hh:mm:ss"));
                    ui->teRFOptLog->append(strLog);
                    ui->lbRFWriteModeState->setStyleSheet("color:red;");
                    ui->lbRFWriteModeState->setText("烧录失败");
                    m_baRFSerialData.clear();
                    return;
                }

                //主动请求ID卡数据
                QByteArray baQueryIDData;
                baQueryIDData[0] = (0x5A);
                baQueryIDData[1] = (0x1A);
                baQueryIDData[2] = (0xA1);
                baQueryIDData[3] = (0xA5);
                //ID卡号
                baQueryIDData[4] = (0x00);
                baQueryIDData[5] = (0x00);
                baQueryIDData[6] = (0x00);
                baQueryIDData[7] = (0x00);
                //命令码
                baQueryIDData[8] = (0x00);
                baQueryIDData[9] = (0x00);
                baQueryIDData[10] = (0x00);
                baQueryIDData[11] = (0x05);
                //数据长度
                baQueryIDData[12] = (0x00);
                baQueryIDData[13] = (0x00);
                baQueryIDData[14] = (0x00);
                baQueryIDData[15] = (0x01);
                //数据内容
                baQueryIDData[16] = (0x00);
                //校验和
                baQueryIDData[17] = (0xFF);
                baQueryIDData[18] = (0xFF);
                baQueryIDData[19] = (0xFF);
                baQueryIDData[20] = (0xFF);
                //帧尾
                baQueryIDData[21] = (0xA5);
                baQueryIDData[22] = (0xA1);
                baQueryIDData[23] = (0x1A);
                baQueryIDData[24] = (0x5A);
                m_RFSerialPort->write(baQueryIDData);
                //
                //ui->lbRFWriteModeState->setStyleSheet("color:red;");
                //ui->lbRFWriteModeState->setText("校验中");
            }else if(3 == nIDCardCMD){
                //滴新卡发来的数据,可不理会

                //然后烧录数据给新卡
                //修改数据模式,2为写数据模式
                QString strCardData = m_baWaitCopySerialData.mid(16,nIDCardLen);
                if(0 == strCardData.compare("IMPROVE QT-200")){
                    if(ui->teRFOptLog->toPlainText().length()>2000){
                        ui->teRFOptLog->clear();
                        ui->teRFOptLog->append("数据过多,清除当前缓存记录!");
                    }
                    QDateTime dt = QDateTime::currentDateTime();
                    QString strLog = QString("%1    提示:此卡为免密卡,不能复制此卡数据").arg(dt.toString("yyyy-MM-dd hh:mm:ss"));
                    ui->teRFOptLog->append(strLog);
                    //状态失败
                    ui->lbRFWriteModeState->setStyleSheet("color:red;");
                    ui->lbRFWriteModeState->setText("烧录失败");
                    m_baRFSerialData.clear();
                    return;
                }
                m_baWaitCopySerialData[11] = 2;
                m_RFSerialPort->write(m_baWaitCopySerialData);

            }else if(5 == nIDCardCMD){
                for(int n=0; n<m_baRFSerialData.size(); n++){
                    if((quint8)m_baRFSerialData.at(n) != (quint8)m_baWaitCopySerialData.at(n)){
                        if(ui->teRFOptLog->toPlainText().length()>2000){
                            ui->teRFOptLog->clear();
                            ui->teRFOptLog->append("数据过多,清除当前缓存记录!");
                        }
                        QDateTime dt = QDateTime::currentDateTime();
                        QString strLog = QString("%1    提示:校验数据不匹配,请重新复制此卡数据").arg(dt.toString("yyyy-MM-dd hh:mm:ss"));
                        ui->teRFOptLog->append(strLog);
                        //状态失败
                        ui->lbRFWriteModeState->setStyleSheet("color:red;");
                        ui->lbRFWriteModeState->setText("烧录失败");
                        //return;
                    }
                }
                if(ui->teRFOptLog->toPlainText().length()>2000){
                    ui->teRFOptLog->clear();
                    ui->teRFOptLog->append("数据过多,清除当前缓存记录!");
                }
                QDateTime dt = QDateTime::currentDateTime();
                QString strLog = QString("%1    提示:校验成功,数据已成功复制").arg(dt.toString("yyyy-MM-dd hh:mm:ss"));
                ui->teRFOptLog->append(strLog);
                //状态成功
                ui->lbRFWriteModeState->setStyleSheet("color:green;");
                ui->lbRFWriteModeState->setText("烧录成功");
                m_nTotal++;
                ui->leRFWriteTotal->setText(QString::number(m_nTotal));
            }
        }else{
            if(ui->teRFOptLog->toPlainText().length()>2000){
                ui->teRFOptLog->clear();
                ui->teRFOptLog->append("数据过多,清除当前记录!");
            }
            QDateTime dt = QDateTime::currentDateTime();
            QString strLog = QString("%1    提示:当前模式下,对数据不进行操作,请更改模式").arg(dt.toString("yyyy-MM-dd hh:mm:ss"));
            ui->teRFOptLog->append(strLog);
            //return;
        }
        //清除数据串口数据
        m_baRFSerialData.clear();
    }
}

/********************************************************
 *@Name:        UpdateControl
 *@Author:      HuaT
 *@Description: 从更新的参数列表中，根据配置参数，让控件显示对应值
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-1-11
********************************************************/
void UserInterface::UpdateControl()
{

}

/********************************************************
 *@Name:        InitListControl
 *@Author:      HuaT
 *@Description: 初始化列表导航栏控件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-11
********************************************************/
void UserInterface::InitListControl()
{
    QStringList listContent;
    listContent.append("常规接口");
    listContent.append("数据库接口");
    listContent.append("日志接口");
    listContent.append("射频接口");
    for(int n=0; n<listContent.size(); n++){
        QListWidgetItem *lwItem = new QListWidgetItem;
        lwItem->setSizeHint(QSize(140,50));
        lwItem->setText(listContent.at(n));
        ui->lwNavBar->insertItem(n,lwItem);
    }
}

/********************************************************
 *@Name:        InitRadioControl
 *@Author:      HuaT
 *@Description: 初始化Radio控件状态
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-11
********************************************************/
void UserInterface::InitRadioControl()
{
    //更新老化控件
    if(m_SetParam->contains(AGINGMODE)){
        int nAgingMode = m_SetParam->value(AGINGMODE).toInt();
        if(0 == nAgingMode){
            ui->rbAgingClose->setChecked(true);
        }else{
            ui->rbAgingOpen->setChecked(true);
        }
    }else{
        ui->rbAgingClose->setChecked(true);
    }
    //更新老化间隔时间
    ui->leAgingTime->setText(m_SetParam->value(AGINGTIME));
    //更新原始数据控件
    if(m_SetParam->contains(RAWDATAMODE)){
        int nRawData = m_SetParam->value(RAWDATAMODE).toInt();
        if(0 == nRawData){
            ui->rbRawDataClose->setChecked(true);
        }else{
            ui->rbRawDataOpen->setChecked(true);
        }
    }else{
        ui->rbRawDataClose->setChecked(true);
    }
    //更新射频卡模式控件
    if(m_SetParam->contains(RFWRITEMODE)){
        int nRFWriteMode = m_SetParam->value(RFWRITEMODE).toInt();
        if(1 == nRFWriteMode){
            ui->rbRFReadMode->setChecked(true);
        }else if(2 == nRFWriteMode){
            ui->rbRFWriteMode->setChecked(true);
        }else{
            ui->rbRFCloseMode->setChecked(true);
        }
    }else{
        ui->rbRFCloseMode->setChecked(true);
    }

}

/********************************************************
 *@Name:        InitAPOffSetShowValue
 *@Author:      HuaT
 *@Description: 初始化放大偏移量控件存储值
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-15
********************************************************/
void UserInterface::InitAPOffSetShowValue()
{
    ui->leAPOffSet1->setText((m_SetParam->value(APOFFSET1)) );
    ui->leAPOffSet2->setText((m_SetParam->value(APOFFSET2)) );
    ui->leAPOffSet3->setText((m_SetParam->value(APOFFSET3)) );
}

/********************************************************
 *@Name:        InitRFUIControl
 *@Author:      HuaT
 *@Description: 初始化射频烧录界面编辑控件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-03-14
********************************************************/
void UserInterface::InitRFUIControl()
{
    ui->leRFItem->setEnabled(false);
    ui->leRFBarCode->setEnabled(false);
    ui->teRFOptLog->setEnabled(false);
    ui->leRFWriteTotal->setEnabled(false);
}

/********************************************************
 *@Name:        on_btnUIOK_clicked
 *@Author:      HuaT
 *@Description: 常规接口-确定事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-11
********************************************************/
void UserInterface::on_btnUIOK_clicked()
{
    //老化
    if(ui->rbAgingClose->isChecked()){
        m_settings->SetParam(AGINGMODE,"0");
    }else{
        m_settings->SetParam(AGINGMODE,"1");
    }
    //老化时间
    m_settings->SetParam(AGINGTIME,ui->leAgingTime->text());
    //原始数据
    if(ui->rbRawDataClose->isChecked()){
        m_settings->SetParam(RAWDATAMODE,"0");
    }else{
        m_settings->SetParam(RAWDATAMODE,"1");
    }
    //写入数据至配置文件
    m_settings->WriteSettingsInfoToMap();
    //
    QString strOptTitle = QObject::tr("operating info");
    QString strOptContent = QObject::tr("Configuration saved successfully");
    QMessageBox::information(this,strOptTitle,strOptContent,QMessageBox::Ok);
}


/********************************************************
 *@Name:        on_btnStopAging_clicked
 *@Author:      HuaT
 *@Description: 常规接口-停止老化事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-11
********************************************************/
void UserInterface::on_btnStopAging_clicked()
{
    emit StopAgingTest();
}

/********************************************************
 *@Name:        SetCanBusObj
 *@Author:      HuaT
 *@Description: 设置Can通讯对象
 *@Param:       Can对象
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-15
********************************************************/
#ifdef Q_OS_LINUX
void UserInterface::SetCanBusObj(CanBus* cb)
{
    m_CanBus = cb;
}
#endif

/********************************************************
 *@Name:        on_btnUIClose_clicked
 *@Author:      HuaT
 *@Description: 退出调试对话框
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-15
********************************************************/
void UserInterface::on_btnUIClose_clicked()
{
    this->close();
}

/********************************************************
 *@Name:        on_btnAPOffSet1_clicked
 *@Author:      HuaT
 *@Description: 发送偏移量大小给通道1
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-15
********************************************************/
void UserInterface::on_btnAPOffSet1_clicked()
{
#ifdef Q_OS_LINUX
    QByteArray byteID,byteData;
    byteID[0] = QT_TESTOPT_SETAPOFFSET;
    byteID[1] = 0x00;
    byteID[2] = QT_HOST_MASTER;
    byteID[3] = QT_HOST_SLAVE1;
    byteData[0] = ui->leAPOffSet1->text().toInt();
    m_CanBus->SendMsg(byteID,byteData.size(),byteData);
#endif
    m_settings->SetParam(APOFFSET1,ui->leAPOffSet1->text());
    //写入数据至配置文件
    m_settings->WriteSettingsInfoToMap();
}

/********************************************************
 *@Name:        on_btnAPOffSet2_clicked
 *@Author:      HuaT
 *@Description: 发送偏移量大小给通道2
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-15
********************************************************/
void UserInterface::on_btnAPOffSet2_clicked()
{
#ifdef Q_OS_LINUX
    QByteArray byteID,byteData;
    byteID[0] = QT_TESTOPT_SETAPOFFSET;
    byteID[1] = 0x00;
    byteID[2] = QT_HOST_MASTER;
    byteID[3] = QT_HOST_SLAVE2;
    byteData[0] = ui->leAPOffSet2->text().toInt();
    m_CanBus->SendMsg(byteID,byteData.size(),byteData);
#endif
    m_settings->SetParam(APOFFSET2,ui->leAPOffSet2->text());
    //写入数据至配置文件
    m_settings->WriteSettingsInfoToMap();
}

/********************************************************
 *@Name:        on_btnAPOffSet3_clicked
 *@Author:      HuaT
 *@Description: 发送偏移量大小给通道3
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-15
********************************************************/
void UserInterface::on_btnAPOffSet3_clicked()
{
#ifdef Q_OS_LINUX
    QByteArray byteID,byteData;
    byteID[0] = QT_TESTOPT_SETAPOFFSET;
    byteID[1] = 0x00;
    byteID[2] = QT_HOST_MASTER;
    byteID[3] = QT_HOST_SLAVE3;
    byteData[0] = ui->leAPOffSet3->text().toInt();
    m_CanBus->SendMsg(byteID,byteData.size(),byteData);
#endif
    m_settings->SetParam(APOFFSET3,ui->leAPOffSet3->text());
    //写入数据至配置文件
    m_settings->WriteSettingsInfoToMap();
}

/********************************************************
 *@Name:        on_btnClearDb_clicked
 *@Author:      HuaT
 *@Description: 删除数据库事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-16
********************************************************/
void UserInterface::on_btnClearDb_clicked()
{
    QString strTitle,strContent;
    QByteArray byteError;
    QString strError;
    QString strDbPath = "/home/root/qt200/qtdb.db";
    QString strCMD = QString("rm %1").arg(strDbPath);
    QProcess pc;
    strContent = "删除数据库后不能恢复,是否继续";
    if(QMessageBox::Ok == QMessageBox::information(this,strTitle,strContent,QMessageBox::Ok|QMessageBox::Cancel)){
        pc.start(strCMD);
        pc.waitForFinished(-1);
        byteError = pc.readAllStandardError();
        strError = byteError.data();
        strTitle = "提示";
        if(strError.size() == 0){
            QMessageBox::information(this,strTitle,"数据库删除成功",QMessageBox::Ok);
        }else{
            QMessageBox::information(this,strTitle,strError,QMessageBox::Ok);
        }
    }
}

/********************************************************
 *@Name:        on_btnCopyDb_clicked
 *@Author:      HuaT
 *@Description: 复制数据库事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-01-16
********************************************************/
void UserInterface::on_btnCopyDb_clicked()
{

}

/********************************************************
 *@Name:        on_rbRFReadMode_clicked
 *@Author:      HuaT
 *@Description: 单选项控件-读模式对象点击
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-03-13
********************************************************/
void UserInterface::on_rbRFReadMode_clicked()
{
    //qDebug()<<"ReadMode Click";
    m_settings->SetParam(RFWRITEMODE,"1");
    m_settings->WriteSettingsInfoToMap();
}

/********************************************************
 *@Name:        on_rbRFWriteMode_clicked
 *@Author:      HuaT
 *@Description: 单选项控件-写模式对象点击
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-03-13
********************************************************/
void UserInterface::on_rbRFWriteMode_clicked()
{
    //qDebug()<<"WriteMode Click";
    m_settings->SetParam(RFWRITEMODE,"2");
    m_settings->WriteSettingsInfoToMap();
}

/********************************************************
 *@Name:        on_rbRFCloseMode_clicked
 *@Author:      HuaT
 *@Description: 单选项控件-关闭对象点击
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-03-13
********************************************************/
void UserInterface::on_rbRFCloseMode_clicked()
{
    m_settings->SetParam(RFWRITEMODE,"0");
    m_settings->WriteSettingsInfoToMap();
}

void UserInterface::on_btnRFClearWriteTotal_clicked()
{
    m_nTotal = 0;
    ui->leRFWriteTotal->setText(QString::number(m_nTotal));
}
