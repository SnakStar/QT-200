#ifndef RESULTMODIFY_H
#define RESULTMODIFY_H

#include <QDialog>
#include<QMessageBox>
#include"cqtprodb.h"
#include"cutilsettings.h"
#include"querywindow.h"

namespace Ui {
class ResultModify;
}

class ResultModify : public QDialog
{
    Q_OBJECT
    
public:
    explicit ResultModify(QWidget *parent = 0);
    ResultModify(CQtProDB* db,QString strNumber,QString strTestDate
                          ,QString strResult,QWidget *parent = 0);
    ~ResultModify();
    
private slots:
    void on_pbRMOK_clicked();

private:
    Ui::ResultModify *ui;
    CQtProDB* m_db;
    CUtilSettings m_Settings;
};

#endif // RESULTMODIFY_H
