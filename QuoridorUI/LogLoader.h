#pragma once
#include "DataLoader.h"
#include <list>
#include <tuple>

namespace QuoridorUtils {
class LogLoader final : public DataLoader {
private:
    static const int COLUMNS_SUM;
    static const std::string COLUMNS_NAME[];
    std::string player0Name;
    std::string player1Name;
    std::list<std::tuple<std::string, QuoridorUtils::Step, QuoridorUtils::GameStatus>> histories;
    std::list<std::tuple<std::string, QuoridorUtils::Step, QuoridorUtils::GameStatus>>::iterator historyIterator;
public:
    LogLoader(const std::string& csvFilename);
    ~LogLoader() override;
    void getPlayerName(std::string& player0Name, std::string& player1Name) override;
    bool getNextStep(std::string& playerName, QuoridorUtils::Step& nextStep, 
                     QuoridorUtils::GameStatus& status) override;
    int getRemainingStep() override;
};
}
