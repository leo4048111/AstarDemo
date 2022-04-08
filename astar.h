#ifndef ASTAR_H
#define ASTAR_H

#include <iostream>
#include <vector>
/*
位置排布
| 0  1  2 |
| 3  4  5 |
| 6  7  8 |

---->>  0x012345678
*/

namespace Astar
{
    typedef struct _Node   //表结点结构
    {
        _Node* parent;
        _Node* children[4]{ NULL, NULL, NULL, NULL };
        uint64_t state;   //当前状态
        uint32_t cost;    //路径耗散
        uint32_t estimation;   //启发值
        bool isRoute;    //该结点是否在最优路径上

        _Node(_Node* parent, uint64_t state, uint32_t cost, uint32_t estimation, bool isRoute)
        {
            this->parent = parent;
            this->state = state;
            this->cost = cost;
            this->estimation = estimation;
            this->isRoute = isRoute;
        }

    }Node, *LPNode;

    typedef struct _NodeWrapper
    {
        LPNode node;

        bool operator < (const _NodeWrapper n) const
        {
            return (this->node->cost + this->node->estimation) < (n.node->cost + n.node->estimation);
        }

        bool operator > (const _NodeWrapper n) const
        {
            return (this->node->cost + this->node->estimation) > (n.node->cost + n.node->estimation);
        }

    }NodeWrapper;

    enum class EstimateMethod
    {
        DIF_BITS = 0,
        DIF_NUMS,
        MANHATTAN_DISTANCE,
        HORIZONTAL_DISTANCE,
        NO_ESTIMATION
    };

    //移动方向
    enum class Dir
    {
        up = 0,
        down,
        left,
        right
    };

    typedef struct _RunResult
    {
        std::vector<std::vector<uint64_t>> route;
        uint32_t cost;
    }RunResult;

    uint32_t abs(uint32_t num);

    //启发函数1:两个状态数码中不同的位数（二进制）
    uint32_t estimate(const uint64_t state, const uint64_t dst);

    //启发函数2：棋盘中相同位置不同数码的个数
    uint32_t estimate2(const uint64_t state, const uint64_t dst);

    //启发函数3：两个棋盘相同数码的曼哈顿距离
    uint32_t estimate3(const uint64_t state, const uint64_t dst);

    //启发函数4：两个棋盘拉平后相同数码的线性距离
    uint32_t estimate4(const uint64_t state, const uint64_t dst);

    //无启发函数
    uint32_t estimate5(const uint64_t state, const uint64_t dst);

    //打印状态
    void printState(const uint64_t state);

    //找到空格的位置
    uint32_t findBlank(const uint64_t state);

    //交换两个数
    uint64_t swap(const uint64_t state, const uint32_t pos1, const uint32_t pos2);

    //向指定方位移动空格
    uint64_t move(const uint64_t state, const Dir dir);

    //验证状态合法性
    bool validate(const uint64_t src, const uint64_t dst);

    //状态数组转位表示
    uint64_t toBits(const std::vector<uint64_t> arr);

    //位表示转状态数组
    std::vector<uint64_t> toVector(const uint64_t state);

    //删除搜索树
    void deleteTree(LPNode& root);

    //返回节点孩子个数
    uint32_t getChildCnt(const LPNode node);

    RunResult run(const uint64_t src, const uint64_t dst, const Astar::EstimateMethod method);

    LPNode runDrawing(const uint64_t src, const uint64_t dst, const Astar::EstimateMethod method);

    //状态转字符串表示
    std::string toString(const uint64_t state);

    //节点信息格式化
    std::string getNodeInfo(const LPNode node);

    //获取搜索树深度
    uint32_t getTreeDepth(const LPNode root);
}

#endif
