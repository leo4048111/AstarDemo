#ifndef DRAWING_H
#define DRAWING_H

#include <QWidget>
#include "astar.h"

namespace Ui {
class Drawing;
}

class Drawing : public QWidget
{
    Q_OBJECT

public:
    explicit Drawing(QWidget *parent = nullptr);
    ~Drawing();

public slots:
    void slot_drawSearchTree(Astar::LPNode root);

private:
    Ui::Drawing *ui;

private:
    void paintNode(const Astar::LPNode node, const int x, const int y);
    void paintConnector(const QRect rect);

protected:
    void paintEvent(QPaintEvent *e);
};

#endif // DRAWING_H
