/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: CUtilSettings.h
 *  简要描述: 配置文件类头文件，实现对INI文件的增加、读取和修改
 *
 *  创建日期: 2016-6-5
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef CUTILSETTINGS_H
#define CUTILSETTINGS_H

#include<QObject>
#include<QSettings>
#include<QDebug>
#include<QFile>
#include<QMap>
#include<QDir>
#include<QTextCodec>
#include"qextserial/qextserialport.h"

#define PRINTMODE   "GeneralSet/PrintMode"
#define ENCODEMODE  "GenaralSet/EncodeMode"
#define AGINGMODE   "GenaralSet/AgingMode"
#define AGINGTIME   "GenaralSet/AgingTime"
#define RAWDATAMODE "GenaralSet/RawdataMode"

#define APOFFSET1   "GenaralSet/APOFFSET1"
#define APOFFSET2   "GenaralSet/APOFFSET2"
#define APOFFSET3   "GenaralSet/APOFFSET3"

#define HARDVERSION "GenaralSet/HardVersion"

#define LISMODE     "SystemSet/LisMode"
#define LOCALIP     "SystemSet/LocalIP"
#define SERVERIP    "SystemSet/ServerIP"
#define SERVERPORT  "SystemSet/ServerPort"
#define LANGUAGESET "SystemSet/LanguageSet"
#define CHANNELSET  "SystemSet/ChannelSet"


class CUtilSettings : public QObject
{
    Q_OBJECT
public:
    explicit CUtilSettings(QObject *parent = 0);
private:
    //配置文件路径
    QString m_IniFilePath;
    //当前程序路径
	QString m_CurrentPath;
    //配置文件参数表
    QMap<QString,QString> m_SetParam;
public:
    //读配置文件
    QVariant ReadSettings(const QString& key, const QVariant& defaultValue=QVariant());
    //写配置文件
    void WriteSettings(const QString &key, const QVariant &value);
    //默认配置文件
    void DefaultSettings();
    //获取配置文件路径
    QString GetIniFilePath();
    //检测配置文件是否存在
    bool CheckFileExist();
    //读取配置信息到参数表中
    QMap<QString,QString> *ReadSettingsInfoToMap();
    //把参数表中的信息写入到配置信息文件中
    void WriteSettingsInfoToMap();
    //获取配置文件参数表
    QMap<QString, QString> *GetSettingsMap();
    //设置配置文件参数表内容
    void SetParam(const QString& key, const QString& value);
    //打印英文版病人数据
    void PrintEnglishData(QextSerialPort* SerialPort,QString strName, QString strNumber, QString strAge, QString strSex, QString strItem, QString strResult, QString strCheckTime, QString strRenfValue ,QString strFlag);
    //打印中文版病人数据
    void PrintChineseData(QextSerialPort* SerialPort, QString strName, QString strNumber, QString strAge, QString strSex, QString strItem, QString strResult, QString strCheckTime, QString strRenfValue, QString strFlag);
    //字节编码转换
    QByteArray Utf8ToGbk(QString str);
    //处理上下限的值，返回需要的格式
    QString ProcValue(QString lower,QString upper);
    //解析传入的结果值，处理成上下限的格式，at(0)为下限，at(1)为上限
    QStringList ParseValue(QString value);
    //分离结果和单位
    QStringList ParseTestUnit(QString strResult);
    //获取参考值
    QString GetRenfValue(QString strAge, QStringList listRenfValue);
    //获取结果标志
    QString GetResultFlag(QString strRenf, QString strResult);
    void emitPrintSettingChange();
    void emitSettingChange();
signals:
    void PrintSettingChange();
    void SettingChange();
    
public slots:
    
};

#endif // CUTILSETTINGS_H
