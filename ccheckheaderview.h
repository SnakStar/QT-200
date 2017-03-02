#ifndef CCHECKHEADERVIEW_H
#define CCHECKHEADERVIEW_H

#include<QHeaderView>
#include<QPainter>


class CheckBoxHeader : public QHeaderView
{
    Q_OBJECT

public:
    CheckBoxHeader(Qt::Orientation orientation, QWidget* parent = 0);

    bool isChecked() const { return isChecked_; }
    void setIsChecked(bool val);

signals:
    void checkBoxClicked(bool state);
private slots:
    void changeHeaderCheckBox(bool state);

protected:
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const;

    void mousePressEvent(QMouseEvent* event);

private:
    bool isChecked_;

    void redrawCheckBox();
};

#endif // CCHECKHEADERVIEW_H
