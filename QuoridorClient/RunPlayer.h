#pragma once
#include "XlinePlayer.h"

namespace QuoridorUtils {
class RunPlayer : virtual public XlinePlayer {
protected:
    Location move(const Location& myLoc, const Location& enemyLoc);
public:
    RunPlayer(const std::string& key) : XlinePlayer("193304Run") { };
    Step nextStep(const ChessboardChange& newChange) override;
    void restart() override;
};
}

