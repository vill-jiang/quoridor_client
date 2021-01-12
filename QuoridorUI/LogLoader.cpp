#include <iostream>
#include "LogLoader.h"
#include "csv.h"

namespace QuoridorUtils {
const int LogLoader::COLUMNS_SUM = 8;
const std::string LogLoader::COLUMNS_NAME[COLUMNS_SUM] = { "player",
                                                 "newLoc_x", "newLoc_y", 
                                                 "newBar_start_x", "newBar_start_y",
                                                 "newBar_stop_x", "newBar_stop_y", "status" };

LogLoader::LogLoader(const std::string& csvFilename) {
    //fast-cpp-csv-parser
    try {
        io::CSVReader<COLUMNS_SUM> in(csvFilename);
        in.read_header(io::ignore_extra_column, COLUMNS_NAME[0], COLUMNS_NAME[1], COLUMNS_NAME[2],
                       COLUMNS_NAME[3], COLUMNS_NAME[4], COLUMNS_NAME[5], COLUMNS_NAME[6], COLUMNS_NAME[7]);
        std::string playerName;
        QuoridorUtils::Step newStep;
        int newStatus;
        QuoridorUtils::Location player0InitLoc;
        while (in.read_row(playerName, newStep.myNewLoc.x, newStep.myNewLoc.y,
                           newStep.myNewBlockBar.start.x, newStep.myNewBlockBar.start.y,
                           newStep.myNewBlockBar.stop.x, newStep.myNewBlockBar.stop.y, 
                           newStatus)) {
            if (playerName.empty()) {
                continue;
            }
            if (this->player0Name.empty()) {
                if (this->player1Name.empty() || this->player1Name != playerName) {
                    this->player0Name = playerName;
                }
            }
            if (this->player1Name.empty()) {
                if (this->player0Name.empty() || this->player0Name != playerName)
                    this->player1Name = playerName;
            }
            if (player0InitLoc.x <= 0 && player0InitLoc.y <= 0 && playerName == this->player0Name && newStep.isMove()) {
                player0InitLoc = newStep.myNewLoc;
            }
            if (playerName != this->player0Name && playerName != this->player1Name) {
                continue;
            }
            this->histories.emplace_back(playerName, newStep, QuoridorUtils::GameStatus(newStatus));
        }
        if (player0InitLoc.distance(QuoridorUtils::PLAYER0_LOC) > player0InitLoc.distance(QuoridorUtils::PLAYER1_LOC)) {
            this->player0Name.swap(this->player1Name);
        }
    } catch (io::error::can_not_open_file&) {
        std::cout << "can't find log file: " << csvFilename << std::endl;
        exit(-1);
    } catch (io::error::too_many_columns&) {
        std::cout << "log file have too many columns: " << csvFilename << std::endl;
        exit(-2);
    } catch (...) {
        std::cout << "log file error: " << csvFilename << std::endl;
        exit(-2);
    }
    this->historyIterator = this->histories.begin();
}

void LogLoader::getPlayerName(std::string& player0Name, std::string& player1Name) {
    player0Name = this->player0Name;
    player1Name = this->player1Name;
}

bool LogLoader::getNextStep(std::string& playerName, QuoridorUtils::Step& nextStep, 
                            QuoridorUtils::GameStatus& status) {
    if (this->historyIterator == this->histories.end()) {
        return false;
    } else {
        playerName = std::get<0>(*this->historyIterator);
        nextStep = std::get<1>(*this->historyIterator);
        status = std::get<2>(*this->historyIterator);
        ++this->historyIterator;
        return true;
    }
}

int LogLoader::getRemainingStep() {
    return std::distance(this->historyIterator, this->histories.end());
}

LogLoader::~LogLoader() {
    ;
}
}