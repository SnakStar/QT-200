/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: setwindow.cpp
 *  简要描述: 设置界面实现文件，实现操作系统、仪器打印、时间、Lis等设置
 *
 *  创建日期: 2016-5-21
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "setwindow.h"
#include "ui_setwindow.h"
#include"mainwindow.h"
#include<qtextcodec.h>

SetWindow::SetWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetWindow)
{
    ui->setupUi(this);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));       //支持Tr中文

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8")); //支持中文文件名显示
    //修改下拉框高度,使qss文档属性中item生效
    //Sets the view to be used in the combobox popup to the given itemView.
    //The combobox takes ownership of the view.
    ui->cBoxLanguage->setView(new QListView());
    ui->cBoxChannel->setView(new QListView());
    //取消时间控件输入法焦点
    ui->dteDateSet->setProperty("noinput",true);
    ui->dteTimeSet->setProperty("noinput",true);
    //获取配置文件对象
    MainWindow* MainWin = (MainWindow*)parent;
    m_settings = MainWin->GetUtilSetting();
    m_QueryWin = MainWin->GetQueryWindow();
    m_TestWin = MainWin->GetTestWindow();
    m_RFSerialPort = MainWin->GetRFSerialPort();
    m_HL7SerialPort = MainWin->GetHL7SerialPort();
    //读取配置文件
    m_SetParam = m_settings->ReadSettingsInfoToMap();
    //初始化导航栏内容
    m_ListContent.append(QObject::tr("General Setting"));
    m_ListContent.append(QObject::tr("Renf Setting"));
    m_ListContent.append(QObject::tr("Date/Time Setting"));
    m_ListContent.append(QObject::tr("System Setting"));
    m_ListContent.append(QObject::tr("RF Card Info"));
    m_ListContent.append(QObject::tr("System Info"));
    m_ListContent.append(QObject::tr("Debug Mode"));

    //初始化设置界面时间控件
    InitDateToSetUi();
    //隐藏功能按钮
    HideDebugFunc(true);
    //隐藏导出按钮
    m_QueryWin->SetExportHide(true);

    /*
    //检测配置文件
    if(!m_settings.CheckFileExist()){
        //创建默认配置文件
        m_settings.DefaultSettings();
        //然后读取配置文件
        m_SetParam = m_settings.ReadSettingsInfoToMap();
    }else{
        //读取配置文件
        m_SetParam = m_settings.ReadSettingsInfoToMap();
    }*/
    //初始化设置导航栏
    InitListWidget();
    //先更新语言
    UpdateLanguage();
    //根据配置更新设置界面的控件
    UpdateControl();
    //连接导航栏事件
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    //初始化参考值表单
    InitRenfVTb();
    //初始化射频卡信息控件
    InitRFCardInfoControl();
    //密码显示方式
    ui->leDebugLoginPwd->setEchoMode(QLineEdit::Password);
    //隐藏ASTM选项
    ui->rbASTM->hide();
}

SetWindow::~SetWindow()
{
    delete ui;
}

/********************************************************
 *@Name:        InitListWidget
 *@Author:      HuaT
 *@Description: 初始化设置界面导航栏
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void SetWindow::InitListWidget()
{
    for(int i=0; i<m_ListContent.count(); i++)
    {
        //ui->listWidget->setSizeHint(140,70);
        QListWidgetItem* subitem = new QListWidgetItem;
        //QListWidgetItem subitem;
        subitem->setSizeHint(QSize(140,50));
        subitem->setText(m_ListContent.at(i));
        ui->listWidget->insertItem(ui->listWidget->count()+1,subitem);
    }
}

void SetWindow::UpdateListContent()
{
    m_ListContent.clear();
    m_ListContent.append(QObject::tr("General Setting"));
    m_ListContent.append(QObject::tr("Renf Setting"));
    m_ListContent.append(QObject::tr("Date/Time Setting"));
    m_ListContent.append(QObject::tr("System Setting"));
    m_ListContent.append(QObject::tr("RF Card Info"));
    m_ListContent.append(QObject::tr("System Info"));
    m_ListContent.append(QObject::tr("Debug Mode"));
    for(int i=0; i<m_ListContent.count(); i++){
        ui->listWidget->item(i)->setText(m_ListContent.at(i));
    }
}

/********************************************************
 *@Name:        InitDateToSetUi
 *@Author:      HuaT
 *@Description: 初始化设置界面中日期时间控件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void SetWindow::InitDateToSetUi()
{
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    ui->dteDateSet->setDate(date);
    ui->dteTimeSet->setTime(time);
}

/********************************************************
 *@Name:        LanguageSel
 *@Author:      HuaT
 *@Description: 初始化语言选择,并加载语言翻译文件
 *@Param:       语言选择索引，0为中文，1为英文
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-20
********************************************************/
void SetWindow::LanguageSel(int index)
{
    QTranslator *tl = new QTranslator(qApp);
    QString LanguagePath;
    QString CurrentPath = QDir::currentPath();
    CurrentPath = "/home/root/qt200";
    switch(index){
    case 0:
        LanguagePath = CurrentPath + "/Languages/QT200_ch_CN";
        break;
    case 1:
        LanguagePath = CurrentPath + "/Languages/QT200_en_CN";
        break;
    default:
        LanguagePath = CurrentPath + "/Languages/QT200_ch_CN";
        break;
    }
    bool bRet = tl->load(LanguagePath);
    if(!bRet){
        qDebug()<<"语言文件加载失败";
    }
    qApp->installTranslator(tl);
    this->ui->retranslateUi(this);
    InitRenfHeader();
    UpdateListContent();
}

/********************************************************
 *@Name:        UpdateLanguage
 *@Author:      HuaT
 *@Description: 根据配置更新选择的语言
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-20
********************************************************/
void SetWindow::UpdateLanguage()
{
    //更新语言设置-由于更新语言会导致界面其它控件更新，所以在此独立出来
    int LanguageSet = m_SetParam->value(LANGUAGESET,0).toInt();
    if(LanguageSet == 0){
        LanguageSel(0);
        ui->cBoxLanguage->setCurrentIndex(0);
    }else if(LanguageSet == 1){
        LanguageSel(1);
        ui->cBoxLanguage->setCurrentIndex(1);
    }else{
        LanguageSel(0);
        ui->cBoxLanguage->setCurrentIndex(0);
    }
}

/********************************************************
 *@Name:        InitRenfVTb
 *@Author:      HuaT
 *@Description: 初始化参考值列表框
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-13
********************************************************/
void SetWindow::InitRenfVTb()
{
    ui->twRefvInfo->setSelectionBehavior(QTableWidget::SelectRows);
    ui->twRefvInfo->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->twRefvInfo->setSelectionMode(QTableWidget::SingleSelection);
    ui->twRefvInfo->setColumnCount(RENFVALUE_COLUMN);
    ui->twRefvInfo->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->twRefvInfo->setRowCount(5);

    InitRenfHeader();
}

/********************************************************
 *@Name:        InitRFCardInfoControl
 *@Author:      HuaT
 *@Description: 初始化射频卡信息列表框
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-5-3
********************************************************/
void SetWindow::InitRFCardInfoControl()
{
    QStringList strlistRFCardInfo;
    strlistRFCardInfo<<QObject::tr("Item")<<QObject::tr("Batch No")<<QObject::tr("Card No")
                    <<QObject::tr("Expiry Date")<<QObject::tr("BarCode No");

    ui->twRFCardInfo->setSelectionBehavior(QTableWidget::SelectRows);
    ui->twRFCardInfo->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->twRFCardInfo->setSelectionMode(QTableWidget::SingleSelection);
    ui->twRFCardInfo->setColumnCount(strlistRFCardInfo.size());

    //ui->twRFCardInfo->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->twRFCardInfo->setColumnWidth(0,100);
    ui->twRFCardInfo->setColumnWidth(1,90);
    ui->twRFCardInfo->setColumnWidth(2,70);
    ui->twRFCardInfo->setColumnWidth(3,90);
    ui->twRFCardInfo->setColumnWidth(4,150);
    ui->twRFCardInfo->horizontalHeader()->setStretchLastSection(true);
    //ui->twRFCardInfo->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    ui->twRFCardInfo->setHorizontalHeaderLabels(strlistRFCardInfo);
}

/********************************************************
 *@Name:        InitRenfHeader
 *@Author:      HuaT
 *@Description: 初始化参考值表头名称
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-20
********************************************************/
void SetWindow::InitRenfHeader()
{
    QStringList headerList;
    /*headerList<<QObject::tr("序号")<<QObject::tr("项目")\
              <<QObject::tr("值")<<QObject::tr("年龄")\
              <<QObject::tr("性别");*/
    headerList<<QObject::tr("Item")<<QObject::tr("Value")\
              <<QObject::tr("Age")<<QObject::tr("Sex");
    ui->twRefvInfo->setHorizontalHeaderLabels(headerList);
}

/********************************************************
 *@Name:        GetRowValue
 *@Author:      HuaT
 *@Description: 获取指定表格控件的一行数据
 *@Param:       指定的表格控件对象
 *@Return:      一行的数据内容，未选择一行则返回空
 *@Version:     1.0
 *@Date:        2016-6-13
********************************************************/
QStringList SetWindow::GetRowValue(QTableWidget *tw)
{
    QStringList strList;
    if(tw->currentRow() == -1){
        return strList;
    }
    for(int i=0; i<tw->columnCount(); i++){
        strList.append(tw->item(tw->currentRow(),i)->text());
    }
    return strList;
}


/********************************************************
 *@Name:        UpdateDbToRenf
 *@Author:      HuaT
 *@Description: 更新参考值数据表内容到表格上
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-13
********************************************************/
void SetWindow::UpdateDbToRenf()
{
    ui->twRefvInfo->setRowCount(0);
    QString querySql = "select * from renfvalue";
    QSqlQuery* query = m_db->GetSqlQuery();
    query->exec(querySql);
    quint8 nSex;
    QString strAge;
    while(query->next()){
        ui->twRefvInfo->insertRow(ui->twRefvInfo->rowCount());
        //项目名称
        QTableWidgetItem* item1 = new QTableWidgetItem;
        item1->setText(query->value(0).toString());
        ui->twRefvInfo->setItem(ui->twRefvInfo->rowCount()-1,0,item1);
        //参考值
        QTableWidgetItem* item2 = new QTableWidgetItem;
        item2->setText(query->value(1).toString());
        ui->twRefvInfo->setItem(ui->twRefvInfo->rowCount()-1,1,item2);
        //年龄
        QTableWidgetItem* item3 = new QTableWidgetItem;
        strAge = m_settings->ProcValue(query->value(2).toString(),query->value(3).toString());
        item3->setText(strAge);
        ui->twRefvInfo->setItem(ui->twRefvInfo->rowCount()-1,2,item3);
        //性别
        QTableWidgetItem* item4 = new QTableWidgetItem;
        nSex = query->value(4).toInt();
        if(nSex == 1){
            item4->setText(QObject::tr("Male"));
        }else if(nSex == 2){
            item4->setText(QObject::tr("Female"));
        }else{
            item4->setText(QObject::tr("Male/Female"));
        }
        ui->twRefvInfo->setItem(ui->twRefvInfo->rowCount()-1,3,item4);
    }
}


/********************************************************
 *@Name:        RecvAddRenfValue
 *@Author:      HuaT
 *@Description: 接收新增参考值信息
 *@Param:       需要新增的参考值内容
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void SetWindow::RecvAddRenfValue(RenfInfo &ri)
{
    //插入数据库
    //用户性别，0为男/女，1为男，2为女，Male / Female
    quint8 nSex;
    if(ri.m_Sex.compare(QObject::tr("Male")) == 0){
        nSex = 1;
    }else if(ri.m_Sex.compare(QObject::tr("Female")) == 0){
        nSex = 2;
    }else{
        nSex = 0;
    }
    QString insertSql;
    insertSql = QString("insert into renfvalue values('%1','%2','%3','%4',%5);")\
            .arg(ri.m_Item).arg(ri.m_Value).arg(ri.m_AgeLow).arg(ri.m_AgeHight).arg(nSex);
    m_db->Exec(insertSql);
    //插入表单控件
    ui->twRefvInfo->insertRow(ui->twRefvInfo->rowCount());
    //项目名称
    QTableWidgetItem* item = new QTableWidgetItem;
    item->setText(ri.m_Item);
    ui->twRefvInfo->setItem(ui->twRefvInfo->rowCount()-1,0,item);
    //值
    QTableWidgetItem* value = new QTableWidgetItem;
    value->setText(ri.m_Value);
    ui->twRefvInfo->setItem(ui->twRefvInfo->rowCount()-1,1,value);
    //年龄
    QTableWidgetItem* age = new QTableWidgetItem;
    age->setText(m_settings->ProcValue(ri.m_AgeLow,ri.m_AgeHight));
    ui->twRefvInfo->setItem(ui->twRefvInfo->rowCount()-1,2,age);
    //性别
    QTableWidgetItem* sex = new QTableWidgetItem;
    sex->setText(ri.m_Sex);
    ui->twRefvInfo->setItem(ui->twRefvInfo->rowCount()-1,3,sex);
}

/********************************************************
 *@Name:        RecvModifyRenfValue
 *@Author:      HuaT
 *@Description: 接收修改参考值信息
 *@Param:       需要修改的参考值内容
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void SetWindow::RecvModifyRenfValue(RenfInfo &ri)
{
    int nCurrentRow = ui->twRefvInfo->currentRow();
    QStringList strList = GetRowValue(ui->twRefvInfo);
    //
    quint8 nSex,nOldSex;
    if(ri.m_Sex.compare(QObject::tr("Male")) == 0){
        nSex = 1;
    }else if(ri.m_Sex.compare(QObject::tr("Female")) == 0){
        nSex = 2;
    }else{
        nSex = 0;
    }
    if(strList.at(3).compare(QObject::tr("Male")) == 0){
        nOldSex = 1;
    }else if(strList.at(3).compare(QObject::tr("Female")) == 0){
        nOldSex = 2;
    }else{
        nOldSex = 0;
    }
    QStringList listOleAge;
    //0：下限，没有则插入空
    //1：上限，没有则插入空
    listOleAge = m_settings->ParseValue(strList.at(2));
    QString UpdateSql;
    //更新数据库内容
    UpdateSql = QString("update renfvalue set item='%1',value='%2',AgeLow='%3',AgeHight='%4'\
                        ,sex=%5 where item='%6' and value='%7' and AgeLow='%8' and AgeHight='%9' and sex=%10 ")\
            .arg(ri.m_Item).arg(ri.m_Value).arg(ri.m_AgeLow).arg(ri.m_AgeHight)
            .arg(nSex).arg(strList.at(0)).arg(strList.at(1))
            .arg(listOleAge.at(0)).arg(listOleAge.at(1)).arg(nOldSex);
    //qDebug()<<UpdateSql;
    if(!m_db->Exec(UpdateSql)){
        qDebug()<<"RecvModifyRenfValue: update renfvalue error";
    }
    //更新控件内容
    //项目名称
    QTableWidgetItem* item = new QTableWidgetItem;
    item->setText(ri.m_Item);
    ui->twRefvInfo->setItem(nCurrentRow,0,item);
    //新值
    QTableWidgetItem* Value = new QTableWidgetItem;
    Value->setText(ri.m_Value);
    ui->twRefvInfo->setItem(nCurrentRow,1,Value);
    //新的年龄
    QTableWidgetItem* Age = new QTableWidgetItem;
    Age->setText(m_settings->ProcValue(ri.m_AgeLow,ri.m_AgeHight));
    ui->twRefvInfo->setItem(nCurrentRow,2,Age);
    //新的性别
    QTableWidgetItem* Sex = new QTableWidgetItem;
    Sex->setText(ri.m_Sex);
    ui->twRefvInfo->setItem(nCurrentRow,3,Sex);
}

/********************************************************
 *@Name:        UpdateControl
 *@Author:      HuaT
 *@Description: 从更新的参数列表中，根据配置参数，让控件显示对应值
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void SetWindow::UpdateControl()
{
    //更新打印模式
    if(m_SetParam->contains(PRINTMODE)){
        int PrintModel = m_SetParam->value(PRINTMODE,0).toInt();
        if(PrintModel == 0){
            ui->rbManual->setChecked(true);
        }else{
            ui->rbAuto->setChecked(true);
        }
    }else{
        ui->rbManual->setChecked(true);
    }

    //更新编码模式
    if(m_SetParam->contains(ENCODEMODE)){
        int PrintModel = m_SetParam->value(ENCODEMODE,0).toInt();
        if(PrintModel == 0){
            ui->rbAutoEncode->setChecked(true);
        }else{
            ui->rbScanEncode->setChecked(true);
        }
    }else{
        ui->rbAutoEncode->setChecked(true);
    }

    //更新连接模式
    if(m_SetParam->contains(LISMODE)){
        int LisModel = m_SetParam->value(LISMODE,0).toInt();
        if(LisModel == 1){
            ui->rbASTM->setChecked(true);
        }else if(LisModel == 2){
            ui->rbHL7->setChecked(true);
        }else{
            ui->rbNull->setChecked(true);
        }
    }else{
        ui->rbManual->setChecked(true);
    }

    //更新本地IP
    QString LocalIp = m_SetParam->value(LOCALIP,"");
    ui->leLocalIP->setText(LocalIp);

    //更新服务器IP
    QString ServerIp = m_SetParam->value(SERVERIP,"");
    ui->leServerIP->setText(ServerIp);

    //更新服务器端口
    QString ServerPort = m_SetParam->value(SERVERPORT,0);
    ui->leServerPort->setText(ServerPort);

    //更新设备ID
    QString DeviceID = m_SetParam->value(DEVICEID,"");
    ui->leDeviceID->setText(DeviceID);

    /*//更新语言设置-由于更新语言会导致界面其它控件更新，所以在此独立出来
    int LanguageSet = m_SetParam->value("SystemSet/LanguageSet",0).toInt();
    if(LanguageSet == 0){
        InitLanguage(0);
        ui->cBoxLanguage->setCurrentIndex(0);
    }else if(LanguageSet == 1){
        InitLanguage(1);
        ui->cBoxLanguage->setCurrentIndex(1);
    }else{
        InitLanguage(0);
        ui->cBoxLanguage->setCurrentIndex(0);
    }*/

    //更新通道设置
    int ChannelSet = m_SetParam->value(CHANNELSET,0).toInt();
    if(ChannelSet == 1){
        ui->cBoxChannel->setCurrentIndex(1);
    }else if(ChannelSet == 2){
        ui->cBoxChannel->setCurrentIndex(2);
    }else {
        ui->cBoxChannel->setCurrentIndex(0);
    }
}

/********************************************************
 *@Name:        AutoUnlock
 *@Author:      HuaT
 *@Description: 通过射频卡自动完成解密
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-3-15
********************************************************/
void SetWindow::AutoUnlock()
{
    ui->leDebugLoginPwd->setText("");
    QString strAuthPassword;
    QDate currentdate = QDate::currentDate();
    strAuthPassword = QString("1996%1").arg(currentdate.toString("MMdd"));
    ui->leDebugLoginPwd->setText(strAuthPassword);
    //qDebug()<<strAuthPassword;
}

/********************************************************
 *@Name:        on_btnPrintSave_clicked
 *@Author:      HuaT
 *@Description: 普通设置-保存事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void SetWindow::on_btnPrintSave_clicked()
{
    if(ui->rbManual->isChecked()){
        //m_SetParam->key["PrintSet/PrintModel"] = "0";
        m_settings->SetParam(PRINTMODE,"0");
    }
    if(ui->rbAuto->isChecked()){
        m_settings->SetParam(PRINTMODE,"1");
    }
    if(ui->rbAutoEncode->isChecked()){
        m_settings->SetParam(ENCODEMODE,"0");
    }
    if(ui->rbScanEncode->isChecked()){
        m_settings->SetParam(ENCODEMODE,"1");
    }
    //写入配置信息到配置文件
    m_settings->WriteSettingsInfoToMap();
    QString strOptTitle = QObject::tr("operating info");
    QString strOptContent = QObject::tr("Configuration saved successfully");
    QMessageBox::information(this,strOptTitle,strOptContent,QMessageBox::Ok);
    m_settings->emitPrintSettingChange();
}

/********************************************************
 *@Name:        on_btnPrintCancel_clicked
 *@Author:      HuaT
 *@Description: 普通设置-取消事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-7
********************************************************/
void SetWindow::on_btnPrintCancel_clicked()
{
    //取消后还原默认配置信息
    UpdateControl();
}

/********************************************************
 *@Name:        on_btnAdd_clicked
 *@Author:      HuaT
 *@Description: 参数值设置-增加项目事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-7
********************************************************/
void SetWindow::on_btnAdd_clicked()
{
    RenfInfo ri;
    //项目列表
    QString strCMD = "select item from itemtype";
    ri.m_listItem = m_db->ExecQuery(strCMD);
    //qDebug()<<ri.m_listItem;
    //参考值对话框
    RenfValue *RenfvDlg = new RenfValue(ri,this);
    connect(RenfvDlg,SIGNAL(RenfInfoAddComplete(RenfInfo&)),this,SLOT(RecvAddRenfValue(RenfInfo&)));
    RenfvDlg->SetShowMode(true);
    RenfvDlg->show();
}

/********************************************************
 *@Name:        on_btnModify_clicked
 *@Author:      HuaT
 *@Description: 参数值设置-修改选中的参考值
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-7
********************************************************/
void SetWindow::on_btnModify_clicked()
{
    //参考值对话框
    RenfInfo ri;
    QString Title = QObject::tr("Note");
    QString Msg = QObject::tr("Please choose the data to modify");
    int CurrentRow = ui->twRefvInfo->currentRow();
    if(CurrentRow == -1){
        QMessageBox::information(this,Title,Msg,QMessageBox::Ok);
        return;
    }
    QStringList strList = GetRowValue(ui->twRefvInfo);
    //项目名称
    ri.m_Item = strList.at(0);
    //项目列表
    QString strCMD = "select item from itemtype";
    ri.m_listItem = m_db->ExecQuery(strCMD);
    //qDebug()<<ri.m_listItem;
    //参考值
    ri.m_Value = strList.at(1);
    //年龄
    QStringList listAge = m_settings->ParseValue(strList.at(2));
    ri.m_AgeLow = listAge.at(0);
    ri.m_AgeHight = listAge.at(1);
    //性别
    ri.m_Sex = strList.at(3);
    RenfValue *RenfvDlg = new RenfValue(ri,this);
    connect(RenfvDlg,SIGNAL(RenfInfoModifyComplete(RenfInfo&)),this,SLOT(RecvModifyRenfValue(RenfInfo&)));
    RenfvDlg->SetShowMode(false);
    RenfvDlg->show();
}

/********************************************************
 *@Name:        on_btnDelete_clicked
 *@Author:      HuaT
 *@Description: 参数值设置-删除项目事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-7
********************************************************/
void SetWindow::on_btnDelete_clicked()
{
    QString Title = QObject::tr("Note");
    QString Msg = QObject::tr("Please choose the data to delete");
    int CurrentRow = ui->twRefvInfo->currentRow();
    if(CurrentRow == -1){
        QMessageBox::information(this,Title,Msg,QMessageBox::Ok);
        return;
    }
    QString strTitle,strContent;
    strTitle = QObject::tr("Note");
    strContent = QObject::tr("Confirmed will have to delete?");

    if(QMessageBox::Yes == QMessageBox::information(this,strTitle,strContent,QMessageBox::Yes | QMessageBox::No)){
        QStringList stringList = GetRowValue(ui->twRefvInfo);
        //更新数据库
        QString deleteSql;
        QStringList listAge;
        listAge = m_settings->ParseValue(stringList.at(2));
        quint8 nSex;
        if(stringList.at(3).compare(QObject::tr("Male")) == 0){
            nSex = 1;
        }else if(stringList.at(3).compare(QObject::tr("Female")) == 0){
            nSex = 2;
        }else{
            nSex = 0;
        }
        deleteSql = QString("delete from renfvalue where \
                            item='%1' and value='%2' and agelow='%3' and agehight='%4'and sex=%5 ")\
                .arg(stringList.at(0)).arg(stringList.at(1)).arg(listAge.at(0))
                .arg(listAge.at(1)).arg(nSex);
                m_db->Exec(deleteSql);
        //qDebug()<<deleteSql;
        //更新控件
        ui->twRefvInfo->removeRow(CurrentRow);
    }
}

/********************************************************
 *@Name:        on_btnDateOk_clicked
 *@Author:      HuaT
 *@Description: 日期设置-修改确认事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-7
********************************************************/
void SetWindow::on_btnSetDateSave_clicked()
{
    QDate date = ui->dteDateSet->date();
    QTime time = ui->dteTimeSet->time();
    SetSysDateTime(date,time);
    QString msgTitle = QObject::tr("operating info");
    QString msgContent = QObject::tr("Configuration saved successfully");
    QMessageBox::information(this,msgTitle,msgContent,QMessageBox::Ok);
}


/********************************************************
 *@Name:        SetSysDateTime
 *@Author:      HuaT
 *@Description: 设置操作系统时间
 *@Param:
 *@Return:
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void SetWindow::SetSysDateTime(QDate& date, QTime time)
{

    //qDebug()<<date<<time;
#ifdef Q_OS_WIN32
    SYSTEMTIME st;
    //getsystemtime函数使用的是UTC时间，设置时会有时间上的偏差
    //GetSystemTime(&st);
    GetLocalTime(&st);
    st.wYear = date.year();
    st.wMonth = date.month();
    st.wDay = date.day();

    st.wHour = time.hour();
    st.wMinute = time.minute();
    st.wSecond = time.second();
    //qDebug()<<date.year()<<date.month()<<date.day()<<time.hour()<<time.minute()<<time.second();
    //SetSystemTime(&st);
    SetLocalTime(&st);

#elif defined(Q_OS_LINUX)
#include<time.h>
    QString WriteTime;
    WriteTime = QString("date -s \"%1 %2\" ").arg(date.toString("yyyy-MM-dd")).arg(time.toString("HH:mm:ss"));
    system(WriteTime.toAscii());
    system("hwclock -w");

#else
//#ifdef __arm__
#include<time.h>
    QString WriteTime;
    WriteTime = QString("date -s \"%1 %2\" ").arg(date.toString("yyyy-MM-dd")).arg(time.toString("HH:mm:ss"));
    system(WriteTime.toAscii());
    system("hwclock -w");
#endif


}


/********************************************************
 *@Name:        on_btnSysSave_clicked
 *@Author:      HuaT
 *@Description: 系统设置-保存事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-7
********************************************************/
void SetWindow::on_btnSysSave_clicked()
{
    //lis模式
    if(ui->rbNull->isChecked()){
        m_settings->SetParam(LISMODE,"0");
    }else if(ui->rbASTM->isChecked()){
        m_settings->SetParam(LISMODE,"1");
    }else if(ui->rbHL7->isChecked()){
        m_settings->SetParam(RFWRITEMODE,"0");
        m_settings->SetParam(LISMODE,"2");
    }
    //本机IP
    QString LocalIP = ui->leLocalIP->text();
    m_settings->SetParam(LOCALIP,LocalIP);
    //服务器IP
    QString ServerIP = ui->leServerIP->text();
    m_settings->SetParam(SERVERIP,ServerIP);
    //服务器端口
    QString ServerPort = ui->leServerPort->text();
    m_settings->SetParam(SERVERPORT,ServerPort);
    //语言选择
    if(ui->cBoxLanguage->currentIndex() == 0){
        m_settings->SetParam(LANGUAGESET,"0");
    }else if(ui->cBoxLanguage->currentIndex() == 1){
        m_settings->SetParam(LANGUAGESET,"1");
    }
    //通道选择
    if(ui->cBoxChannel->currentIndex() == 0){
        m_settings->SetParam(CHANNELSET,"0");
    }else if(ui->cBoxChannel->currentIndex() == 1){
        m_settings->SetParam(CHANNELSET,"1");
    }else if(ui->cBoxChannel->currentIndex() == 2){
        m_settings->SetParam(CHANNELSET,"2");
    }
    //更新参数表到配置文件
    m_settings->WriteSettingsInfoToMap();
    QString msgTitle = QObject::tr("operating info");
    QString msgContent = QObject::tr("Configuration saved successfully");
    QMessageBox::information(this,msgTitle,msgContent,QMessageBox::Ok);
    m_settings->emitSettingChange();
}

/********************************************************
 *@Name:        on_btnSysCancel_clicked
 *@Author:      HuaT
 *@Description: 系统设置-取消事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-7
********************************************************/
void SetWindow::on_btnSysCancel_clicked()
{
    //直接更新控件状态
    UpdateControl();
}

/********************************************************
 *@Name:        on_btnSoftUpdate_clicked
 *@Author:      HuaT
 *@Description: 系统信息-软件升级事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-7
********************************************************/
void SetWindow::on_btnSoftUpdate_clicked()
{
    //QMessageBox::information(this,"消息","功能完善中，请等待",QMessageBox::Ok);
    Upgrading* upd = new Upgrading;
    upd->show();
    upd->setWindowTitle(QObject::tr("Soft Update"));
}

/********************************************************
 *@Name:        on_btnHardUpdate_clicked
 *@Author:      HuaT
 *@Description: 系统信息-硬件升级事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-11-15
********************************************************/
void SetWindow::on_btnHardUpdate_clicked()
{
    HardGrading* hardup = new HardGrading();
    hardup->show();
    hardup->setWindowTitle(QObject::tr("Hard Update"));
    hardup->SetMainWin(this->parent()->parent()->parent()->parent());
}

/********************************************************
 *@Name:        SetDbObj
 *@Author:      HuaT
 *@Description: 获取数据库对象指针
 *@Param:       数据库对象地址
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-7
********************************************************/
void SetWindow::SetDbObj(CQtProDB *db)
{
    m_db = db;
}

/********************************************************
 *@Name:        on_cBoxLanguage_activated
 *@Author:      HuaT
 *@Description: 语言选择改变事件
 *@Param:       下拉框索引
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-20
********************************************************/
void SetWindow::on_cBoxLanguage_activated(int index)
{
    LanguageSel(index);
    UpdateControl();
    ui->cBoxLanguage->setCurrentIndex(index);
}

/********************************************************
 *@Name:        on_btnDebugOk_clicked
 *@Author:      HuaT
 *@Description: 调试界面登陆按钮
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-11-9
********************************************************/
void SetWindow::on_btnDebugOk_clicked()
{
    QString strInputPassword = ui->leDebugLoginPwd->text();
    QString strAuthPassword;
    QDate currentdate = QDate::currentDate();
    strAuthPassword = QString("1996%1").arg(currentdate.toString("MMdd"));
    //qDebug()<<strAuthPassword;
    if(strInputPassword.compare("qt200") == 0){
        HideDebugLogin(true);
        //m_QueryWin->SetExportHide(false);
        //显示升级按钮
        ui->btnSoftUpdate->setHidden(false);
        ui->btnHardUpdate->setHidden(false);

        ui->btnDebugLogout->setHidden(false);
    }else if(strInputPassword.compare(strAuthPassword) == 0){
        HideDebugFunc(false);
        HideDebugLogin(true);
        m_QueryWin->SetExportHide(false);
    }else{
        QString strTilte = QObject::tr("Note");
        QString strContain = QObject::tr("longin failed");
        QMessageBox::information(this,strTilte,strContain,QMessageBox::Ok);
    }
    ui->leDebugLoginPwd->setText("");
}

/********************************************************
 *@Name:        HideDebugFunc
 *@Author:      HuaT
 *@Description: 隐藏调试界面的功能按钮
 *@Param:       是否隐藏
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-11-9
********************************************************/
void SetWindow::HideDebugFunc(bool bHide)
{
    ui->btnDebugInterface->setHidden(bHide);
    ui->btnDebugLogout->setHidden(bHide);
    ui->btnSoftUpdate->setHidden(bHide);
    ui->btnHardUpdate->setHidden(bHide);
    ui->btnRFCDelete->setHidden(bHide);

    //数据库修改删除按钮
    m_QueryWin->SetDbModifyFuncHide(bHide);
}

/********************************************************
 *@Name:        HideDebugLogin
 *@Author:      HuaT
 *@Description: 隐藏调试界面的登陆功能
 *@Param:       是否隐藏
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-11-9
********************************************************/
void SetWindow::HideDebugLogin(bool bHide)
{
    ui->lbDebugLoginPwd->setHidden(bHide);
    ui->leDebugLoginPwd->setHidden(bHide);
    ui->btnDebugOk->setHidden(bHide);
}


/********************************************************
 *@Name:        InitHardVersion
 *@Author:      HuaT
 *@Description: 初始化硬件信息版本
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-1-18
********************************************************/
void SetWindow::UpdateHardVersion(QString strVersion)
{
    ui->lbHardNum->setText(strVersion);
}


/********************************************************
 *@Name:        on_btnDebugLogout_clicked
 *@Author:      HuaT
 *@Description: 注销登陆
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-3-15
 *@Modify:      1、添加退出时清空密码登陆框内容
********************************************************/
void SetWindow::on_btnDebugLogout_clicked()
{
    HideDebugFunc(true);
    HideDebugLogin(false);
    m_QueryWin->SetExportHide(true);
    ui->leDebugLoginPwd->clear();
}



/********************************************************
 *@Name:        on_btnDebugInterface_clicked
 *@Author:      HuaT
 *@Description: 调试接口按钮
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-1-11
********************************************************/
void SetWindow::on_btnDebugInterface_clicked()
{
    UserInterface* ui = new UserInterface(this,m_db,m_settings,m_RFSerialPort,m_HL7SerialPort);
    ui->show();
#ifdef Q_OS_LINUX
    ui->SetCanBusObj(&m_TestWin->GetCanBus());
#endif
    connect(ui,SIGNAL(StopAgingTest()), m_TestWin,SLOT(StopAgingTimer()) );
}

/********************************************************
 *@Name:        on_btnRFCQuery_clicked
 *@Author:      HuaT
 *@Description: 射频界面查询按钮
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-5-3
********************************************************/
void SetWindow::on_btnRFCQuery_clicked()
{
    ui->twRFCardInfo->setRowCount(0);

    //"Item","Batch No","Card No","Expiry Date"
    QString strCMD;
    strCMD = QString("select * from idcard order by inputtime desc");
    QSqlQuery* query = m_db->GetSqlQuery();
    query->exec(strCMD);
    int nRowCount = 0;
    int nNumber = 1;
    while(query->next()){
        nRowCount = ui->twRFCardInfo->rowCount();
        ui->twRFCardInfo->insertRow(nRowCount);
        //序号
        //QTableWidgetItem* item1 = new QTableWidgetItem;
        //item1->setText(QString::number(nNumber));
        //ui->twRFCardInfo->setItem(nRowCount,0,item1);
        //项目
        QTableWidgetItem* item2 = new QTableWidgetItem;
        item2->setText(query->value(3).toString());
        ui->twRFCardInfo->setItem(nRowCount,0,item2);
        //批号
        QTableWidgetItem* item3 = new QTableWidgetItem;
        item3->setText(query->value(4).toString());
        ui->twRFCardInfo->setItem(nRowCount,1,item3);
        //卡号
        QTableWidgetItem* item4 = new QTableWidgetItem;
        item4->setText(query->value(5).toString());
        ui->twRFCardInfo->setItem(nRowCount,2,item4);
        //过期时间
        QTableWidgetItem* item5 = new QTableWidgetItem;
        item5->setText(query->value(6).toString());
        ui->twRFCardInfo->setItem(nRowCount,3,item5);
        //条码号
        QTableWidgetItem* item6 = new QTableWidgetItem;
        item6->setText(query->value(2).toString());
        ui->twRFCardInfo->setItem(nRowCount,4,item6);
        nNumber++;
    }
}


/********************************************************
 *@Name:        on_btnRFCDelete_clicked
 *@Author:      HuaT
 *@Description: 射频界面删除按钮
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-8-7
********************************************************/
void SetWindow::on_btnRFCDelete_clicked()
{
    QString Title = QObject::tr("Note");
    QString Msg = QObject::tr("Please choose the data to delete");
    int CurrentRow = ui->twRFCardInfo->currentRow();
    if(CurrentRow == -1){
        QMessageBox::information(this,Title,Msg,QMessageBox::Ok);
        return;
    }
    QString strTitle,strContent;
    strTitle = QObject::tr("Note");
    strContent = QObject::tr("Confirmed will have to delete?");

    if(QMessageBox::Yes == QMessageBox::information(this,strTitle,strContent,QMessageBox::Yes | QMessageBox::No)){
        QString strID = ui->twRFCardInfo->item(CurrentRow,4)->text();
        QString strCMD;
        strCMD = QString("delete from idcard where barcode='%1'").arg(strID);
        if(!m_db->Exec(strCMD)){
            QString Msg = QObject::tr("Delete failed");
            QMessageBox::information(this,Title,Msg,QMessageBox::Ok);
        }else{
            ui->twRFCardInfo->removeRow(CurrentRow);
            QString Msg = QObject::tr("deleted successfully");
            QMessageBox::information(this,Title,Msg,QMessageBox::Ok);
        }
    }

}

/********************************************************
 *@Name:        on_btnDeviceIDSave_clicked
 *@Author:      HuaT
 *@Description: 系统信息界面删除按钮
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-1-24
********************************************************/
void SetWindow::on_btnDeviceIDSave_clicked()
{
    m_settings->SetParam(DEVICEID,ui->leDeviceID->text());
    m_settings->WriteSettingsInfoToMap();
    QString Title = QObject::tr("Note");
    QString Msg = QObject::tr("Configuration saved successfully");
    QMessageBox::information(this,Title,Msg,QMessageBox::Ok);
}
