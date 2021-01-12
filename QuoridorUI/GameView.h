#pragma once
#include <string>
#include "../QuoridorUtils/QuoridorUtils.h"

namespace QuoridorUtils {
class GameView {
public:
    virtual ~GameView() = default;
    // �û���Ԥ����, ������λ��Ҳ�����Ϸ.
    // firstPlayer �������� QuoridorUtils::PLAYER0_LOC ��ʼ�����;
    //secondPlayer �������� QuoridorUtils::PLAYER1_LOC ��ʼ�����;
    virtual void playerRegister(const std::string& firstPlayer, const std::string& secondPlayer) = 0;
    // ������һ��������ͼ, ��playerName�����򲻽���. 
    virtual bool putStep(const std::string& playerName, const Step& nextStep, const GameStatus& status) = 0;
    virtual bool showNextStep() = 0;
    virtual bool showPrevStep() = 0;
};
}