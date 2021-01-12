#pragma once

#include "XlinePlayer.h"

namespace QuoridorUtils {
class DefensivePlayer : virtual public XlinePlayer {
protected:
    // ģ����õ���, ����Է�·������Ŀ�ĵؾ�ֵ, ���� INT32_MIN ��ʾΥ�� 
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
