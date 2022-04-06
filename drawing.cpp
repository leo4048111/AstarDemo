#include "drawing.h"
#include "ui_drawing.h"

#include <QPainter>
#include <QPushButton>
#include <QFont>

#include "astar.h"


static Astar::LPNode g_root = nullptr;

Drawing::Drawing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Drawing)
{
    ui->setupUi(this);


}

void Drawing::slot_drawSearchTree(Astar::LPNode root)
{
    g_root = root->children[0];
    update();
}

void Drawing::paintEvent(QPaintEvent *e)
{
    paintNode(g_root, 100, 100);
}

void Drawing::paintNode(const Astar::LPNode node, const int x, const int y)
{
    if(node == NULL) return;

    const int nodeWrapperHeight = 70;
    const int nodeWrapperWidth = 150;
    const int stateTextWidth = 70;
    QString nodeState = QString::fromStdString(Astar::toString(node->state));
    QString nodeInfo = QString::fromStdString(Astar::getNodeInfo(node));

    //painters
    QPainter bgPainter(this);
    QPainter stateTextPainter(this);
    QPainter stateCostPainter(this);
    //brushes
    QBrush transparentBrush(Qt::transparent);

    //pens
    QPen rectOutlinePen(Qt::SolidLine);
    rectOutlinePen.setColor(Qt::black);
    QPen stateTextPen(Qt::SolidLine);
    stateTextPen.setColor(Qt::black);

    //font
    QFont stateTextFont("Segoe UI Black", 10, QFont::Bold, false);

    //set utilities
    bgPainter.setBrush(transparentBrush);
    bgPainter.setPen(rectOutlinePen);
    stateTextPainter.setPen(stateTextPen);
    stateTextPainter.setFont(stateTextFont);

    //rect wrappers
    QRect stateTextWrapperRect(x, y, stateTextWidth, nodeWrapperHeight);
    QRect nodeWrapperRect(x, y,nodeWrapperWidth, nodeWrapperHeight);
    QRect nodeInfoRect(x + stateTextWidth, y,nodeWrapperWidth - stateTextWidth, nodeWrapperHeight);

    bgPainter.drawRect(stateTextWrapperRect);
    bgPainter.drawRect(nodeWrapperRect);
    bgPainter.drawRect(nodeInfoRect);
    stateTextPainter.drawText(stateTextWrapperRect, Qt::AlignHCenter | Qt::AlignVCenter, nodeState);
    stateTextPainter.drawText(nodeInfoRect, Qt::AlignHCenter | Qt::AlignVCenter, nodeInfo);
}

Drawing::~Drawing()
{
    delete ui;
}
