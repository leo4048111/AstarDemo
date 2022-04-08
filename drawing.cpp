#include "drawing.h"
#include "ui_drawing.h"

#include <QPainter>
#include <QPushButton>
#include <QSlider>
#include <QFont>
#include <queue>

#include "astar.h"

static Astar::LPNode g_root = nullptr;
const int g_drawWindowWidth = 1920;
const int g_drawWindowHeight = 1080;
const int g_connectorHeight = 30;
const int g_nodeWrapperHeight = 140;
const int g_nodeWrapperWidth = 70;
const int g_stateTextHeight = g_nodeWrapperHeight/2;

Drawing::Drawing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Drawing)
{
    ui->setupUi(this);

    //set window title
    this->setWindowTitle("Search Tree Demo");

    this->setMaximumSize(99999, 99999);
    this->setMinimumSize(g_drawWindowWidth, g_drawWindowHeight);
}

void Drawing::slot_drawSearchTree(Astar::LPNode root)
{
    g_root = root;
    uint32_t depth = Astar::getTreeDepth(g_root->children[0]);
    for(uint32_t i = 0;i < depth - 1;i++)
    {
        auto s = new QSlider();
        s->setOrientation(Qt::Orientation::Horizontal);
        ui->vLayout->addWidget(s);
        vecSlider.push_back(s);
        mapSliderDepth.insert(std::make_pair(s, i));
        s->setValue((depth - 1 - i) * 10);
        g_drawLayout.push_back(g_drawWindowWidth * s->value() / 100);
        connect(s, &QSlider::valueChanged, this, &Drawing::slot_valueChanged);
    }

    update();
}

void Drawing::slot_valueChanged(int value)
{
    QSlider* s = qobject_cast<QSlider*>(sender());
    int depth = (*(this->mapSliderDepth.find(s))).second;
    this->g_drawLayout[depth] = value/100.0f * g_drawWindowWidth;
    update();
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
        if(!childCnt) continue;
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
    QPainter painter(this);

    //brushes
    QBrush transparentBrush(Qt::transparent);
    QBrush bgBrush(Qt::gray);

    //pens
    QPen rectOutlinePen(Qt::SolidLine);
    if(node->isRoute)
    {
        rectOutlinePen.setColor(Qt::red);
        rectOutlinePen.setWidth(4);
    }
    else
    {
        rectOutlinePen.setColor(Qt::black);
        rectOutlinePen.setWidth(3);
    }

    QPen stateTextPen(Qt::SolidLine);
    if(node->isRoute)
    {
        stateTextPen.setColor(Qt::black);
        stateTextPen.setWidth(3);
    }
    else
    {
        stateTextPen.setColor(Qt::gray);
        stateTextPen.setWidth(2);
    }

    //font
    QFont stateTextFont("Segoe UI Black", 10, QFont::Bold, false);

    //rect wrappers
    QRect stateTextWrapperRect(x, y, g_nodeWrapperWidth, g_stateTextHeight);
    QRect nodeWrapperRect(x, y,g_nodeWrapperWidth, g_nodeWrapperHeight);
    QRect nodeInfoRect(x, y + g_stateTextHeight, g_nodeWrapperWidth, g_nodeWrapperHeight - g_stateTextHeight);

    //draw utils
    painter.setBrush(transparentBrush);
    painter.setPen(rectOutlinePen);
    painter.drawRect(stateTextWrapperRect);
    painter.drawRect(nodeWrapperRect);
    painter.drawRect(nodeInfoRect);

    painter.setPen(stateTextPen);
    painter.setFont(stateTextFont);
    painter.drawText(stateTextWrapperRect, Qt::AlignHCenter | Qt::AlignVCenter, nodeState);
    painter.drawText(nodeInfoRect, Qt::AlignHCenter | Qt::AlignVCenter, nodeInfo);
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

void Drawing::closeEvent(QCloseEvent* e)
{
    this->mapSliderDepth.clear();
    this->g_drawLayout.clear();
    for(auto s:this->vecSlider)
    {
        s->setParent(nullptr);
        this->ui->vLayout->removeWidget(s);
    }
    this->ui->vLayout->update();
    Astar::deleteTree(g_root);
    g_root = nullptr;
}

Drawing::~Drawing()
{
    delete ui;
}
