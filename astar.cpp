#include <iostream>
#include <cstdint>
#include <vector>
#include <queue>
#include <stack>
#include <map>

#include "log.h"
#include "astar.h"


/*
位置排布
| 0  1  2 |
| 3  4  5 |
| 6  7  8 |

---->>  0x012345678
*/

namespace Astar
{
    uint32_t abs(uint32_t num)
    {
        return (num >= 0) ? num:-num;
    }

    //启发函数1:两个状态数码中不同的位数（二进制）
    uint32_t estimate(const uint64_t state, const uint64_t dst)
    {
        uint64_t tmp = state ^ dst;
        tmp = (tmp & 0x5555555555555555) + ((tmp >> 1) & 0x5555555555555555);
        tmp = (tmp & 0x3333333333333333) + ((tmp >> 2) & 0x3333333333333333);
        tmp = (tmp & 0x0F0F0F0F0F0F0F0F) + ((tmp >> 4) & 0x0F0F0F0F0F0F0F0F);
        tmp = (tmp & 0x00FF00FF00FF00FF) + ((tmp >> 8) & 0x00FF00FF00FF00FF);
        tmp = (tmp & 0x0000FFFF0000FFFF) + ((tmp >> 16) & 0x0000FFFF0000FFFF);
        tmp = (tmp & 0x00000000FFFFFFFF) + ((tmp >> 32) & 0X00000000FFFFFFFF);

        return (uint32_t)tmp / 4;
    }

    //启发函数2：棋盘中相同位置不同数码的个数
    uint32_t estimate2(const uint64_t state, const uint64_t dst)
    {
        uint64_t mask = 0xF00000000;
        uint32_t ret = 0;

        for (uint32_t i = 0; i < 9; i++)
        {
            if ((state & mask) != (dst & mask)) ret++;
            mask >>= 4;
        }
        return ret;
    }

    //启发函数3：两个棋盘相同数码的曼哈顿距离
    uint32_t estimate3(const uint64_t state, const uint64_t dst)
    {
        uint32_t ret = 0;
        uint64_t mask = 0xF00000000;

        for(uint32_t i = 0; i < 9; i++)
        {
            uint64_t num = ((state & mask) >> i);
            uint64_t mask2 = 0xF00000000;
            uint32_t j = 0;
            for(j; j < 9; j++)
            {
                uint64_t num2 = ((state & mask2) >> j);
                if(num2 == num) break;
                mask2 >>=4;
            }
            ret += abs((i/3) - (j/3)) + abs((i % 3) + (j % 3));
            mask >>=4;
        }
        return ret;
    }

    //启发函数4：两个棋盘拉平后相同数码的线性距离
    uint32_t estimate4(const uint64_t state, const uint64_t dst)
    {
        uint32_t ret = 0;
        uint64_t mask = 0xF00000000;

        for(uint32_t i = 0; i < 9; i++)
        {
            uint64_t num = ((state & mask) >> i);
            uint64_t mask2 = 0xF00000000;
            uint32_t j = 0;
            for(j; j < 9; j++)
            {
                uint64_t num2 = ((state & mask2) >> j);
                if(num2 == num) break;
                mask2 >>=4;
            }
            ret += abs(i - j);
            mask >>=4;
        }
        return ret;
    }

    //打印状态
    void printState(const uint64_t state)
    {
        uint64_t mask = 0xF00000000;
        for (int32_t i = 8; i >= 0; i--)
        {
            std::cout << ((state & mask) >> (i * 4)) << ' ';
            mask >>= 4;
            if (i % 3 == 0) std::cout << std::endl;
        }

        std::cout << std::endl;
    }

    //找到空格的位置
    uint32_t findBlank(const uint64_t state)
    {
        uint64_t mask = 0xF00000000;
        int32_t i = 0;
        for (i = 0; i < 9; i++)
        {
            if (!(state & mask))
                return i;
            mask >>= 4;
        }

        return -1;
    }

    //交换两个数
    uint64_t swap(const uint64_t state, const uint32_t pos1, const uint32_t pos2)
    {
        uint64_t mask1 = 0xF00000000;
        uint64_t mask2 = 0xF00000000;

        mask1 >>= (pos1 * 4);
        mask2 >>= (pos2 * 4);

        uint64_t num1 = state & mask1;
        uint64_t num2 = state & mask2;

        if (pos1 > pos2)
        {
            num1 <<= (4 * (pos1 - pos2));
            num2 >>= (4 * (pos1 - pos2));
        }
        else
        {
            num1 >>= (4 * (pos2 - pos1));
            num2 <<= (4 * (pos2 - pos1));
        }
        return ((state & ~mask1) & ~mask2) | num1 | num2;
    }

    //向指定方位移动空格
    uint64_t move(const uint64_t state, const Dir dir)
    {
        int32_t blank = findBlank(state);
        switch (dir)
        {
        case Dir::up:
        {
            if (blank == 0 || blank == 1 || blank == 2) return NULL;
            return swap(state, blank, blank - 3);
        }

        case Dir::down:
        {
            if (blank == 6 || blank == 7 || blank == 8) return NULL;
            return swap(state, blank, blank + 3);
        }

        case Dir::left:
        {
            if (blank == 0 || blank == 3 || blank == 6) return NULL;
            return swap(state, blank, blank - 1);
        }

        case Dir::right:
        {
            if (blank == 2 || blank == 5 || blank == 8) return NULL;
            return swap(state, blank, blank + 1);
        }

        default:
            return NULL;
        }

        return NULL;
    }

    //验证状态合法性
    bool validate(const uint64_t src, const uint64_t dst)
    {
        uint32_t revCnt1 = 0;
        for (uint32_t i = 0; i < 9; i++)
        {
            uint64_t mask = 0xF00000000 >> (i * 4);
            uint64_t curNum = (src & mask) >> ((8 - i) * 4);
            for (uint32_t j = i + 1; j < 9; j++)
            {
                uint64_t mask2 = 0xF00000000 >> (j * 4);
                uint64_t cmpNum = (src & mask2) >> ((8 - j) * 4);
                if (cmpNum == curNum) return false;

                if (!cmpNum || !curNum) continue;

                if (cmpNum < curNum) revCnt1++;
            }
        }

        uint32_t revCnt2 = 0;
        for (uint32_t i = 0; i < 9; i++)
        {
            uint64_t mask = 0xF00000000 >> (i * 4);
            uint64_t curNum = (dst & mask) >> ((8 - i) * 4);
            for (uint32_t j = i + 1; j < 9; j++)
            {
                uint64_t mask2 = 0xF00000000 >> (j * 4);
                uint64_t cmpNum = (dst & mask2) >> ((8 - j) * 4);

                if (cmpNum == curNum) return false;

                if (!cmpNum || !curNum) continue;

                if (cmpNum < curNum) revCnt2++;
            }
        }

        if ((revCnt1 & 0x01) != (revCnt2 & 0x01)) return false;

        return true;
    }

    //状态数组转位表示
    uint64_t toBits(const std::vector<uint64_t> arr)
    {
        if(arr.size() < 9) return NULL;
        uint64_t ret = 0;
        for(uint32_t i = 0;i < 9; i++)
        {
            ret |= (arr[i] << ((8 - i) * 4));
        }

        return ret;
    }

    //位表示转状态数组
    std::vector<uint64_t> toVector(const uint64_t state)
    {
        uint64_t mask = 0xF00000000;
        std::vector<uint64_t> ret;
        for(uint32_t i =0; i < 9; i++)
        {
            uint64_t num = (state & mask) >> (4 * (8 - i));
            ret.push_back(num);
            mask >>= 4;
        }
        return ret;
    }

    //删除搜索树
    void deleteTree(LPNode& root)
    {
        for(uint32_t i = 0;i<4;i++)
        {
            if(root->children[i] != NULL)
                deleteTree(root->children[i]);
        }

        delete root;
        root = NULL;
    }

    std::string getNodeInfo(const LPNode node)
    {
        std::string ret = "";
        if(node == NULL) return ret;
        ret += "g(n) = ";
        ret += std::to_string(node->cost);
        ret += '\n';
        ret += "h(n) = ";
        ret += std::to_string(node->estimation);
        ret += '\n';
        ret += "f(n) = ";
        ret += std::to_string(node->cost + node->estimation);

        return ret;
    }

//最大耗散
#define COST_LIMIT 4000000

//树结点生成宏
#define INSERT_IF_VALID(parent,s, dir, method, cost)  \
{\
    if(s != NULL && (closeMap.find(s) == closeMap.end())) \
    {\
        LPNode n = new Node(parent, s, cost, method(s, dst), false);  \
        open.push(NodeWrapper{ n });  \
        if (parent != NULL)   \
        { \
            parent->children[dir] = n; \
        }\
    }\
}

using FuncpEstimateMethod = uint32_t(*)(const uint64_t, const uint64_t);

    RunResult run(const uint64_t src, const uint64_t dst, const Astar::EstimateMethod method)
    {
        RunResult ret;
        if (!validate(src, dst))
        {
            LOG("Can't solve.", Log::LogType::runtime);
            return ret;
        }
        uint32_t resultCost = 0; //最终路径耗散
        std::priority_queue<NodeWrapper, std::vector<NodeWrapper>, std::greater<NodeWrapper>> open;   //优先队列
        std::map<uint64_t, bool> closeMap;   //close表，防止重复访问相同局面
        LPNode root = new Node(nullptr, NULL, NULL, NULL, false);    //初始化树根
        FuncpEstimateMethod pEstimateMethod = nullptr;       //使用的启发函数
        switch(method)
        {
        case Astar::EstimateMethod::DIF_BITS:
            pEstimateMethod = &estimate;
            break;
        case Astar::EstimateMethod::DIF_NUMS:
            pEstimateMethod = &estimate2;
            break;
        case Astar::EstimateMethod::MANHATTAN_DISTANCE:
            pEstimateMethod = &estimate3;
            break;
        case Astar::EstimateMethod::HORIZONTAL_DISTANCE:
            pEstimateMethod = &estimate4;
            break;
        }

        INSERT_IF_VALID(root, src, NULL, pEstimateMethod, 0);
        while (!open.empty())     //算法运行
        {
            NodeWrapper cur = open.top();
            open.pop();

            if (cur.node->state == dst)    //找到目标状态
            {
                LOG("Route is found at cost of " + QString::number(resultCost), Log::LogType::runtime);
                std::stack<LPNode> s;
                for (LPNode n = cur.node; n->parent != nullptr; n = n->parent)
                    s.push(n);
                while (!s.empty())
                {
                    ret.route.push_back(toVector(s.top()->state));
                    s.pop();
                }
                break;
            };

            if(cur.node->cost > COST_LIMIT)      //耗散过大
            {
                LOG("Cost exceeds limit, aborting...", Log::LogType::runtime);
                break;
            }

            uint64_t nextState = NULL;
            nextState = move(cur.node->state, Dir::up);
            INSERT_IF_VALID(cur.node, nextState, (uint32_t)Dir::up, pEstimateMethod, cur.node->cost+1);
            nextState = move(cur.node->state, Dir::down);
            INSERT_IF_VALID(cur.node, nextState, (uint32_t)Dir::down, pEstimateMethod, cur.node->cost+1);
            nextState = move(cur.node->state, Dir::left);
            INSERT_IF_VALID(cur.node, nextState, (uint32_t)Dir::left, pEstimateMethod, cur.node->cost+1);
            nextState = move(cur.node->state, Dir::right);
            INSERT_IF_VALID(cur.node, nextState, (uint32_t)Dir::right, pEstimateMethod, cur.node->cost+1);
            closeMap.insert(std::make_pair(cur.node->state, true));

            resultCost++;
        }

        ret.cost = resultCost;
        deleteTree(root);
        return ret;
    }

    std::string toString(const uint64_t state)
    {
        std::string ret;
        uint64_t mask = 0xF00000000;
        for(uint32_t i = 0;i<9;i++)
        {
            uint64_t num = (state & mask) >> ((8 - i) * 4);
            ret += (num + '0' - 0);
            mask >>= 4;
            if(i == 2 || i == 5) ret += '\n';
            else if(i != 8) ret += "  ";
        }

        return ret;
    }

    uint32_t getChildCnt(const LPNode node)
    {
        uint32_t ret = 0;
        for(uint32_t i = 0;i<4;i++)
            if(node->children[i] != NULL) ret++;

        return ret;
    }

    LPNode runDrawing(const uint64_t src, const uint64_t dst, const Astar::EstimateMethod method)
    {
        std::vector<std::vector<uint64_t>> ret;
        uint32_t searchCost = 0;    //搜索耗散值
        std::priority_queue<NodeWrapper, std::vector<NodeWrapper>, std::greater<NodeWrapper>> open;   //优先队列
        std::map<uint64_t, bool> closeMap;   //close表，防止重复访问相同局面
        LPNode root = new Node(nullptr, NULL, NULL, NULL, false);    //初始化树根
        FuncpEstimateMethod pEstimateMethod = nullptr;       //使用的启发函数
        switch(method)
        {
        case Astar::EstimateMethod::DIF_BITS:
            pEstimateMethod = &estimate;
            break;
        case Astar::EstimateMethod::DIF_NUMS:
            pEstimateMethod = &estimate2;
            break;
        case Astar::EstimateMethod::MANHATTAN_DISTANCE:
            pEstimateMethod = &estimate3;
            break;
        case Astar::EstimateMethod::HORIZONTAL_DISTANCE:
            pEstimateMethod = &estimate4;
            break;
        }

        INSERT_IF_VALID(root, src, NULL, pEstimateMethod, 0);
        while (!open.empty())     //算法运行
        {
            NodeWrapper cur = open.top();
            open.pop();

            if (cur.node->state == dst)    //找到目标状态
            {
                for (LPNode n = cur.node; n->parent != nullptr; n = n->parent)
                    n->isRoute = true;
                break;
            };

            if(cur.node->cost > COST_LIMIT)      //耗散过大
            {
                LOG("Cost exceeds limit, aborting...", Log::LogType::runtime);
                break;
            }

            uint64_t nextState = NULL;
            nextState = move(cur.node->state, Dir::up);
            INSERT_IF_VALID(cur.node, nextState, (uint32_t)Dir::up, pEstimateMethod, cur.node->cost+1);
            nextState = move(cur.node->state, Dir::down);
            INSERT_IF_VALID(cur.node, nextState, (uint32_t)Dir::down, pEstimateMethod, cur.node->cost+1);
            nextState = move(cur.node->state, Dir::left);
            INSERT_IF_VALID(cur.node, nextState, (uint32_t)Dir::left, pEstimateMethod, cur.node->cost+1);
            nextState = move(cur.node->state, Dir::right);
            INSERT_IF_VALID(cur.node, nextState, (uint32_t)Dir::right, pEstimateMethod, cur.node->cost+1);
            closeMap.insert(std::make_pair(cur.node->state, true));
        }

        return root;
    }
}

