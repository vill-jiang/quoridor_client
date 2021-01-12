#include "DefensiveRunPlayer.h"

#include <iostream>

namespace QuoridorUtils {

Step DefensiveRunPlayer::nextStep(const ChessboardChange& newChange) {
    if (this->myInitLoc.x <= 0 || this->myInitLoc.y <= 0) {
        this->myInitLoc = newChange.myLoc;
    }
    if (!newChange.newEnemyBlockBar.isNan()) {
        addOrDelBlock(newChange.newEnemyBlockBar);
    }
    Step step;
    if (remainBlocks > 0) {
        // ≤‚ ‘∑≈µ≤∞Â
        const auto x = newChange.enemyLoc.x;
        const auto y = newChange.enemyLoc.y;
        std::vector<std::pair<BlockBar, int>> candidateBlocks;
        candidateBlocks.reserve(8);
        candidateBlocks.emplace_back(BlockBar(x - 2, y, x, y), 0);
        candidateBlocks.emplace_back(BlockBar(x - 1, y, x + 1, y), 0);
        candidateBlocks.emplace_back(BlockBar(x - 2, y - 1, x, y - 1), 0);
        candidateBlocks.emplace_back(BlockBar(x - 1, y - 1, x + 1, y - 1), 0);
        candidateBlocks.emplace_back(BlockBar(x, y - 2, x, y), 0);
        candidateBlocks.emplace_back(BlockBar(x, y - 1, x, y + 1), 0);
        candidateBlocks.emplace_back(BlockBar(x - 1, y - 2, x - 1, y), 0);
        candidateBlocks.emplace_back(BlockBar(x - 1, y - 1, x - 1, y + 1), 0);
        BlockBar bestBlock;
        int maxProfit = currentProfit(newChange.myLoc, newChange.enemyLoc) + 840;
        for (auto& candidateBlock : candidateBlocks) {
            candidateBlock.second = simulateBlockBar(newChange.myLoc, newChange.enemyLoc, candidateBlock.first);
            if (maxProfit < candidateBlock.second) {
                bestBlock = candidateBlock.first;
                maxProfit = candidateBlock.second;
            }
        }
        step.myNewBlockBar = bestBlock;
        if (!bestBlock.isNan()) {
            remainBlocks--;
            addOrDelBlock(bestBlock);
        } else {
            step.myNewLoc = RunPlayer::move(newChange.myLoc, newChange.enemyLoc);
            step.myNewBlockBar = BlockBar();
        }
    } else {
        step.myNewLoc = RunPlayer::move(newChange.myLoc, newChange.enemyLoc);
    }
    if (step.myNewBlockBar.isNan() && (step.myNewLoc == newChange.myLoc || !step.isMove())) {
        remainChance--;
        step.myNewLoc = newChange.myLoc;
    }
    std::cout << newChange << "  ->  " << step << std::endl;
    return step;
}

void DefensiveRunPlayer::restart() {
    chessboardInit();
    myInitLoc.x = -1;
    myInitLoc.y = -1;
    remainBlocks = BLOCK_SUM;
    remainChance = 3;
}
}
