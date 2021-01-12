#pragma once
#include "GameView.h"
#include "DataLoader.h"

namespace QuoridorUtils {
class Replayer {
    enum class InputCode;
private:
    static InputCode userInput();
public:
    ~Replayer();
    void start(GameView& view, DataLoader& loader);
};
}
