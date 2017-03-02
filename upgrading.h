/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: upgrading.h
 *  简要描述: 软件升级功能头文件
 *
 *  创建日期: 2016-10-14
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef UPGRADING_H
#define UPGRADING_H
#include"udiskshow.h"
#include"ui_udiskshow.h"

class Upgrading : public UDiskShow
{
public:
    Upgrading();

protected slots:
    //确定事件
    void on_btnOK_clicked();

private:
    //软件升级功能函数
    void UpdateSoftware();
};

#endif // UPGRADING_H
