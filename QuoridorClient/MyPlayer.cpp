#include "MyPlayer.h"

#include <iostream>
#include <cstdlib>

// 面向过程实现开始
//using namespace QuoridorUtils;
//int targetY = 0;
//std::vector<QuoridorUtils::BlockBar> blocks;
//QuoridorUtils::Location randomWalk(const QuoridorUtils::Location& myLoc, const QuoridorUtils::Location& enemyLoc) {
//    bool directions[4];
//    directions[0] = myLoc.y < QuoridorUtils::SIZE;           // 是否可以向上走 
//    directions[1] = myLoc.y > 1;                             // 是否可以向下走 
//    directions[2] = myLoc.x > 1;                             // 是否可以向左走 
//    directions[3] = myLoc.x < QuoridorUtils::SIZE;           // 是否可以向右走 
//    for (auto block : blocks) {                        // 遍历挡板列表, 找到被阻挡的方向 
//        if (block.isH()) {                                   // 水平方向挡板, start 与 stop 的 y 相等 
//            if (block.start.x == myLoc.x - 1 ||
//                block.start.x == myLoc.x - 2) {              // 可能挡路的挡板 
//                if (block.start.y == myLoc.y) {
//                    directions[0] = false;
//                } else if (block.start.y == myLoc.y - 1) {
//                    directions[1] = false;
//                }
//            }
//        }
//        if (block.isV()) {                                   // 竖直方向挡板, start 与 stop 的 x 相等 
//            if (block.start.y == myLoc.y - 1 ||
//                block.start.y == myLoc.y - 2) {              // 可能挡路的挡板 
//                if (block.start.x == myLoc.x) {
//                    directions[3] = false;
//                } else if (block.start.x == myLoc.x - 1) {
//                    directions[2] = false;
//                }
//            }
//        }
//    }
//    int optionsCount = directions[0] + directions[1] + 
//                       directions[2] + directions[3];        // 规则不能堵死棋子, 必定大于等于1 
//    int tmp = rand() % optionsCount;                         // 随机一个 [0, optionsCount - 1] 的整数
//    int directionIndex;                                      // 找出随机方向的下标 
//    for (directionIndex = 0; directionIndex < 4; directionIndex++) {
//        if (directions[directionIndex]) {
//            if (tmp <= 0) {
//                break;
//            }
//            tmp--;
//        }
//    }
//    QuoridorUtils::Location nextLoc = myLoc;
//    switch (directionIndex) {
//    case 0:   // 向上走
//        nextLoc.y++;
//        break;
//    case 1:   // 向下走
//        nextLoc.y--;
//        break;
//    case 2:   // 向左走
//        nextLoc.x--;
//        break;
//    case 3:   // 向右走
//        nextLoc.x++;
//        break;
//    default: ;
//    }
//    if (nextLoc.distance(enemyLoc) == 0) {                   // 下一步是敌军位置则应跳过敌军棋子 
//        nextLoc.x = -1;                                      // 此处不做实现, 默认下一步空操作 
//        nextLoc.y = -1;
//    }
//    return nextLoc;
//}
// 面向过程实现结束

namespace QuoridorUtils {

/*
 * Step MyPlayer::nextStep(const ChessboardChange& newChange)
 *
 * newChange 为修改的棋局, 其格式为
 * (GameStatus status, Location enemyLoc, Location myLoc, BlockBar newEnemyBlockBar)
 * 其中棋子坐标和挡板坐标见下图, 当对手没有添加新的挡板时,
 * newChange.newEnemyBlockBar.isNan()为true
 * 
 * 需要返回的数据Step为
 * (Location myNewLoc, BlockBar myNewBlockBar)
 * 仅能选择让移动棋子一步（前后左右四个方向）或者放一个挡板:
 * 当移动棋子时, step.myNewBlockBar.isNan()为true; 
 * 当放置挡板时, step.myNewLoc为newChange中的myLoc
 *
 * 所有错误(挡板位置错误, 挡板已用完, 移动位置错误, 同时移动并放置挡板, 超时等)均由服务器检测,
 * 可能需要在自行实现错误避免功能. 
 *
 *  棋子坐标如下:                 挡板坐标结构如下：
 *  ┌-┬-┬-┬-┬-┬-┬-┬-┬-┐         ┌-┬-┬-┬-┬-┬-┬-┬-┬-┐9
 *  ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖9       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤8
 *  ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖8       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤7
 *  ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖7       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤6
 *  ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖6       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤5
 *  ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖5       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤4
 *  ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖4       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤3
 *  ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖3       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤2
 *  ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖2       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤1
 *  └-┴-┴-┴-┴-┴-┴-┴-┴-┘↖1 y     └-┴-┴-┴-┴-┴-┴-┴-┴-┘0 y
 * x 1 2 3 4 5 6 7 8 9        x 0 1 2 3 4 5 6 7 8 9
 *
 */

// 面向对象实现开始
Location MyPlayer::randomWalk(const Location& myLoc, const Location& enemyLoc) {
    bool directions[4];
    directions[0] = myLoc.y < SIZE;                          // 是否可以向上走 
    directions[1] = myLoc.y > 1;                             // 是否可以向下走 
    directions[2] = myLoc.x > 1;                             // 是否可以向左走 
    directions[3] = myLoc.x < SIZE;                          // 是否可以向右走 
    for (auto block : this->blocks) {                        // 遍历挡板列表, 找到被阻挡的方向 
        if (block.isH()) {                                   // 水平方向挡板, start 与 stop 的 y 相等 
            if (block.start.x == myLoc.x - 1 ||
                block.start.x == myLoc.x - 2) {              // 可能挡路的挡板 
                if (block.start.y == myLoc.y) {
                    directions[0] = false;
                } else if (block.start.y == myLoc.y - 1) {
                    directions[1] = false;
                }
            }
        }
        if (block.isV()) {                                   // 竖直方向挡板, start 与 stop 的 x 相等 
            if (block.start.y == myLoc.y - 1 ||
                block.start.y == myLoc.y - 2) {              // 可能挡路的挡板 
                if (block.start.x == myLoc.x - 1) {
                    directions[2] = false;
                } else if (block.start.x == myLoc.x) {
                    directions[3] = false;
                }
            }
        }
    }
    int optionsCount = int(directions[0]) + int(directions[1]) + 
                       int(directions[2]) + int(directions[3]);    // 规则中不能堵死棋子, 必定大于等于1
    int tmp = rand() % optionsCount;                               // 随机一个 [0, optionsCount - 1] 的整数
    int directionIndex;                                            // 找出随机方向的下标 
    for (directionIndex = 0; directionIndex < 4; directionIndex++) {
        if (directions[directionIndex]) {
            if (tmp <= 0) {
                break;
            }
            tmp--;
        }
    }
    Location nextLoc = myLoc;
    switch (directionIndex) {
    case 0:   // 向上走
        nextLoc.y++;
        break;
    case 1:   // 向下走
        nextLoc.y--;
        break;
    case 2:   // 向左走
        nextLoc.x--;
        break;
    case 3:   // 向右走
        nextLoc.x++;
        break;
    default: ;
    }
    if (nextLoc.distance(enemyLoc) == 0) {                   // 下一步是敌军位置则应跳过敌军棋子 
        nextLoc.x = -1;                                      // 此处不做实现, 默认下一步空操作 
        nextLoc.y = -1;
    }
    return nextLoc;
}

Step MyPlayer::nextStep(const ChessboardChange& newChange) {
    // 须知: 本示例代码仅作为 API 使用以及游戏规则了解范例, 甚至保证不了正确性, 切勿照抄照搬 
    if (this->targetY == 0) {                                // 判断并记录自己的目标 
        const auto d = std::chrono::system_clock::now().time_since_epoch();
        const auto nanosecond = int((std::chrono::nanoseconds(d).count() / 100) % int64_t(RAND_MAX));
        srand(nanosecond);
        if (newChange.myLoc == PLAYER0_LOC) {                // 己方初始坐标为 (5,1) 则目标为 (x,9) 
            this->targetY = PLAYER1_LOC.y;
        } else {                                             // 己方初始坐标为 (5,9) 则目标为 (x,1) 
            this->targetY = PLAYER0_LOC.y;
        }
    }
    std::cout << newChange;                                  // 输出接收到的数据到控制台显示 
    if (!newChange.newEnemyBlockBar.isNan()) {               // 对方放置了挡板 
        BlockBar tmp = newChange.newEnemyBlockBar;
        tmp.normalization();                                 // 规范为 start 坐标小于 stop 坐标的形式 
        this->blocks.push_back(tmp);                         // 存储规范挡板 
    }
    Step step;                                               // 初始化 step 默认不移动不放板 
    step.myNewLoc = randomWalk(newChange.myLoc, newChange.enemyLoc);
    std::cout << " -> " << step << std::endl;                // 输出我的决策到控制台显示 
    // 输入决策 
    // std::cout << " ->  ";
    // std::cin >> step.myNewLoc.x >> step.myNewLoc.y
    //          >> step.myNewBlockBar.start.x >> step.myNewBlockBar.start.y
    //          >> step.myNewBlockBar.stop.x >> step.myNewBlockBar.stop.y;
    return step;
}

void MyPlayer::restart() {
    this->blocks.clear();
    this->targetY = 0;
}
// 面向对象实现结束 

// 面向过程实现开始 
//Step MyPlayer::nextStep(const ChessboardChange& newChange) {
//    // 须知: 本示例代码仅作为 API 使用以及游戏规则了解范例, 甚至保证不了正确性, 切勿照抄照搬 
//    if (targetY == 0) {                                // 判断并记录自己的目标
//        //const auto d = std::chrono::system_clock::now().time_since_epoch();
//        //const int nanosecond = (std::chrono::nanoseconds(d).count() / 100) % int64_t(INT32_MAX);
//        //srand(nanosecond);
//        if (newChange.myLoc == PLAYER0_LOC) {                // 己方初始坐标为 (5,1) 则目标为 (x,9)
//            targetY = PLAYER1_LOC.y;
//        } else {                                             // 己方初始坐标为 (5,9) 则目标为 (x,1)
//            targetY = PLAYER0_LOC.y;
//        }
//    }
//    std::cout << newChange << std::endl;                     // 输出接收到的数据到控制台显示  
//    if (!newChange.newEnemyBlockBar.isNan()) {               // 对方放置了挡板 
//        BlockBar tmp = newChange.newEnemyBlockBar;
//        tmp.normalization();                                 // 规范为 start 坐标小于 stop 坐标的形式 
//        blocks.push_back(tmp);                         // 存储挡板 
//    }
//    Step step;                                               // 初始化 step 默认不移动不放板 
//    step.myNewLoc = randomWalk(newChange.myLoc, newChange.enemyLoc);
//    std::cout << step << std::endl;                          // 输出我的决策到控制台显示 
//    return step;
//}
//
//void MyPlayer::restart() {
//    blocks.clear();
//    targetY = 0;
//}
// 面向过程实现结束
}
