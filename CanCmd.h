#ifndef CANCMD_H
#define CANCMD_H

#define QT_SYSOPT_ACK             0x01
#define QT_SYSOPT_GETDEVINFO      0x02
#define QT_SYSOPT_GETLINKSTATUS   0x03
#define QT_SYSOPT_ALARM           0x04
#define QT_SYSOPT_REQUESTUPDATA   0x05
#define QT_SYSOPT_READYUPDATA     0x06
#define QT_SYSOPT_STARTUPDATA     0x07


#define QT_TESTOPT_SETUPTEST      0x10
#define QT_TESTOPT_STARTTEST      0x11
#define QT_TESTOPT_TESTDATA       0x12
#define QT_TESTOPT_STOPTEST       0x13
#define QT_TESTOPT_BARCODE        0x14
#define QT_TESTOPT_SHUTDOWN       0x15
#define QT_TESTOPT_SETAPOFFSET    0x16
#define QT_TESTOPT_GETAPOFFSET    0x17

//条码有效或无效
#define QT_BARCODE_VALID          0x00
#define QT_BARCODE_INVALID        0x01

//测试模式
#define QT_TESTMODE_AUTO          0x01
#define QT_TESTMODE_INSTANT       0x00

//主机状态
#define QT_HOST_MASTER            0x08
#define QT_HOST_SLAVE1            0x01
#define QT_HOST_SLAVE2            0x02
#define QT_HOST_SLAVE3            0x03

#define QT_NULL_SYNC              0x00

//ALARM
enum{
//QT_ALRMOPT_NOBARCODE=1,
    QT_ALARMOPT_MOTROINIT          = 1,          //电机初始化
    QT_ALARMOPT_OPTICALA           = 2,          //光电开关A
    QT_ALARMOPT_OPTICALB           = 3,          //光电开关B
    QT_ALARMOPT_LASER              = 4,          //激光组件
    QT_ALARMOPT_NOBARCODE          = 5,          //无条码
    QT_ALARMOPT_BARCODEERROR       = 6           //条码错误

};

enum LogType{
    QT_LOGOPT_NORMAL              = 0,          //正常信息
    QT_LOGOPT_ERROR               = 1           //错误信息
};


#endif // CANCMD_H
