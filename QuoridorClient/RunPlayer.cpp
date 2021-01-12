#include "RunPlayer.h"

#include <iostream>

namespace QuoridorUtils {
Location RunPlayer::move(const Location& myLoc, const Location& enemyLoc) {
    std::queue<std::pair<Location, int>> myPath;
    minDistancePath(myLoc, enemyLoc, SIZE - this->myInitLoc.y + 1, myPath);
    Location bestNextLoc;
    if (myLoc.distance(enemyLoc) == 1) {  // 优先跳跃 
        std::vector<std::pair<Location, int>> tmpPath;  // 存储我方最短路径中2步可达位置 
        bool blocked = true;
        while (!myPath.empty()) {
            const auto tmp = myPath.front();
            myPath.pop();
            if (tmp.second >= 3) {
                continue;
            }
            if (tmp.first == enemyLoc && tmp.second == 1) {  // 敌我之间无挡板, 且最短路径第1步经过对方棋子 
                blocked = false;
            }
            tmpPath.emplace_back(tmp);
        }
        if (blocked) {  // 敌我之间有挡板, 必定不能跳, 此时step中下一步必定可行 
            for (const auto tmp : tmpPath) {
                if (tmp.second == 1) {
                    bestNextLoc = tmp.first;
                    break;
                }
            }
        } else {  // 敌我之间无挡板, 且最短路径经过对方棋子 
            Location jump;
            if (myLoc.x == enemyLoc.x) {  // 竖着跳 
                jump.x = myLoc.x;
                jump.y = enemyLoc.y + (enemyLoc.y - myLoc.y);
            } else if (myLoc.y == enemyLoc.y) {  // 横着跳 
                jump.x = enemyLoc.x + (enemyLoc.x - myLoc.x);
                jump.y = myLoc.y;
            }
            for (const auto tmp : tmpPath) {
                if (tmp.second == 2 && tmp.first == jump) {  // 将跳位置在最短路径内, 可跳 
                    bestNextLoc = jump;
                    break;
                }
            }
            if (!(bestNextLoc == jump)) { // 不可跳则需要选择另外的最短路径或静止不动 
                for (const auto tmp : tmpPath) {
                    if (tmp.second == 1 && !(tmp.first == enemyLoc)) {
                        bestNextLoc = tmp.first;
                        break;
                    }
                }
                if (!(bestNextLoc == jump)) {  // 最短路径只经过敌军棋子且不可跳 
                    if (remainChance > 1) {    // 不动, 违规一次 
                        remainChance--;
                    } else {                   // 必须走一步 
                        if (this->chessboard[myLoc].up && myLoc.y + 1 != enemyLoc.y) { // 可以向上走 
                            bestNextLoc = Location(myLoc.x, myLoc.y + 1);
                        } else if (this->chessboard[myLoc].down && myLoc.y - 1 != enemyLoc.y) {  // 可以向下走 
                            bestNextLoc = Location(myLoc.x, myLoc.y - 1);
                        } else if (this->chessboard[myLoc].right && myLoc.x + 1 != enemyLoc.x) {  // 可以向右走 
                            bestNextLoc = Location(myLoc.x + 1, myLoc.y);
                        } else if (this->chessboard[myLoc].left && myLoc.y - 1 != enemyLoc.y) {  // 可以向左走 
                            bestNextLoc = Location(myLoc.x - 1, myLoc.y);
                        } else {  // 不可能执行, 玩家被堵死 
                            remainChance--;
                        }
                    }
                }
            }
        }
    } else {  // 棋子不相邻, 最短路径的下一步必定可行 
        bestNextLoc = myPath.back().first;
    }
    return bestNextLoc;
}

QuoridorUtils::Step QuoridorUtils::RunPlayer::nextStep(const ChessboardChange& newChange) {
    if (this->myInitLoc.x <= 0 || this->myInitLoc.y <= 0) {
        this->myInitLoc = newChange.myLoc;
    }
    if (!newChange.newEnemyBlockBar.isNan()) {
        addOrDelBlock(newChange.newEnemyBlockBar);
    }
    Step step;
    step.myNewLoc = move(newChange.myLoc, newChange.enemyLoc);
    if (step.myNewLoc == newChange.myLoc || !step.isMove()) {
        remainChance--;
        step.myNewLoc = newChange.myLoc;
    }
    std::cout << newChange << "  ->  " << step << std::endl;
    return step;
}

void QuoridorUtils::RunPlayer::restart() {
    chessboardInit();
    myInitLoc.x = -1;
    myInitLoc.y = -1;
    remainBlocks = BLOCK_SUM;
    remainChance = 3;
}
}
