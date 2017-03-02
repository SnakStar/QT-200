#include "ccheckheaderview.h"



CheckBoxHeader::CheckBoxHeader(Qt::Orientation orientation, QWidget* parent /*= 0*/)
    : QHeaderView(orientation, parent)
{
    isChecked_ = false;
}

void CheckBoxHeader::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();
    if (logicalIndex == 0)
    {
        QStyleOptionButton option;

        option.rect = QRect(1,3,20,20);

        option.state = QStyle::State_Enabled | QStyle::State_Active;

        if (isChecked_)
            option.state |= QStyle::State_On;
        else
            option.state |= QStyle::State_Off;
        option.state |= QStyle::State_Off;

        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter);
    }
}

void CheckBoxHeader::mousePressEvent(QMouseEvent* event)
{
    setIsChecked(!isChecked());

    emit checkBoxClicked(isChecked());
}

void CheckBoxHeader::redrawCheckBox()
{
    viewport()->update();
}

void CheckBoxHeader::setIsChecked(bool val)
{
    if (isChecked_ != val)
    {
        isChecked_ = val;

        redrawCheckBox();
    }
}


void CheckBoxHeader::changeHeaderCheckBox(bool state)
{
    isChecked_ = state;
    redrawCheckBox();
}
