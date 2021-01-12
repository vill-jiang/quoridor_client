#pragma once
#include "DefensivePlayer.h"
#include "RunPlayer.h"

namespace QuoridorUtils {
class DefensiveRunPlayer : public DefensivePlayer, public RunPlayer {
//private:
//    Location move(const Location& myLoc, const Location& enemyLoc);
public:
    DefensiveRunPlayer(const std::string& key) : XlinePlayer("193304DefensiveRun"), DefensivePlayer(key), RunPlayer(key) { };
    Step nextStep(const ChessboardChange& newChange) override;
    void restart() override;
};
}
