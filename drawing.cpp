#include "drawing.h"
#include "ui_drawing.h"

#include <QPainter>
#include <QPushButton>
#include <QFont>
#include <queue>

#include "astar.h"

const int g_drawLayout[6] = {800, 700, 600, 500, 400, 284};
static Astar::LPNode g_root = nullptr;
const int g_drawWindowWidth = 1920;
const int g_drawWindowHeight = 1080;
const int g_connectorHeight = 20;
const int g_nodeWrapperHeight = 140;
const int g_nodeWrapperWidth = 70;
const int g_stateTextHeight = g_nodeWrapperHeight/2;

Drawing::Drawing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Drawing)
{
    ui->setupUi(this);

    this->setMaximumSize(g_drawWindowWidth, g_drawWindowHeight);
    this->setMinimumSize(g_drawWindowWidth, g_drawWindowHeight);
}

void Drawing::slot_drawSearchTree(Astar::LPNode root)
{
    g_root = root;
    update();
    //Astar::deleteTree(root);
}

void Drawing::paintEvent(QPaintEvent *e)
{
    std::queue<DrawNodeWrapper> q;
    DrawNodeWrapper rootWrapper(g_root->children[0], 0, g_drawWindowWidth/2, 10);
    q.push(rootWrapper);
    while(!q.empty())
    {
        DrawNodeWrapper nodeWrapper = q.front();
        q.pop();
        paintNode(nodeWrapper.node, nodeWrapper.midX, nodeWrapper.midY);
        int childCnt = Astar::getChildCnt(nodeWrapper.node);
        int gap = (((childCnt - 1) * 2) ? g_drawLayout[nodeWrapper.depth] / ((childCnt - 1) * 2):0);
        int seq = 0;
        for(uint32_t i = 0;i<4;i++)
        {
            if(nodeWrapper.node->children[i] == NULL) continue;
            int offset = -(childCnt - 1) * gap + 2 * seq * gap;
            int childMidX = nodeWrapper.midX + offset;
            int childMidY = nodeWrapper.midY + g_nodeWrapperHeight + g_connectorHeight;
            paintConnector(nodeWrapper.midX,  nodeWrapper.midY +  g_nodeWrapperHeight, childMidX, childMidY, nodeWrapper.node->isRoute & nodeWrapper.node->children[i]->isRoute);
            DrawNodeWrapper childNodeWrapper(nodeWrapper.node->children[i], nodeWrapper.depth+1, childMidX, childMidY);
            q.push(childNodeWrapper);
            seq++;
        }
    }

    //Astar::deleteTree(g_root);
}

void Drawing::paintNode(const Astar::LPNode node, const int midX, const int midY)
{
    if(node == NULL) return;

    int x = midX - g_nodeWrapperWidth/2;
    int y = midY;
    QString nodeState = QString::fromStdString(Astar::toString(node->state));
    QString nodeInfo = QString::fromStdString(Astar::getNodeInfo(node));

    //painters
    QPainter bgPainter(this);
    QPainter stateTextPainter(this);
    QPainter stateCostPainter(this);
    //brushes
    QBrush transparentBrush(Qt::transparent);
    QBrush bgBrush(Qt::gray);

    //pens
    QPen rectOutlinePen(Qt::DashDotDotLine);
    if(node->isRoute)
    {
        rectOutlinePen.setColor(Qt::red);
        rectOutlinePen.setWidth(5);
    }
    else
    {
        rectOutlinePen.setColor(Qt::black);
        rectOutlinePen.setWidth(3);
    }

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
    QRect stateTextWrapperRect(x, y, g_nodeWrapperWidth, g_stateTextHeight);
    QRect nodeWrapperRect(x, y,g_nodeWrapperWidth, g_nodeWrapperHeight);
    QRect nodeInfoRect(x, y + g_stateTextHeight, g_nodeWrapperWidth, g_nodeWrapperHeight - g_stateTextHeight);

    bgPainter.drawRect(stateTextWrapperRect);
    bgPainter.drawRect(nodeWrapperRect);
    bgPainter.drawRect(nodeInfoRect);
    stateTextPainter.drawText(stateTextWrapperRect, Qt::AlignHCenter | Qt::AlignVCenter, nodeState);
    stateTextPainter.drawText(nodeInfoRect, Qt::AlignHCenter | Qt::AlignVCenter, nodeInfo);
}

void Drawing::paintConnector(int startX, int startY, int endX, int endY, bool isRoute)
{
    QPainter painter(this);
    //pen
    QPen connectorPen(Qt::SolidLine);
    if(!isRoute)
    {
        connectorPen.setColor(Qt::black);
        connectorPen.setStyle(Qt::DashDotLine);
        connectorPen.setWidth(3);
    }
    else
    {
        connectorPen.setColor(Qt::red);
        connectorPen.setStyle(Qt::SolidLine);
        connectorPen.setWidth(5);
    }
    painter.setPen(connectorPen);

    QVector<QLine> vecLines;
    QLine line1(startX, startY, startX, startY + g_connectorHeight/2);
    QLine line2(startX, startY + g_connectorHeight/2, endX, startY + g_connectorHeight/2);
    QLine line3(endX, startY + g_connectorHeight/2, endX, endY);
    vecLines.push_back(line1);
    vecLines.push_back(line2);
    vecLines.push_back(line3);
    painter.drawLines(vecLines);
}

Drawing::~Drawing()
{
    delete ui;
}
