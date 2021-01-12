#include <iostream>
#include <vector>
#include "Replayer.h"

namespace QuoridorUtils {

enum class Replayer::InputCode {
    Quit,
    Prev,
    Next,
};

Replayer::InputCode Replayer::userInput() {
    std::cout << "\33[2K" << "choose quit(q), prev(p) or next(n, \\n):  \b";
    char ch = '\0';
    do {
        std::cin.get(ch);
        if (ch != '\n') {
            std::cin.ignore(INT32_MAX, '\n');
        }
        switch(ch) {
        case 'q':
        case 'Q':
            return InputCode::Quit;
        case 'p':
        case 'P':
            return InputCode::Prev;
        case 'n':
        case 'N':
        case '\n':
            return InputCode::Next;
        default: ;
        }
    } while (true);
}

void Replayer::start(GameView& view, DataLoader& loader) {
    std::string player0Name, player1Name;
    loader.getPlayerName(player0Name, player1Name);
    view.playerRegister(player0Name, player1Name);
    const int steps = loader.getRemainingStep();

    std::vector<std::string> playerNameRounds;
    std::string tmpPlayerName;
    QuoridorUtils::Step tmpStep;
    QuoridorUtils::GameStatus tmpStatus;
    while (loader.getNextStep(tmpPlayerName, tmpStep, tmpStatus)) {
        view.putStep(tmpPlayerName, tmpStep, tmpStatus);
        playerNameRounds.emplace_back(tmpPlayerName);
    }

    InputCode input = InputCode::Quit;
    int stepCounter = 0;
    do {
        input = userInput();
        switch (input) {
        case InputCode::Quit:
            return;
        case InputCode::Next:
            if (!view.showNextStep()) {
                return;
            }
            stepCounter++;
            break;
        case InputCode::Prev:
            if (!view.showPrevStep()) {
                continue;
            }
            stepCounter--;
            break;
        }
    } while (true);
}

Replayer::~Replayer() {
    ;
}
}
