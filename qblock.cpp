#include "qblock.h"

QBlock::QBlock(QWidget *parent) : QPushButton(parent)
{

}

void QBlock::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
        emit this->signal_blockRightPressed();

    if(event->button() == Qt::LeftButton)
        emit this->signal_blockLeftPressed();
}
