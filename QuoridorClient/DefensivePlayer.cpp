#include "DefensivePlayer.h"

#include <iostream>

namespace QuoridorUtils {

int DefensivePlayer::currentProfit(const Location& myLoc, const Location& enemyLoc) {
    int beforeEnemyDistance = 0;
    int beforeEnemyTargetCount = 0;
    for (int i = 1; i <= SIZE; i++) {
        const auto tmpDistance = minDistance(enemyLoc, myLoc, Location(i, myInitLoc.y));
        if (tmpDistance != INT32_MAX && !(Location(i, SIZE - myInitLoc.y + 1) == myLoc)) {
            beforeEnemyTargetCount++;
            beforeEnemyDistance += tmpDistance;
        }
    }
    if (beforeEnemyTargetCount == 0) {
        return INT32_MIN;
    }
    return (beforeEnemyDistance * 2520) / beforeEnemyTargetCount;
}

int DefensivePlayer::simulateBlockBar(const Location& myLoc, const Location& enemyLoc, const BlockBar& fakeBlock) {
    if (fakeBlock.isNan()) {
        return INT32_MIN;
    }
    if (fakeBlock.isH() && fakeBlock.start.y > 0 && fakeBlock.start.y < SIZE) {  // y相同 
        // eg. ((0,1), (2,1)) will block (1,1) u, (2,1) u (1,2) d, (2,2) d
        const auto stopLoc = fakeBlock.start.x > fakeBlock.stop.x ? fakeBlock.start : fakeBlock.stop;
        if (!(this->chessboard[stopLoc].up && this->chessboard[Location(stopLoc.x - 1, stopLoc.y)].up &&
            this->chessboard[Location(stopLoc.x, stopLoc.y + 1)].down &&
            this->chessboard[Location(stopLoc.x - 1, stopLoc.y + 1)].down)) {
            return INT32_MIN;
        }// else if (!this->chessboard[stopLoc].left && !this->chessboard[Location(stopLoc.x - 1, stopLoc.y)].right && 
        //           !this->chessboard[Location(stopLoc.x, stopLoc.y + 1)].left && 
        //           !this->chessboard[Location(stopLoc.x - 1, stopLoc.y + 1)].right) {  // 不能交叉 
        //    return INT32_MIN;
        //}
    } else if (fakeBlock.isV() && fakeBlock.start.x > 0 && fakeBlock.start.x < SIZE) {  // x相同 
        // eg. ((2,1), (2,3)) will block (2,2) r, (2,3) r (3,2) l, (3,3) l
        const auto stopLoc = fakeBlock.start.y > fakeBlock.stop.y ? fakeBlock.start : fakeBlock.stop;
        if (!(this->chessboard[stopLoc].right && this->chessboard[Location(stopLoc.x, stopLoc.y - 1)].right &&
            this->chessboard[Location(stopLoc.x + 1, stopLoc.y)].left &&
            this->chessboard[Location(stopLoc.x + 1, stopLoc.y - 1)].left)) {
            return INT32_MIN;
        }// else if (!this->chessboard[stopLoc].down && !this->chessboard[Location(stopLoc.x, stopLoc.y - 1)].up &&
        //           !this->chessboard[Location(stopLoc.x + 1, stopLoc.y)].down &&
        //           !this->chessboard[Location(stopLoc.x + 1, stopLoc.y - 1)].up) {
        //    return INT32_MIN;
        //}
    } else {
        return INT32_MIN;
    }
    addOrDelBlock(fakeBlock);
    const auto profit = currentProfit(myLoc, enemyLoc);
    addOrDelBlock(fakeBlock, true);
    return profit;
}

Location DefensivePlayer::move(const Location& myLoc) {
    Location newLoc = myLoc;
    if (this->chessboard[myLoc].left) {
        newLoc.x--;
    } else if (this->chessboard[myLoc].right) {
        newLoc.x++;
    } else if (this->chessboard[myLoc].up) {
        newLoc.y++;
    } else if (this->chessboard[myLoc].down) {
        newLoc.y--;
    }
    return newLoc;
}

Step DefensivePlayer::nextStep(const ChessboardChange& newChange) {
    if (this->myInitLoc.x <= 0 || this->myInitLoc.y <= 0) {
        this->myInitLoc = newChange.myLoc;
    }
    if (!newChange.newEnemyBlockBar.isNan()) {
        addOrDelBlock(newChange.newEnemyBlockBar);
    }
    Step step;
    if (remainBlocks > 0) {
        // 测试放挡板
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
        } else if (remainChance > 1) {
            remainChance--;
            step.myNewLoc = newChange.myLoc;
        } else {
            step.myNewLoc = move(newChange.myLoc);
            step.myNewBlockBar = BlockBar();
        }
    } else {
        step.myNewLoc = move(newChange.myLoc);
    }
    std::cout << newChange << "  ->  " << step << std::endl;
    return step;
}

void DefensivePlayer::restart() {
    chessboardInit();
    myInitLoc.x = -1;
    myInitLoc.y = -1;
    remainBlocks = BLOCK_SUM;
    remainChance = 3;
}
}
