#pragma once

#include "XlinePlayer.h"

namespace QuoridorUtils {
class DefensivePlayer : virtual public XlinePlayer {
protected:
    // 模拟放置挡板, 计算对方路径到达目的地均值, 返回 INT32_MIN 表示违规 
    int currentProfit(const Location& myLoc, const Location& enemyLoc);
    int simulateBlockBar(const Location& myLoc, const Location& enemyLoc,
                         const BlockBar& fakeBlock);
    Location move(const Location& myLoc);
public:
    DefensivePlayer(const std::string& key) : XlinePlayer("193304Defensive") { };
    Step nextStep(const ChessboardChange& newChange) override;
    void restart() override;
};
}
