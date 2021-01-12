#pragma once
#include <string>
#include "../QuoridorUtils/QuoridorUtils.h"

namespace QuoridorUtils {
class GameView {
public:
    virtual ~GameView() = default;
    // 用户名预输入, 仅能两位玩家参与游戏.
    // firstPlayer 是坐标在 QuoridorUtils::PLAYER0_LOC 开始的玩家;
    //secondPlayer 是坐标在 QuoridorUtils::PLAYER1_LOC 开始的玩家;
    virtual void playerRegister(const std::string& firstPlayer, const std::string& secondPlayer) = 0;
    // 输入下一步建立地图, 若playerName错误则不建立. 
    virtual bool putStep(const std::string& playerName, const Step& nextStep, const GameStatus& status) = 0;
    virtual bool showNextStep() = 0;
    virtual bool showPrevStep() = 0;
};
}