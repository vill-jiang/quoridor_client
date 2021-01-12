#include "RunPlayer.h"

#include <iostream>

namespace QuoridorUtils {
Location RunPlayer::move(const Location& myLoc, const Location& enemyLoc) {
    std::queue<std::pair<Location, int>> myPath;
    minDistancePath(myLoc, enemyLoc, SIZE - this->myInitLoc.y + 1, myPath);
    Location bestNextLoc;
    if (myLoc.distance(enemyLoc) == 1) {  // ������Ծ 
        std::vector<std::pair<Location, int>> tmpPath;  // �洢�ҷ����·����2���ɴ�λ�� 
        bool blocked = true;
        while (!myPath.empty()) {
            const auto tmp = myPath.front();
            myPath.pop();
            if (tmp.second >= 3) {
                continue;
            }
            if (tmp.first == enemyLoc && tmp.second == 1) {  // ����֮���޵���, �����·����1�������Է����� 
                blocked = false;
            }
            tmpPath.emplace_back(tmp);
        }
        if (blocked) {  // ����֮���е���, �ض�������, ��ʱstep����һ���ض����� 
            for (const auto tmp : tmpPath) {
                if (tmp.second == 1) {
                    bestNextLoc = tmp.first;
                    break;
                }
            }
        } else {  // ����֮���޵���, �����·�������Է����� 
            Location jump;
            if (myLoc.x == enemyLoc.x) {  // ������ 
                jump.x = myLoc.x;
                jump.y = enemyLoc.y + (enemyLoc.y - myLoc.y);
            } else if (myLoc.y == enemyLoc.y) {  // ������ 
                jump.x = enemyLoc.x + (enemyLoc.x - myLoc.x);
                jump.y = myLoc.y;
            }
            for (const auto tmp : tmpPath) {
                if (tmp.second == 2 && tmp.first == jump) {  // ����λ�������·����, ���� 
                    bestNextLoc = jump;
                    break;
                }
            }
            if (!(bestNextLoc == jump)) { // ����������Ҫѡ����������·����ֹ���� 
                for (const auto tmp : tmpPath) {
                    if (tmp.second == 1 && !(tmp.first == enemyLoc)) {
                        bestNextLoc = tmp.first;
                        break;
                    }
                }
                if (!(bestNextLoc == jump)) {  // ���·��ֻ�����о������Ҳ����� 
                    if (remainChance > 1) {    // ����, Υ��һ�� 
                        remainChance--;
                    } else {                   // ������һ�� 
                        if (this->chessboard[myLoc].up && myLoc.y + 1 != enemyLoc.y) { // ���������� 
                            bestNextLoc = Location(myLoc.x, myLoc.y + 1);
                        } else if (this->chessboard[myLoc].down && myLoc.y - 1 != enemyLoc.y) {  // ���������� 
                            bestNextLoc = Location(myLoc.x, myLoc.y - 1);
                        } else if (this->chessboard[myLoc].right && myLoc.x + 1 != enemyLoc.x) {  // ���������� 
                            bestNextLoc = Location(myLoc.x + 1, myLoc.y);
                        } else if (this->chessboard[myLoc].left && myLoc.y - 1 != enemyLoc.y) {  // ���������� 
                            bestNextLoc = Location(myLoc.x - 1, myLoc.y);
                        } else {  // ������ִ��, ��ұ����� 
                            remainChance--;
                        }
                    }
                }
            }
        }
    } else {  // ���Ӳ�����, ���·������һ���ض����� 
        bestNextLoc = myPath.back().first;
    }
    return bestNextLoc;
}

QuoridorUtils::Step QuoridorUtils::RunPlayer::nextStep(const ChessboardChange& newChange) {
    if (this->myInitLoc.x <= 0 || this->myInitLoc.y <= 0) {
        this->myInitLoc = newChange.myLoc;
    }
    if (!newChange.newEnemyBlockBar.isNan()) {
        addOrDelBlock(newChange.newEnemyBlockBar);
    }
    Step step;
    step.myNewLoc = move(newChange.myLoc, newChange.enemyLoc);
    if (step.myNewLoc == newChange.myLoc || !step.isMove()) {
        remainChance--;
        step.myNewLoc = newChange.myLoc;
    }
    std::cout << newChange << "  ->  " << step << std::endl;
    return step;
}

void QuoridorUtils::RunPlayer::restart() {
    chessboardInit();
    myInitLoc.x = -1;
    myInitLoc.y = -1;
    remainBlocks = BLOCK_SUM;
    remainChance = 3;
}
}
