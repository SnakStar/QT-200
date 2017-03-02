#ifndef HARDGRADING_H
#define HARDGRADING_H
#include"udiskshow.h"

class HardGrading : public UDiskShow
{
    Q_OBJECT
public:
    HardGrading(QObject* parent=0);
    ~HardGrading();
    //设置主窗口对象
    void SetMainWin(QObject *obj);
    //设置进度条的值
    void SetProgressValue(quint32 nValue);
    //设置进度条的范围
    void SetProgressRange(quint32 nMin, quint32 nMax);
protected slots:
    //确定事件
    void on_btnOK_clicked();
    //刷新事件
    virtual void on_btnRefresh_clicked();
private:
    QObject* m_MainWin;
private:
    void UpdateHardWare();
    int QueryUDisk();
    void ShowHardFile();
    QByteArray ReadFileData(QString strFilePath);
};

#endif // HARDGRADING_H
