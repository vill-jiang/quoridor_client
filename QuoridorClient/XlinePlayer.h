#pragma once
#include <queue>
#include <unordered_map>

#include "Player.h"

namespace QuoridorUtils {
class XlinePlayer : public Player {
protected:
    Location myInitLoc;
    int remainBlocks = BLOCK_SUM;
    int remainChance = 3;
    std::unordered_map<Location, Doors, LocationHash> chessboard;
    void chessboardInit();
    // 转换挡板到 chessboard, isDel 控制添加或删除, 默认为添加挡板  
    void addOrDelBlock(const BlockBar& block, const bool isDel = false);
    // 给定两个坐标和目标y值, 计算 myLoc 到达指定 y 值的最小距离与可达路径, 不可达则返回 INT32_MAX 
    // 注意: 暂时未使用 enemyLoc 
    // 其中 path 将先被清空, 再按照与 myLoc 距离非升序填充位置与距离 pair, 最小距离为1 
    int minDistance(const Location& startLoc, const Location& anotherLoc, const Location& stopLoc);
    int minDistancePath(const Location& startLoc, const Location& anotherLoc,
                        const Location& stopLoc, std::queue<std::pair<Location, int>>& path);
    int minDistancePath(const Location& startLoc, const Location& anotherLoc,
                        const int targetY, std::queue<std::pair<Location, int>>& path);
public:
    XlinePlayer(const std::string& key) : Player(key) { };
};
}
