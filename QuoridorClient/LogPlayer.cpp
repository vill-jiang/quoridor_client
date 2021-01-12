#include "LogPlayer.h"
#include <iostream>
#include <csv.h>

namespace QuoridorUtils {

LogPlayer::LogPlayer(const std::string& key) : Player(key) {
    std::string csvFilename = "log_xxxxxxxx_xxxxxx.csv";
    try {
        io::CSVReader<7> in(csvFilename);
        in.read_header(io::ignore_extra_column, "player",
                       "newLoc_x", "newLoc_y",
                       "newBar_start_x", "newBar_start_y",
                       "newBar_stop_x", "newBar_stop_y");
        std::string playerName;
        QuoridorUtils::Step newStep;
        while (in.read_row(playerName, newStep.myNewLoc.x, newStep.myNewLoc.y,
                           newStep.myNewBlockBar.start.x, newStep.myNewBlockBar.start.y,
                           newStep.myNewBlockBar.stop.x, newStep.myNewBlockBar.stop.y)) {
            this->steps.emplace_back(newStep);
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
    this->stepIterator = this->steps.begin();
}

QuoridorUtils::Step LogPlayer::nextStep(const QuoridorUtils::ChessboardChange& newChange) {
    static bool started = false;
    if (!started) {
        if (newChange.myLoc == QuoridorUtils::PLAYER1_LOC) {
            this->stepIterator += 1;
            std::cout << "role: Enemy." << std::endl;
        } else {
            std::cout << "role: Me." << std::endl;
        }
        started = true;
    }
    std::cout << "recv:  " << newChange << std::endl;
    if (this->stepIterator == this->steps.end()) {
        std::cout << "empty" << std::endl;
        return Step();
    } else {
        const Step newStep = *this->stepIterator;
        std::cout << "trans: " << newStep << std::endl;
        if (this->stepIterator + 1 == this->steps.end()) {
            this->stepIterator += 1;
        } else {
            this->stepIterator += 2;
        }
        return newStep;
    }
}

void LogPlayer::restart() {
    this->stepIterator = this->steps.begin();
}
}
