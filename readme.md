 

## A\*算法求解8数码问题 作者：同济大学20级计算机系 2050250 2022.4.8 ##

摘 要

A*算法是一种启发式路径搜索算法。这种算法通过一个启发函数得到当前节点的启发值，加上当前节点的路径耗散值作为当前节点的总耗散值，每次采用BFS的思路遍历所有叶节点，同时维护一个优先队列，从中取出总耗散值最低的节点进行了扩展。A*算法的设计同时融合了BFS和DFS的优势，既考虑到了从起点通过当前路线的代价，又不断地计算当前路线方向是否更加趋近终点的方向（保证不会搜索很多的图块），是一种静态路网中最有效的直接搜索算法。在本次实验中，本人就八数码问题求解的A*算法实现，进行一个整体思路阐述、算法设计和展示程序编写，在理论学习和实际操作中熟悉和掌握启发式搜索的策略定义、评价函数和算法过程，深入地理解和掌握了A*算法的一个求解过程和搜索顺序，对于人工智能搜索原理的认识达到了一个更加深入的程度。

 

**关键词****：**启发式搜索，八数码问题，A*算法



目 录

[1 实验概述... 1](#_Toc100350019)

[1.1    实验目的... 1](#_Toc100350020)

[1.1.1 A*算法... 1](#_Toc100350021)

[1.1.2 八数码问题... 1](#_Toc100350022)

[1.1.3 实验简述... 1](#_Toc100350023)

[1.2 实验内容... 1](#_Toc100350024)

[1.2.1 启发函数设计... 1](#_Toc100350025)

[1.2.2 结果比较与性能分析.. 2](#_Toc100350026)

[1.2.3 步骤可视化... 2](#_Toc100350027)

[1.2.4 流程可视化... 2](#_Toc100350028)

[1.3 本文所作的工作.. 2](#_Toc100350029)

[2 实验方案设计... 3](#_Toc100350030)

[2.1 总体设计思路与总体架构.. 3](#_Toc100350031)

[2.1.1 总体设计思路... 3](#_Toc100350032)

[2.1.2 总体架构... 3](#_Toc100350033)

[2.2 核心算法及基本原理.. 4](#_Toc100350034)

[2.2.1 A*算法的基本原理及算法的代码实现.. 4](#_Toc100350035)

[2.2.2 A*算法估值函数的代码实现.. 5](#_Toc100350036)

[2.2.3 数码状态转移算法实现... 5](#_Toc100350037)

[2.2.4 A*算法的路径存储.. 5](#_Toc100350038)

[2.2.5 A*算法的搜索树绘制... 6](#_Toc100350039)

[2.3 模块设计... 7](#_Toc100350040)

[2.3.1 A*算法实现模块... 7](#_Toc100350041)

[2.3.2 GUI模块... 7](#_Toc100350042)

[2.3.3 日志模块... 7](#_Toc100350043)

[2.3.4 绘图模块... 7](#_Toc100350044)

[2.3.5 模块关系... 7](#_Toc100350045)

[2.3.5 其他创新内容或优化算法.. 7](#_Toc100350046)

[3 实验过程... 8](#_Toc100350047)

[3.1 环境说明... 8](#_Toc100350048)

[3.1.1 操作系统... 8](#_Toc100350049)

[3.1.2 开发语言... 8](#_Toc100350050)

[3.1.3 开发环境... 8](#_Toc100350051)

[3.2 源代码文件和主要函数清单... 8](#_Toc100350052)

[3.2.1 源代码文件... 8](#_Toc100350053)

[3.2.2主要函数清单.. 8](#_Toc100350054)

[3.3 实验结果展示... 8](#_Toc100350055)

[3.3.1 算法性能评估... 8](#_Toc100350056)

[3.3.2 启发对于A*算法性能的影响... 10](#_Toc100350057)

[3.3.3 不同启发函数对于同一问题求解的搜索树展示与对比.. 10](#_Toc100350058)

[4  总结... 13](#_Toc100350059)

[4.1 实验中存在的问题及解决方案... 13](#_Toc100350060)

[4.2 心得体会... 13](#_Toc100350061)

[4.3 后续改进方向... 13](#_Toc100350062)

[4.4 源码... 13](#_Toc100350063)

[参考文献... 14](#_Toc100350064)



# 1 实验概述



## 1.1  实验目的

### 1.1.1 A*算法

A*算法是一种启发式搜索算法，A*算法是一种启发式路径搜索算法。这种算法通过一个启发函数得到当前节点的启发值，加上当前节点的路径耗散值作为当前节点的总耗散值，每次采用BFS的思路遍历所有叶节点，同时维护一个优先队列和一个哈希表，从中取出总耗散值最低的节点进行扩展，将所有未被访问过的状态加入队列。A*算法的设计同时融合了BFS和DFS的优势，既考虑到了从起点通过当前路线的代价，又不断地计算当前路线方向是否更加趋近终点的方向（保证不会搜索很多的图块），是一种静态路网中最有效的直接搜索算法。

### 1.1.2 八数码问题

八数码问题包括了一个3x3的棋盘，棋盘上摆放着8个数字的棋子，留下一个空位。与空位相邻的棋子可以滑动到空位中。游戏的目的是要达到一个特定的目标状态。请见下图：

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image002.jpg)

### 1.1.3 实验简述

本实验要求以八数码问题为例，实现A*算法的求解程序，要求设计至少两种不同的启发函数。通过实验过程，熟悉和掌握启发式搜索策略的定义、评价函数和算法过程，并利用A*算法求解八数码问题，理解求解流程和搜索顺序。

 

## 1.2 实验内容

### 1.2.1 启发函数设计

本实验中要求以八数码问题为例，实现A*算法的求解程序，要求设计至少两种不同的启发函数。

### 1.2.2 结果比较与性能分析

​      本实验中需要进行结果对比。通过设置相同初始状态和目标状态，针对不同的评价函数求得问题的解，比较它们对搜索算法性能的影响，包括扩展节点数、生成节点数和运行时间等。要求画出结果比较的图表，并进行性能分析。

### 1.2.3 步骤可视化

​      要求界面显示初始状态，目标状态和中间搜索步骤。

###    1.2.4 流程可视化

要求显示搜索过程，画出搜索过程生成的搜索树，并在每个节点显示对应节点的评价值。标注出最终结果所选用的解路线。

## 1.3 本文所作的工作

本文将八数码问题和A*算法分为两个对象，分别对于问题的求解和算法的原理进行阐述，同时对于实验项目代码的具体架构进行解析。

（1）八数码问题算法的设计思路

（2）A*算法的工作原理

（3）项目的模块设计

（4）结果展示与性能分析

 



# 2 实验方案设计



## 2.1 总体设计思路与总体架构

### 2.1.1 总体设计思路

程序主要分为两大模块。A*搜索算法模块接受初始状态和目标状态的状态输入，负责验算状态的可达性和求解路径。GUI模块负责绘制程序的主要图形化人机交互界面，进行状态的输入，用户操作和日志输出。通过图形化界面读入用户自定义输入的起始状态和终结状态后，首先通过算法模块验证起始状态和终结状态的有效性与可解性。如果验证通过，状态有效并且理论可解，此时算法模块负责通过A*算法求解一条可行路径，将求解过程和求解结果输出到日志。最后，图形化界面负责进行算法步骤的可视化演示。

### 2.1.2 总体架构

​      程序的大致运行流程如下流程图所示：

 ![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image004.jpg)

GUI界面的控件状态转换通过状态机实现，状态机图如下：

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image006.jpg)

GUI界面设计与功能如下：

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image008.jpg)

​      

## 2.2 核心算法及基本原理

### 2.2.1 A*算法的基本原理及算法的代码实现

A*算法的主要步骤可以分解为如下若干：

（1）   算法开始时，将初始状态加入优先队列，设定其耗散值为0，通过启发函数得到启发值，从而计算得到其评价值

（2）   算法执行过程中，循环终止条件为优先队列为空或者当前状态与目标状态相同。每次循环中，取出优先队列的队首元素作为待扩展的结点。

（3）   访问该结点的后继结点，计算其评价值，如果该状态未被访问过则将该结点的状态和评价值加入优先队列。

（4）   重复上述2,3,4步骤，直到当前访问的结点状态与目标状态相同。

（5）   为了防止搜索层次过深导致的栈溢出，设定搜索耗散值大于阈值时算法退出。

本程序中，通过一个64位整形数存储当前棋盘的状态。八数码问题中，3x3的方格共有9种情况，分别为0~8，其中0表示空格。因此，每个数字可以用4位二进制数表示，总共需要占用4x9=36位的二进制位。因此，每个64位整形数利用低36位存储数据，其余位置零，这种存储方式比使用char类型数组存储少了8位数据，能够更加有效地利用存储空间。通过代码实现上述算法流程，有效地实现了A*算法求解八数码问题。

### 2.2.2 A*算法估值函数的代码实现

​      本算法采用四种启发函数，分别采用不同的启发算法得出估值。

（1）   采用计算两个表示棋盘数码之间的不同位数，再将不同位数除以4作为估计值的启发函数

（2）   采用计算两个棋盘相同位置上数码不同的位置个数的方式，将计算得出的个数作为估计值的启发函数

（3）   采用计算将棋盘展平后，数值相同的数码线性间隔的总和的方式，将计算得到的结果作为估计值的启发函数

（4）   采用计算棋盘上所有相同数码的曼哈顿距离的总和作为启发值的启发函数。

### 2.2.3 数码状态转移算法实现

算法中，每次生成下一状态的数码可以看做将空位与上下左右四个方位的数码进行交换。当访问当前状态后，分别遍历四个移动方向。如果该方向的移动是合法的，则通过交换两个位置上的数码实现移动，具体代码如下：

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image010.jpg)

​      

### 2.2.4 A*算法的路径存储

​      为了在算法结束后，能够从目标结点找到一条合法路径，因此维护一棵搜索树，树结点的结构定义如下：

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image012.jpg)

其中，children[i]为空表示空格向i方向移动的状态是不合法的。算法结束后，从目标状态结点开始自底向上地递归遍历父亲节点，最终在达到起始状态时，能够找到一条合法路径。具体算法如下图所示：

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image014.jpg)

### 2.2.5 A*算法的搜索树绘制

​      本程序使用QT的QPainter类实现搜索树的可视化展示。首先，由算法实现模块返回搜索树的根节点。随后，绘图模块使用队列对搜索树进行层次遍历，根据每个结点的孩子数量动态计算结点的方框坐标位置，随后对于每一个结点调用绘制方法实现绘图。由于屏幕窗口的大小有限，考虑到路径过长或者耗散值过大的搜索树显示效果不佳，因此绘图模块将不会绘制这些搜索树。同时，绘图模块根据搜索树深度，对于每层结点提供一个滑动条控件用于用户手动调整树层次的宽度，使得搜索树显示的元素位置更加合理地排布在界面中。

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image016.jpg)

## 2.3 模块设计

本程序根据实验的具体要求分为若干模块，具体划分与功能如下：

### 2.3.1 A*算法实现模块

​      本模块接受从GUI界面输入的初始状态和目标状态，负责验证状态的可达性和A*算法求解路径的相关算法实现，将运算结果返回到GUI模块。

### 2.3.2 GUI模块

​      本模块接受用户的自定义状态输入和自定义操作，通过界面控件与相关底层模块进行交互，并且输出数据到GUI界面。

### 2.3.3 日志模块

​      通过信号槽机制与GUI模块进行交互，负责程序运行时的日志输出管理。

### 2.3.4 绘图模块

​      根据搜索得到的结果进行搜索树的图形化绘制显示。

### 2.3.5 模块关系

​      具体的模块的交互关系如图：

 

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image018.jpg)

###    2.3.5 其他创新内容或优化算法

​      本程序在支持用户自定义输入的基础上，增加了随机起始状态和目标状态的功能，允许程序快速生成可解的一个八数码问题状态。同时，使用位数码的方式存储数据，通过位运算实现数据运算，理论上更有效地利用了存储空间，增加算法运行速度速度。同时设计了4中不同的估值函数，分别支持在运行时动态切换，能够有效地方便控制变量进行结果比较。最后大量使用线程并发技术，实现了程序算法运算、动态演示等状态下的流程运行，并且使用线程安全的方法、机制和对象，充分增强了程序的健壮性。

# 3 实验过程

## 3.1 环境说明  

### 3.1.1 操作系统

本开发环境基于Windows 10（build 22000.556）操作系统搭建。

### 3.1.2 开发语言

本程序使用C++语言开发。

### 3.1.3 开发环境

本程序使用QT Creator 6.0.2 + QT 5.1.12 + MinGW 32bit的开发环境进行开发。核心使用的IDE为QT Creator 6.0.2，GUI库为QT 5.1.12，交叉编译环境为MinGW 32bit，如果使用其它框架版本或者编译环境生成本项目可能会导致程序运行出现问题。

## 3.2 源代码文件和主要函数清单 

### 3.2.1 源代码文件

（1）mainwindow.cpp 主窗体模块文件

（2）astar.cpp 算法实现模块文件

（3）log.cpp 日志模块文件

（4）mainwindow.ui 主窗体控件样式文件

（5）qblock.cpp 数码块类重载QpushButton

（6）drawing.cpp 绘图类

### 3.2.2主要函数清单

​      主要函数详见附件头文件(.h/.hpp)。

## 3.3 实验结果展示     

### 3.3.1 算法性能评估

​      对于同一启发函数，面对最优路径长度不同的状态求解过程，分别测试求解速度与总耗散值（搜索次数）。

**启发函数1**：

| 初始状态              | 目标状态              | 路径长度 | 求解用时 | 求解过程总耗散 |
| --------------------- | --------------------- | -------- | -------- | -------------- |
| 3 1 8   6 5 4   7 0 2 | 3 1 8   2 5 4   6 0 7 | 10       | 0ms      | 215            |
| 4 5 1   0 6 3   7 2 8 | 1 6 5   8 2 7   4 3 0 | 21       | 22ms     | 22222          |
| 8 6 4   3 5 0   1 2 7 | 5 8 2   7 6 0   1 4 3 | 26       | 140ms    | 123089         |
| 2 0 7   4 6 8   5 1 3 | 3 2 5   6 8 7   0 1 4 | 23       | 40ms     | 42413          |
| 2 5 6   1 4 8   7 3 0 | 3 0 8   6 4 5   1 7 2 | 25       | 88ms     | 81302          |

**启发函数2**

| 初始状态              | 目标状态              | 路径长度 | 求解用时 | 求解过程总耗散 |
| --------------------- | --------------------- | -------- | -------- | -------------- |
| 3 1 8   6 5 4   7 0 2 | 3 1 8   2 5 4   6 0 7 | 10       | 0ms      | 60             |
| 4 5 1   0 6 3   7 2 8 | 1 6 5   8 2 7   4 3 0 | 21       | 4ms      | 5487           |
| 8 6 4   3 5 0   1 2 7 | 5 8 2   7 6 0   1 4 3 | 26       | 44ms     | 44595          |
| 2 0 7   4 6 8   5 1 3 | 3 2 5   6 8 7   0 1 4 | 23       | 10ms     | 12472          |
| 2 5 6   1 4 8   7 3 0 | 3 0 8   6 4 5   1 7 2 | 25       | 27ms     | 27966          |

**启发函数3**

| 初始状态              | 目标状态              | 路径长度 | 求解用时 | 求解过程总耗散 |
| --------------------- | --------------------- | -------- | -------- | -------------- |
| 3 1 8   6 5 4   7 0 2 | 3 1 8   2 5 4   6 0 7 | 10       | 0ms      | 480            |
| 4 5 1   0 6 3   7 2 8 | 1 6 5   8 2 7   4 3 0 | 21       | 43ms     | 37679          |
| 8 6 4   3 5 0   1 2 7 | 5 8 2   7 6 0   1 4 3 | 26       | 347ms    | 223569         |
| 2 0 7   4 6 8   5 1 3 | 3 2 5   6 8 7   0 1 4 | 23       | 55ms     | 45878          |
| 2 5 6   1 4 8   7 3 0 | 3 0 8   6 4 5   1 7 2 | 25       | 314ms    | 215138         |

**启发函数4**

| 初始状态              | 目标状态              | 路径长度 | 求解用时 | 求解过程总耗散 |
| --------------------- | --------------------- | -------- | -------- | -------------- |
| 3 1 8   6 5 4   7 0 2 | 3 1 8   2 5 4   6 0 7 | 10       | 1ms      | 67             |
| 4 5 1   0 6 3   7 2 8 | 1 6 5   8 2 7   4 3 0 | 21       | 26ms     | 24717          |
| 8 6 4   3 5 0   1 2 7 | 5 8 2   7 6 0   1 4 3 | 26       | 204ms    | 164674         |
| 2 0 7   4 6 8   5 1 3 | 3 2 5   6 8 7   0 1 4 | 23       | 30ms     | 30097          |
| 2 5 6   1 4 8   7 3 0 | 3 0 8   6 4 5   1 7 2 | 25       | 316ms    | 225980         |

对比分析4张性能表，可以发现四个启发函数对于最优路径长度不同的局面的求解效率高低是不同的。虽然在部分局面的搜索下效率低于启发函数3和4，但是就平均而言启发函数1和2的性能比较理想。同时，最优路径长度的增加会导致求解的耗散值急剧增加。

### 3.3.2 启发函数对于A*算法性能的影响

​    对于上述所有状态，不使用启发函数，只是用耗散函数进行A*搜索算法，结果如下：

**不使用启发函数：**

| 初始状态              | 目标状态              | 路径长度 | 求解用时 | 求解过程总耗散 |
| --------------------- | --------------------- | -------- | -------- | -------------- |
| 3 1 8   6 5 4   7 0 2 | 3 1 8   2 5 4   6 0 7 | 10       | 14ms     | 726            |
| 4 5 1   0 6 3   7 2 8 | 1 6 5   8 2 7   4 3 0 | 21       | 140ms    | 131478         |
| 8 6 4   3 5 0   1 2 7 | 5 8 2   7 6 0   1 4 3 | 26       | 465ms    | 383731         |
| 2 0 7   4 6 8   5 1 3 | 3 2 5   6 8 7   0 1 4 | 23       | 216ms    | 192946         |
| 2 5 6   1 4 8   7 3 0 | 3 0 8   6 4 5   1 7 2 | 25       | 424ms    | 348258         |

对比分析不使用启发函数的性能表和使用启发函数的上方四张性能表，可以发现使用启发函数能够大幅度地降低求解用时和求解过程总耗散。因此，可以得出结论，启发函数对于A*算法的效率优化起到了一个极其重要的作用。

### 3.3.3 不同启发函数对于同一问题求解的搜索树展示与对比

​      对于同一状态，使用不同的启发函数会导致生成不同的搜索树，进而影响算法求解问题的效率。下面就一个最优路径长度为5的路径搜索问题，分别展示不同启发函数生成的搜索树。首先，该搜索问题的初始状态和目标状态如下：

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image020.jpg)

使用启发函数1的日志输出：

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image022.jpg)

使用启发函数1的搜索树：

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image024.jpg)

使用启发函数2的日志输出：

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image026.jpg)

使用启发函数2的搜索树：

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image028.jpg)

使用启发函数3的日志输出：

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image030.jpg)

使用启发函数3的搜索树：

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image032.jpg)

使用启发函数4的日志输出：

![img](https://github.com/leo4048111/AstarDemo/blob/main/img/clip_image034.jpg)

使用启发函数4的搜索树：

***该搜索树宽度过宽，无法在有限的窗口大小下可视化画出。\***

对比上述搜索树和日志输出，可以发现对于同一搜索问题的求解中，使用不同的启发函数会生成宽度和深度均不同的搜索树，这反应了使用不同的评估方法也会较大地影响算法效率。在实际问题中，针对不同的搜索情景，我们应当进行多组实验对比，选择一个最为高效的启发函数。



# 4  总结

## 4.1 实验中存在的问题及解决方案

（1）实验初期首先进行在控制台界面的算法实现与测试，起初耗散值较大，算法运行速度并不理想甚至出现许多在给定耗散值下无法求解的状态，之后通过仔细研究A*算法的实现原理，改正了原先算法实现中的错误处，并且通过实验结果对比设计了若干效率较高的启发函数，最终达到了比较理想的算法效率。

（2）实验后期对于搜索树的图形化绘制实验了若干方案，包括根据搜索树宽度和深度动态绘制、可调整的节点大小等，最后根据具体的呈现效果使用了程序动态计算节点坐标+允许用户使用控件手动调整宽度的功能设计，实现了一个较为理想的搜索树绘制效果。

## 4.2 心得体会

本次实验中，通过复现A*算法解决八数码问题的算法，我一方面通过不断地算法改进优化，深入掌握了A*算法的原理与实现，真正地化抽象知识为具象，贯彻了一个计算机学习中的理论与实践结合的相关思想，有助于今后对于计算机相关领域的相关专业内容的进一步深入学习。

同时，通过实验我加强了项目能力与软件架构设计能力，通过合理划分程序功能模块，对于每个模块的功能进行实现，采用MVC的设计模式组织架构，尽量做到低耦合高内聚的软件设计准则，充分进一步锻炼了一个整体的编程能力与编程思维，提升了对于图形库的运用熟练程度，有益于个人计算机专业水平的整体化提升与技术栈的充实。

最后，通过实验我进一步加深了对于人工智能方向静态问题搜索方法的理解与认识，培养了对于人工智能方向算法研究的兴趣，加深了对于人工智能相关概念与知识点的理解与掌握，这将有所裨益于今后人工智能课程的学习。

## 4.3 后续改进方向

本次实验中，在搜索树显示的方面还是采取一个动态计算图形布局+允许手动调整的方式，并且在搜索树深度过深的时候显示效果不理想。因此，在后续改进中，必须对于这一点问题进行优化，采取一个有效的算法对于搜索树界面进行动态布局，实现一个更为美观的布局效果。同时，程序实现中有部分地方还有更加高效化的改进空间，通过合并接口、函数复用、解耦合等方法能够进一步提高程序运行的效率与健壮性。这些方面在后续的改进中都可以进一步进行优化。

## 4.4 源码

​      本实验项目源码开源在github站点，url：https://github.com/leo4048111/AstarDemo



# 参考文献

[1]算法分析与设计（第四版）

[2]老师上课的课件PPT

[3]QT开发文档https://doc.qt.io/qt.html#qt5

 