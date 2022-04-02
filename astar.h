#ifndef ASTAR_H
#define ASTAR_H

#include <iostream>
#include <cstdint>
#include <vector>
#include <queue>
#include <stack>

#include "log.h"


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

        _Node(_Node* parent, uint64_t state, uint32_t cost, uint32_t estimation)
        {
            this->parent = parent;
            this->state = state;
            this->cost = cost;
            this->estimation = estimation;
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

    //启发函数
    uint32_t estimate(const uint64_t state, const uint64_t dst)
    {
        uint64_t tmp = state ^ dst;
        tmp = (tmp & 0x5555555555555555) + ((tmp >> 1) & 0x5555555555555555);
        tmp = (tmp & 0x3333333333333333) + ((tmp >> 2) & 0x3333333333333333);
        tmp = (tmp & 0x0F0F0F0F0F0F0F0F) + ((tmp >> 4) & 0x0F0F0F0F0F0F0F0F);
        tmp = (tmp & 0x00FF00FF00FF00FF) + ((tmp >> 8) & 0x00FF00FF00FF00FF);
        tmp = (tmp & 0x0000FFFF0000FFFF) + ((tmp >> 16) & 0x0000FFFF0000FFFF);
        tmp = (tmp & 0x00000000FFFFFFFF) + ((tmp >> 32) & 0X00000000FFFFFFFF);

        return (uint32_t)tmp / 8;
    }

    //启发函数2
    uint32_t calDifference(const uint64_t state, const uint64_t dst)
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

    //移动方向
    enum class Dir
    {
        up = 0,
        down,
        left,
        right
    };

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

#define INSERT_IF_VALID(parent,s, dir)  \
{\
    if(s != NULL) \
    {\
        LPNode n = new Node(parent, s, cost, estimate(s, dst));  \
        open.push(NodeWrapper{ n });  \
        if (parent != NULL)   \
        { \
            parent->children[dir] = n; \
        }\
    }\
}

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

#define COST_LIMIT 3000000

    std::vector<std::vector<uint64_t>> run(const uint64_t src, const uint64_t dst)
    {
        std::vector<std::vector<uint64_t>> ret;
        if (!validate(src, dst))
        {
            LOG("Can't solve.", Log::LogType::runtime);
            return ret;
        }

        uint32_t cost = 0;
        std::priority_queue<NodeWrapper, std::vector<NodeWrapper>, std::greater<NodeWrapper>> open;
        std::vector<NodeWrapper> close;
        LPNode root = new Node(nullptr, NULL, NULL, NULL);
        INSERT_IF_VALID(root, src, NULL);
        while (!open.empty())
        {
            NodeWrapper cur = open.top();
            open.pop();

            if (cur.node->state == dst)
            {
                LOG("Route is found at cost of " + QString::number(cost), Log::LogType::runtime);
                std::stack<LPNode> s;
                for (LPNode n = cur.node; n->parent != nullptr; n = n->parent)
                    s.push(n);
                while (!s.empty())
                {
                    ret.push_back(toVector(s.top()->state));
                    s.pop();
                }
                break;
            };

            if(cost > COST_LIMIT)
            {
                LOG("Cost exceeds limit, aborting...", Log::LogType::runtime);
                break;
            }

            uint64_t nextState = NULL;
            nextState = move(cur.node->state, Dir::up);
            INSERT_IF_VALID(cur.node, nextState, (uint32_t)Dir::up);
            nextState = move(cur.node->state, Dir::down);
            INSERT_IF_VALID(cur.node, nextState, (uint32_t)Dir::down);
            nextState = move(cur.node->state, Dir::left);
            INSERT_IF_VALID(cur.node, nextState, (uint32_t)Dir::left);
            nextState = move(cur.node->state, Dir::right);
            INSERT_IF_VALID(cur.node, nextState, (uint32_t)Dir::right);
            close.push_back(cur);
            cost++;
        }

        deleteTree(root);
        return ret;
    }

}

#endif
