#ifndef DRAWING_H
#define DRAWING_H

#include <QWidget>
#include <QSlider>
#include <map>
#include <vector>
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

private:
    typedef struct _DrawNodeWrapper
    {
        Astar::LPNode node{nullptr};
        int depth{NULL};
        int midX{NULL};
        int midY{NULL};

        _DrawNodeWrapper(Astar::LPNode node, int depth, int midX, int midY)
        {
            this->node = node;
            this->depth = depth;
            this->midX = midX;
            this->midY = midY;
        }
    }DrawNodeWrapper, *LPDrawNodeWrapper;

public slots:
    void slot_drawSearchTree(Astar::LPNode root);
    void slot_valueChanged(int value);

private:
    Ui::Drawing *ui;

    std::map<QSlider*, int> mapSliderDepth;

    std::vector<int> g_drawLayout;

    std::vector<QSlider*> vecSlider;

private:
    void paintNode(const Astar::LPNode node, const int x, const int y);
    void paintConnector(int startX, int startY, int endX, int endY, bool isRoute);


protected:
    void paintEvent(QPaintEvent *e);
    void closeEvent(QCloseEvent* e);
};

#endif // DRAWING_H
