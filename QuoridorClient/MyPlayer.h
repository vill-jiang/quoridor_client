#pragma once
#include <chrono>
#include <vector>
#include "Player.h"

namespace QuoridorUtils {
class MyPlayer final : public Player{
private:
    std::vector<BlockBar> blocks;                                           // 实例所需要, 可删 
    int targetY = 0;                                                        // 实例所需要, 可删 
    Location randomWalk(const Location& myLoc, const Location& enemyLoc);   // 实例所需要, 可删 
public:
    MyPlayer(const std::string& key) : Player(key) { };           // 必须存在, 请勿修改 
    Step nextStep(const ChessboardChange& newChange) override;    // 必须自行实现 
    void restart() override;                                      // 必须自行实现 
};
}

// 面向过程
//namespace QuoridorUtils {
//class MyPlayer final : public Player {
//public:
//    MyPlayer(const std::string& key) : Player(key) { };          // 必须存在 
//    Step nextStep(const ChessboardChange& newChange) override;   // 必须自行实现 
//    void restart() override;                                     // 必须自行实现 
//};
//}

/*
 *
 * 棋子坐标如下:                 挡板坐标结构如下：
 * ┌-┬-┬-┬-┬-┬-┬-┬-┬-┐         ┌-┬-┬-┬-┬-┬-┬-┬-┬-┐9
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖9       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤8
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖8       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤7
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖7       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤6
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖6       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤5
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖5       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤4
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖4       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤3
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖3       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤2
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖2       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤1
 * └-┴-┴-┴-┴-┴-┴-┴-┴-┘↖1 y     └-┴-┴-┴-┴-┴-┴-┴-┴-┘0 y
 *x 1 2 3 4 5 6 7 8 9         x0 1 2 3 4 5 6 7 8 9
 *
 *
 */