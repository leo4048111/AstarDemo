#ifndef QBLOCK_H
#define QBLOCK_H

#include <QPushButton>
#include <QMouseEvent>

class QBlock : public QPushButton
{
    Q_OBJECT
public:
    explicit QBlock(QWidget *parent = 0);

private slots:
    void mousePressEvent(QMouseEvent* event);

signals:
    void signal_blockRightPressed();
    void signal_blockLeftPressed();

};

#endif // QBLOCK_H
