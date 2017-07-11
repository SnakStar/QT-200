/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: CUtilSettings.cpp
 *  简要描述: 配置文件类实现文件，实现对INI文件的增加、读取和修改
 *
 *  创建日期: 2016-6-5
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "cutilsettings.h"

CUtilSettings::CUtilSettings(QObject *parent) :
    QObject(parent)
{
    //m_CurrentPath = QDir::currentPath();
    //m_IniFilePath = m_CurrentPath + "/Set/Setup.ini";
    m_IniFilePath = "/home/root/qt200/Set/Setup.ini";
}

/********************************************************
 *@Name:        ReadSettings
 *@Author:      HuaT
 *@Description: 按给定的键，获得默认路径的配置文件中对应键的值
 *@Param1:      需要获取值的键名
 *@Param2:      默认值获取的值，当获取指定的键名失败时，会将此值返回
 *@Return:      获取对应键值成功时，则返回对应的键值，失败则返回参数2
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
QVariant CUtilSettings::ReadSettings(const QString& key, const QVariant& defaultValue)
{
    QSettings settings(m_IniFilePath,QSettings::IniFormat);
    return settings.value(key,defaultValue);
}

/********************************************************
 *@Name:        WriteSettings
 *@Author:      HuaT
 *@Description: 按给定的键值对，写入默认路径的配置文件中
 *@Param1:      参数的键
 *@Param2:      参数的值
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void CUtilSettings::WriteSettings(const QString &key, const QVariant& value)
{
    QSettings settings(m_IniFilePath,QSettings::IniFormat);
    settings.setValue(key,value);
}

/********************************************************
 *@Name:        DefaultSettings
 *@Author:      HuaT
 *@Description: 当程序配置文件不存在或者丢失时，自动新建文件并初始化内容
 *              打印模式（0:手动打印，1:自动打印）
 *              Lis模式（0:无，1:ASTM，2:HL7）
 *              语言设置（0:中文，1:英文）
 *              通道设置（0:通道1，1:通道2，2:通道3）
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-5
********************************************************/
void CUtilSettings::DefaultSettings()
{
    QSettings settings(m_IniFilePath,QSettings::IniFormat);
    settings.setValue(PRINTMODE,0);
    settings.setValue(ENCODEMODE,0);
    settings.setValue(AGINGMODE,0);
    settings.setValue(AGINGTIME,0);
    settings.setValue(RAWDATAMODE,0);
    settings.setValue(RFWRITEMODE,0);
    settings.setValue(LOGRECORDMODE,0);

    settings.setValue(APOFFSET1,0);
    settings.setValue(APOFFSET2,0);
    settings.setValue(APOFFSET3,0);

    settings.setValue(HARDVERSION,"");

    settings.setValue(LISMODE,0);
    settings.setValue(LOCALIP,"");
    settings.setValue(SERVERIP,"");
    settings.setValue(SERVERPORT,0);
    settings.setValue(LANGUAGESET,0);
    settings.setValue(CHANNELSET,0);
}

/********************************************************
 *@Name:        GetIniFilePath
 *@Author:      HuaT
 *@Description: 获取配置文件的路径
 *@Param:       无
 *@Return:      返回记录数据的配置文件路径
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
QString CUtilSettings::GetIniFilePath()
{
    return m_IniFilePath;
}

/********************************************************
 *@Name:        CheckFileExist
 *@Author:      HuaT
 *@Description: 检测配置文件是否存在
 *@Param:       无
 *@Return:      配置文件存在返回真，不在则返回假
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
bool CUtilSettings::CheckFileExist()
{
    QFile iniFile(m_IniFilePath);
    //qDebug()<<m_IniFilePath;
    if(iniFile.exists()){
        return true;
    }else{
        return false;
    }
}

/********************************************************
 *@Name:        ReadSettingsInfoToMap
 *@Author:      HuaT
 *@Description: 读取配置文件信息到参数表中
 *@Param:       无
 *@Return:      返回参数表对象
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
QMap<QString,QString>* CUtilSettings::ReadSettingsInfoToMap()
{
    QSettings settings(m_IniFilePath,QSettings::IniFormat);
    m_SetParam[PRINTMODE]     = settings.value(PRINTMODE,0).toString();
    m_SetParam[ENCODEMODE]    = settings.value(ENCODEMODE,0).toString();
    m_SetParam[AGINGMODE]     = settings.value(AGINGMODE,0).toString();
    m_SetParam[AGINGTIME]     = settings.value(AGINGTIME,0).toString();
    m_SetParam[RAWDATAMODE]   = settings.value(RAWDATAMODE,0).toString();
    m_SetParam[RFWRITEMODE]   = settings.value(RFWRITEMODE,0).toString();
    m_SetParam[LOGRECORDMODE] = settings.value(LOGRECORDMODE,0).toString();

    m_SetParam[APOFFSET1]     = settings.value(APOFFSET1,0).toString();
    m_SetParam[APOFFSET2]     = settings.value(APOFFSET2,0).toString();
    m_SetParam[APOFFSET3]     = settings.value(APOFFSET3,0).toString();

    m_SetParam[HARDVERSION]   = settings.value(HARDVERSION,"V0.0.0").toString();

    m_SetParam[LISMODE]       = settings.value(LISMODE,0).toString();
    m_SetParam[LOCALIP]       = settings.value(LOCALIP,"").toString();
    m_SetParam[SERVERIP]      = settings.value(SERVERIP,"").toString();
    m_SetParam[SERVERPORT]    = settings.value(SERVERPORT,0).toString();
    m_SetParam[LANGUAGESET]   = settings.value(LANGUAGESET,0).toString();
    m_SetParam[CHANNELSET]    = settings.value(CHANNELSET,0).toString();
    return &m_SetParam;
}

/********************************************************
 *@Name:        WriteSettingsInfoToMap
 *@Author:      HuaT
 *@Description: 将Map映射中的数据写入到配置文件中
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-7
********************************************************/
void CUtilSettings::WriteSettingsInfoToMap()
{
    QSettings settings(m_IniFilePath,QSettings::IniFormat);
    QMap<QString,QString>::const_iterator it;
    for(it = m_SetParam.constBegin(); it != m_SetParam.constEnd(); it++){
        settings.setValue(it.key(),it.value());
    }
}

/********************************************************
 *@Name:        GetSettingsMap
 *@Author:      HuaT
 *@Description: 返回配置文件信息参数表对象
 *@Param:       无
 *@Return:      返回参数表对象
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
QMap<QString,QString> *CUtilSettings::GetSettingsMap()
{
    return &m_SetParam;
}

/********************************************************
 *@Name:        SetParam
 *@Author:      HuaT
 *@Description: 设置配置文件信息参数对应的键值
 *@Param1:      需要设置的键名
 *@Param2:      需要设置的值
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void CUtilSettings::SetParam(const QString &key, const QString &value)
{
    m_SetParam[key] = value;
}

/********************************************************
 *@Name:        PrintEnglishData
 *@Author:      HuaT
 *@Description: 打印英文病人信息
 *@Param1:      姓名
 *@Param2:      编号
 *@Param3:      年龄
 *@Param4:      性别
 *@Param5:      测试项目
 *@Param6:      测试结果
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-11
********************************************************/
void CUtilSettings::PrintEnglishData(QextSerialPort* SerialPort,QString strName,
                                     QString strNumber, QString strAge,
                                     QString strSex, QString strItem, QString strResult,
                                     QString strCheckTime,QString strRenfValue,QString strFlag)
{
    //QString strName,strNumber,strAge,strSex,strItem,strResult;
    QByteArray bytecmd;
    bytecmd[0] = 0x1B;
    bytecmd[1] = 0x61;
    bytecmd[2] = 0x01;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x1D;
    bytecmd[1] = 0x21;
    bytecmd[2] = 0x01;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    //SerialPort->write("Hospital");
    bytecmd[0] = 0x0A;
    SerialPort->write(Utf8ToGbk("Test report"));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x1B;
    bytecmd[1] = 0x61;
    bytecmd[2] = 0x00;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x1D;
    bytecmd[1] = 0x21;
    bytecmd[2] = 0x00;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    QString str;
    str = QString("Name:%1            Number:%2").arg(strName).arg(strNumber);
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    str = QString("Age:%1             Sex:%2").arg(strAge).arg(strSex);
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x1D;
    bytecmd[1] = 0x21;
    bytecmd[2] = 0x01;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    SerialPort->write("--------------------------------");
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x1D;
    bytecmd[1] = 0x21;
    bytecmd[2] = 0x00;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    str = QString("Item: %1").arg(strItem);
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    //str = QString("Sample Type: whole blood");
    //bytecmd.append(Utf8ToGbk(str));
    //SerialPort->write(bytecmd);
    //bytecmd.clear();
    //bytecmd[0] = 0x0A;
    //SerialPort->write(bytecmd);
    //bytecmd.clear();
    str = QString("Check Result:");
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();

    //hsCRP+CRP为多行结果
    QStringList strlistResultUnit = ParseTestUnit(strResult);
    if(strItem.compare("hsCRP+CRP") == 0){
        QString strValue = strlistResultUnit.at(0);
        if(strValue.indexOf('>') != -1 ){
            str = QString("hsCRP: >5mg/L");
            SerialPort->write(Utf8ToGbk(str));
            bytecmd[0] = 0x0A;
            SerialPort->write(bytecmd);
            bytecmd.clear();

            str = QString("%1: %2 %3").arg("CRP").arg(strResult).arg(strFlag);
            SerialPort->write(Utf8ToGbk(str));
            bytecmd[0] = 0x0A;
            SerialPort->write(bytecmd);
            bytecmd.clear();
        }else if(strValue.indexOf('<') != -1){
            str = QString("%1: %2 %3").arg("hsCRP").arg(strResult).arg(strFlag);
            SerialPort->write(Utf8ToGbk(str));
            bytecmd[0] = 0x0A;
            SerialPort->write(bytecmd);
            bytecmd.clear();

            str = QString("CRP: <5mg/L");
            SerialPort->write(Utf8ToGbk(str));
            bytecmd[0] = 0x0A;
            SerialPort->write(bytecmd);
            bytecmd.clear();
        }else{
            if(strlistResultUnit.at(0).toFloat() <= 5){
                str = QString("%1: %2 %3").arg("hsCRP").arg(strResult).arg(strFlag);
                SerialPort->write(Utf8ToGbk(str));
                bytecmd[0] = 0x0A;
                SerialPort->write(bytecmd);
                bytecmd.clear();

                str = QString("CRP: <5mg/L");
                SerialPort->write(Utf8ToGbk(str));
                bytecmd[0] = 0x0A;
                SerialPort->write(bytecmd);
                bytecmd.clear();
            }else{
                str = QString("hsCRP: >5mg/L");
                SerialPort->write(Utf8ToGbk(str));
                bytecmd[0] = 0x0A;
                SerialPort->write(bytecmd);
                bytecmd.clear();

                str = QString("%1: %2 %3").arg("CRP").arg(strResult).arg(strFlag);
                SerialPort->write(Utf8ToGbk(str));
                bytecmd[0] = 0x0A;
                SerialPort->write(bytecmd);
                bytecmd.clear();
            }
        }
    }else{
        str = QString("%1: %2 %3").arg(strItem).arg(strResult).arg(strFlag);
        SerialPort->write(Utf8ToGbk(str));
        bytecmd[0] = 0x0A;
        SerialPort->write(bytecmd);
        bytecmd.clear();
    }

    str = QString("reference Value Range:");
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    str = QString("%1: %2").arg(strItem).arg(strRenfValue);
    SerialPort->write(Utf8ToGbk(str));
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    str = QString("Note:");
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x1D;
    bytecmd[1] = 0x21;
    bytecmd[2] = 0x01;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    str = QString("--------------------------------");
    SerialPort->write(str.toAscii());
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x1D;
    bytecmd[1] = 0x21;
    bytecmd[2] = 0x00;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    str = QString("Check Date: %1").arg(strCheckTime);
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    str = QString("Opt Doctor: ");
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
}

void CUtilSettings::emitPrintSettingChange()
{
    emit PrintSettingChange();
}

void CUtilSettings::emitSettingChange()
{
    emit SettingChange();
}

/********************************************************
 *@Name:        PrintChineseData
 *@Author:      HuaT
 *@Description: 打印中文病人信息
 *@Param1:      姓名
 *@Param2:      编号
 *@Param3:      年龄
 *@Param4:      性别
 *@Param5:      测试项目
 *@Param6:      测试结果
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-7-11
********************************************************/
void CUtilSettings::PrintChineseData(QextSerialPort* SerialPort,QString strName,
                                     QString strNumber, QString strAge,
                                     QString strSex, QString strItem, QString strResult,
                                     QString strCheckTime,QString strRenfValue, QString strFlag)
{
    QByteArray bytecmd;
    bytecmd[0] = 0x1B;
    bytecmd[1] = 0x61;
    bytecmd[2] = 0x01;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x1D;
    bytecmd[1] = 0x21;
    bytecmd[2] = 0x01;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    //SerialPort->write("Hospital");
    bytecmd[0] = 0x0A;
    SerialPort->write(Utf8ToGbk("检测报告单"));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x1B;
    bytecmd[1] = 0x61;
    bytecmd[2] = 0x00;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x1D;
    bytecmd[1] = 0x21;
    bytecmd[2] = 0x00;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    QString str;
    str = QString("姓名:%1          编号:%2").arg(strName).arg(strNumber);
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    str = QString("年龄:%1          性别:%2").arg(strAge).arg(strSex);
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x1D;
    bytecmd[1] = 0x21;
    bytecmd[2] = 0x01;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    SerialPort->write("--------------------------------");
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x1D;
    bytecmd[1] = 0x21;
    bytecmd[2] = 0x00;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    str = QString("项目: %1").arg(strItem);
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    //str = QString("样本类型: 全血");
    //bytecmd.append(Utf8ToGbk(str));
    //SerialPort->write(bytecmd);
    //bytecmd.clear();
    //bytecmd[0] = 0x0A;
    //SerialPort->write(bytecmd);
    //bytecmd.clear();
    str = QString("检测结果:");
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();

    //hsCRP+CRP为多行结果
    QStringList strlistResultUnit = ParseTestUnit(strResult);
    if(strItem.compare("hsCRP+CRP") == 0){
        QString strValue = strlistResultUnit.at(0);
        if(strValue.indexOf('>') != -1 ){
            str = QString("hsCRP: >5mg/L");
            SerialPort->write(Utf8ToGbk(str));
            bytecmd[0] = 0x0A;
            SerialPort->write(bytecmd);
            bytecmd.clear();

            str = QString("%1: %2 %3").arg("CRP").arg(strResult).arg(strFlag);
            SerialPort->write(Utf8ToGbk(str));
            bytecmd[0] = 0x0A;
            SerialPort->write(bytecmd);
            bytecmd.clear();
        }else if(strValue.indexOf('<') != -1){
            str = QString("%1: %2 %3").arg("hsCRP").arg(strResult).arg(strFlag);
            SerialPort->write(Utf8ToGbk(str));
            bytecmd[0] = 0x0A;
            SerialPort->write(bytecmd);
            bytecmd.clear();

            str = QString("CRP: <5mg/L");
            SerialPort->write(Utf8ToGbk(str));
            bytecmd[0] = 0x0A;
            SerialPort->write(bytecmd);
            bytecmd.clear();
        }else{
            if(strlistResultUnit.at(0).toFloat() <= 5){
                str = QString("%1: %2 %3").arg("hsCRP").arg(strResult).arg(strFlag);
                SerialPort->write(Utf8ToGbk(str));
                bytecmd[0] = 0x0A;
                SerialPort->write(bytecmd);
                bytecmd.clear();

                str = QString("CRP: <5mg/L");
                SerialPort->write(Utf8ToGbk(str));
                bytecmd[0] = 0x0A;
                SerialPort->write(bytecmd);
                bytecmd.clear();
            }else{
                str = QString("hsCRP: >5mg/L");
                SerialPort->write(Utf8ToGbk(str));
                bytecmd[0] = 0x0A;
                SerialPort->write(bytecmd);
                bytecmd.clear();

                str = QString("%1: %2 %3").arg("CRP").arg(strResult).arg(strFlag);
                SerialPort->write(Utf8ToGbk(str));
                bytecmd[0] = 0x0A;
                SerialPort->write(bytecmd);
                bytecmd.clear();
            }
        }
    }else{
        str = QString("%1: %2 %3").arg(strItem).arg(strResult).arg(strFlag);
        SerialPort->write(Utf8ToGbk(str));
        bytecmd[0] = 0x0A;
        SerialPort->write(bytecmd);
        bytecmd.clear();
    }

    str = QString("参考值范围:");
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    str = QString("%1: %2").arg(strItem).arg(strRenfValue);
    SerialPort->write(Utf8ToGbk(str));
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    str = QString("说明:");
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x1D;
    bytecmd[1] = 0x21;
    bytecmd[2] = 0x01;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    str = QString("--------------------------------");
    SerialPort->write(str.toAscii());
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x1D;
    bytecmd[1] = 0x21;
    bytecmd[2] = 0x00;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    str = QString("检验时间: %1").arg(strCheckTime);
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    str = QString("操作医生: ");
    bytecmd.append(Utf8ToGbk(str));
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
    bytecmd[0] = 0x0A;
    SerialPort->write(bytecmd);
    bytecmd.clear();
}


/********************************************************
 *@Name:        Utf8ToGbk
 *@Author:      HuaT
 *@Description: UTF8编码格式转GBK编码格式
 *@Param:       UTF8编码字符串
 *@Return:      Gbk编码字符串内码
 *@Version:     1.0
 *@Date:        2016-6-20
********************************************************/
QByteArray CUtilSettings::Utf8ToGbk(QString str)
{
    QTextCodec *gbk = QTextCodec::codecForName("gb2312");
    QTextCodec *utf8 = QTextCodec::codecForName("utf8");
    QTextCodec::setCodecForTr(gbk);
    QTextCodec::setCodecForLocale(gbk);
    QTextCodec::setCodecForCStrings(gbk);
    QByteArray byteGbk;
    byteGbk.append(gbk->toUnicode(str.toLocal8Bit()));
    QTextCodec::setCodecForTr(utf8);
    QTextCodec::setCodecForLocale(utf8);
    QTextCodec::setCodecForCStrings(utf8);
    return byteGbk;
}

/********************************************************
 *@Name:        ProcValue
 *@Author:      HuaT
 *@Description: 处理参考值上下限的格式
 *@Param1:      参考值下限
 *@Param2:      参考值上限
 *@Return:      格式化的参考值，不符合要求返回空值
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
QString CUtilSettings::ProcValue(QString lower, QString upper)
{
    QString FormatValue = "";
    if(lower.isEmpty() && upper.isEmpty()){
        return FormatValue;
    }
    if(lower.isEmpty()){
        FormatValue = QString("<%1").arg(upper);
    }else if(upper.isEmpty()){
        FormatValue = QString(">%1").arg(lower);
    }else{
        FormatValue = QString("%1-%2").arg(lower).arg(upper);
    }
    return FormatValue;
}

/********************************************************
 *@Name:        ParseValue
 *@Author:      HuaT
 *@Description: 解析参考值
 *@Param:       带符号的参考值
 *@Return:      返回下限、上限值，返回格式固定为：
 *              0：下限，没有则插入空
 *              1：上限，没有则插入空
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
QStringList CUtilSettings::ParseValue(QString value)
{
    QStringList valueList;
    int index = -1;
    if(value.indexOf("<") != -1){
        index = value.indexOf("<");
        valueList.append("");
        valueList.append(value.mid(index+1));
    }else if(value.indexOf(">") != -1){
        index = value.indexOf(">");
        valueList.append(value.mid(index+1));
        valueList.append("");
    }else if(value.indexOf("-") != -1){
        index = value.indexOf("-");
        valueList.append(value.mid(0,index));
        valueList.append(value.mid(index+1));
    }else{
        valueList.append("");
        valueList.append("");
    }
    return valueList;
}

/********************************************************
 *@Name:        ParseTestUnit
 *@Author:      HuaT
 *@Description: 把结果和单位分离并返回
 *@Param:       带结果单位的字符串
 *@Return:      返回结果和单位的字符链表
 *@Version:     1.0
 *@Date:        2017-2-22
********************************************************/
QStringList CUtilSettings::ParseTestUnit(QString strResult)
{
    QStringList listResult;
    quint8 nAsc;
    for(int n=0; n<strResult.size(); n++){
        nAsc = (quint8)strResult.at(n).toAscii();
        //如果不是数字了,则就是单位开始了
        if(nAsc > 62){
            listResult.append(strResult.left(n));
            listResult.append(strResult.right(strResult.size()-n));
        }
    }
    return listResult;
}


/********************************************************
 *@Name:        GetRenfValue
 *@Author:      HuaT
 *@Description: 通过病人信息，获取参考值
 *@Param1:      通
 *@Param2:      对应测试状态
 *@Return:      对应病人结果的参考值
 *@Version:     1.0
 *@Date:        2016-12-29
********************************************************/
QString CUtilSettings::GetRenfValue(QString strAge, QStringList listRenfValue)
{
    QString strRenf;
    quint8 nAge = strAge.toInt();
    if(listRenfValue.size() == 0){
        return strRenf;
    }
    quint32 nAgeLow,nAgeHight;
    for(int n=0; n<listRenfValue.size(); n+=5){

        if(listRenfValue.at(n+2).isEmpty() && !listRenfValue.at(n+2).isEmpty()){
            nAgeLow = 0;
            nAgeHight = listRenfValue.at(n+3).toInt();
            if(nAge <= nAgeHight){
                strRenf = listRenfValue.at(n+1);
            }
        }else if(!listRenfValue.at(n+2).isEmpty() && listRenfValue.at(n+3).isEmpty()){
            nAgeLow = listRenfValue.at(n+2).toInt();
            nAgeHight = 0;
            if(nAge >= nAgeLow){
                strRenf = listRenfValue.at(n+1);
            }
        }else if(listRenfValue.at(n+2).isEmpty() && listRenfValue.at(n+3).isEmpty()){
            strRenf = listRenfValue.at(n+1);
        }else{
            nAgeLow = listRenfValue.at(n+2).toInt();
            nAgeHight = listRenfValue.at(n+3).toInt();
            if(nAge>=nAgeLow && nAge<=nAgeHight){
                strRenf = listRenfValue.at(n+1);
            }
        }
    }
    return strRenf;

}


/********************************************************
 *@Name:        GetResultFlag
 *@Author:      HuaT
 *@Description: 通过病人信息，获取参考值
 *@Param1:      病人对应项目参考值
 *@Param2:      病人检测结果
 *@Return:      返回病人打印标志是↑↓
 *@Version:     1.0
 *@Date:        2016-12-29
********************************************************/
QString CUtilSettings::GetResultFlag(QString strRenf, QString strResult)
{
    QString strFlag;
    if(strRenf.isEmpty()){
        return strFlag;
    }
    QString re;
    quint8 nAscii = 0;
    quint16 nResultCount = strResult.size();
    for(int n=0; n<nResultCount; n++){
        nAscii = (quint8)(strResult.at(n).toAscii());
        if(nAscii == 62 || nAscii == 60){
            continue;
        }
        if(nAscii >= 48 && nAscii <=57 || nAscii == 46){
            re.append(strResult.at(n));
        }else{
            break;
        }
    }
    float fResult = re.toFloat();
    float fRenfLow,fRenfHight;
    QStringList listRenf = ParseValue(strRenf);
    //只有上限，没有下限，小于号系列
    if(listRenf.at(0).isEmpty()){
        fRenfLow = 0;
        fRenfHight = listRenf.at(1).toFloat();
        if(fResult >=fRenfHight){
            strFlag="↑";
        }
    //只有下限，没有上限，大于号系列
    }else if(listRenf.at(1).isEmpty()){
        fRenfLow = listRenf.at(0).toFloat();
        fRenfHight = 0;
        if(fResult <=fRenfLow){
            strFlag = "↓";
        }
    //区间值系列
    }else{
        fRenfLow = listRenf.at(0).toFloat();
        fRenfHight = listRenf.at(1).toFloat();
        if(fResult >= fRenfHight){
            strFlag = "↑";
        }else if(fResult <= fRenfLow){
            strFlag = "↓";
        }
    }
    return strFlag;

}


